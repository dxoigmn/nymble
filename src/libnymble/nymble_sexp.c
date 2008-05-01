#include "nymble_sexp.h"
#include "unistd.h"
#include "assert.h"

extern int characterTablesInitialized;
extern char upper[256];            /* upper[c] is upper case version of c */
extern char whitespace[256];       /* whitespace[c] is nonzero if c is whitespace */
extern char decdigit[256];         /* decdigit[c] is nonzero if c is a dec digit */
extern char decvalue[256];         /* decvalue[c] is value of c as dec digit */
extern char hexdigit[256];         /* hexdigit[c] is nonzero if c is a hex digit */
extern char hexvalue[256];         /* hexvalue[c] is value of c as a hex digit */
extern char base64digit[256];      /* base64char[c] is nonzero if c is base64 digit */
extern char base64value[256];      /* base64value[c] is value of c as base64 digit */
extern char tokenchar[256];        /* tokenchar[c] is true if c can be in a token */
extern char alpha[256];            /* alpha[c] is true if c is alphabetic A-Z a-z */

void ssPutChar(sexpOutputStream *os, int c) {
  appendCharToSimpleString(c, (sexpSimpleString *)os->outputFile);
  os->column++;
}

void strGetChar(sexpStringInputStream *sis) { 
  int c;
  sexpInputStream *is = &sis->stream; 
  
  if (is->nextChar == EOF) { 
    is->byteSize = 8;
    return;
  }
  
  while (TRUE) { 
    c = is->nextChar = sis->string[sis->pos++];
    
    if (sis->pos == sis->len) {
      c = is->nextChar = EOF;
    }
    
    if (c == EOF) 
      return;
      
    if ((is->byteSize == 6 && (c == '|' || c == '}')) || (is->byteSize == 4 && (c == '#'))) {
      /* end of region reached; return terminating character, after
      checking for unused bits */  
      
      if (is->nBits>0 && (((1<<is->nBits)-1) & is->bits) != 0)
        ErrorMessage(WARNING, "%d-bit region ended with %d unused bits left-over", is->byteSize,is->nBits);
    
      changeInputByteSize(is,8);
      
      return;
    } else if (is->byteSize != 8 && isWhiteSpace(c)) 
      ; /* ignore white space in hex and base64 regions */
    else if (is->byteSize == 6 && c == '=') 
      ; /* ignore equals signs in base64 regions */
    else if (is->byteSize==8) { 
      is->count++; 
      return;
    }
    else if (is->byteSize < 8) { 
      is->bits = is->bits << is->byteSize;
      is->nBits += is->byteSize;
      if (is->byteSize == 6 && isBase64Digit(c))
        is->bits = is->bits | base64value[c];
      else if (is->byteSize == 4 && isHexDigit(c))
        is->bits = is->bits | hexvalue[c];
      else
        ErrorMessage(ERROR, "character %c found in %d-bit coding region", (int) is->nextChar, is->byteSize);
      if (is->nBits >= 8) { 
        is->nextChar = (is->bits >> (is->nBits-8)) & 0xFF;
        is->nBits -= 8;
        is->count++;
        return;
      }
    }
  }
}

sexpStringInputStream *newSexpStringInputStream(char *string, int len) {
  sexpStringInputStream *sis = (sexpStringInputStream *)sexpAlloc(sizeof(sexpStringInputStream));
  
  sis->stream.nextChar = ' ';
  sis->stream.getChar = strGetChar;
  sis->stream.count = -1;
  sis->stream.byteSize = 8;
  sis->stream.bits = 0;
  sis->stream.nBits = 0;
  sis->stream.inputFile = NULL; // and stay down!
  
  sis->string = string;
  sis->pos = 0;
  sis->len = len;
  
  return sis;
}

void printTicket(ticket_t *ticket) {  
  fprintf(stderr, "server_id: ");
  for (int i = 0; i < DIGEST_SIZE; i++) {
    fprintf(stderr, "%02x", ticket->server_id[i]);
  }
  fprintf(stderr, "\ntime_period: %d\nlink_window: %d\nnymble: ", ticket->time_period, ticket->link_window);
  for (int i = 0; i < DIGEST_SIZE; i++) {
    fprintf(stderr, "%02x", ticket->nymble[i]);
  }
  fprintf(stderr, "\ntrapdoorenc: ");
  for (int i = 0; i < TRAPDOORENC_SIZE; i++) {
    fprintf(stderr, "%02x", ticket->trapdoorenc[i]);
  }
  fprintf(stderr, "\nmac_n: ");
  for (int i = 0; i < DIGEST_SIZE; i++) {
    fprintf(stderr, "%02x", ticket->mac_n[i]);
  }
  fprintf(stderr, "\nmac_ns: ");
  for (int i = 0; i < DIGEST_SIZE; i++) {
    fprintf(stderr, "%02x", ticket->mac_ns[i]);
  }
  fprintf(stderr, "\n");
  
}
  
OBJ_TO_SEXP_FN_START(ticket)

  SEXP_LIST_ADDPAIR(ticket_outer_list, server_id, ticket->server_id, DIGEST_SIZE);

  char *time_period_buf = malloc(11); sprintf(time_period_buf, "%d", ticket->time_period);
  char *link_window_buf = malloc(11); sprintf(link_window_buf, "%d", ticket->link_window);

  SEXP_LIST_ADDPAIR(ticket_outer_list, time_period, (u_char *)time_period_buf, strlen(time_period_buf));
  SEXP_LIST_ADDPAIR(ticket_outer_list, link_window, (u_char *)link_window_buf, strlen(link_window_buf));

  free(time_period_buf);
  free(link_window_buf);

  SEXP_LIST_ADDPAIR(ticket_outer_list, nymble, ticket->nymble, DIGEST_SIZE);
  SEXP_LIST_ADDPAIR(ticket_outer_list, trapdoorenc, ticket->trapdoorenc, TRAPDOORENC_SIZE);
  SEXP_LIST_ADDPAIR(ticket_outer_list, mac_n, ticket->mac_n, DIGEST_SIZE);
  SEXP_LIST_ADDPAIR(ticket_outer_list, mac_ns, ticket->mac_ns, DIGEST_SIZE);

OBJ_TO_SEXP_FN_END(ticket)


SEXP_TO_OBJ_FN_START(ticket)
  
  SEXP_TO_OBJ_ATTR_PAIR_START(ticket)
  
    // All our values are strings (i.e., not lists)
    if (!isObjectString(value)) {
      return NULL;
    }
    u_char *valueptr = simpleStringString(sexpStringString(value));
  
    if (memcmp(keystr, "server_id", strlen("server_id")) == 0) {
      memcpy(ticket->server_id, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "time_period", strlen("time_period")) == 0) {
      sscanf((char *)valueptr, "%ud", &ticket->time_period);
    } else if (memcmp(keystr, "link_window", strlen("link_window")) == 0) {
      sscanf((char *)valueptr, "%ud", &ticket->link_window);      
    } else if (memcmp(keystr, "nymble", strlen("nymble")) == 0) {
      memcpy(ticket->nymble, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "trapdoorenc", strlen("trapdoorenc")) == 0) {
      memcpy(ticket->trapdoorenc, valueptr, TRAPDOORENC_SIZE);
    } 
    // The ordering of the following two cases is important, for now. FIXME: should check with 
    // length of the simplestring instead of the length of the value we're looking for.
    else if (memcmp(keystr, "mac_ns", strlen("mac_ns")) == 0) {
      memcpy(ticket->mac_ns, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "mac_n", strlen("mac_n")) == 0) {
      memcpy(ticket->mac_n, valueptr, DIGEST_SIZE);
    } else {
      return NULL;
    }
      
  SEXP_TO_OBJ_ATTR_PAIR_END(ticket)

SEXP_TO_OBJ_FN_END(ticket)


OBJ_TO_STR_FN(ticket)

STR_TO_OBJ_FN(ticket)

/*******************************************************/

OBJ_TO_SEXP_FN_START(pseudonym)

  SEXP_LIST_ADDPAIR(pseudonym_outer_list, pseudonym, (u_char *)pseudonym->pseudonym, DIGEST_SIZE);
  SEXP_LIST_ADDPAIR(pseudonym_outer_list, mac_np, (u_char *)pseudonym->mac_np, DIGEST_SIZE);
  
OBJ_TO_SEXP_FN_END(pseudonym)

SEXP_TO_OBJ_FN_START(pseudonym)

  SEXP_TO_OBJ_ATTR_PAIR_START(pseudonym)

    // All our values are strings (i.e., not lists)
    if (!isObjectString(value)) {
      return NULL;
    }
    u_char *valueptr = simpleStringString(sexpStringString(value));
  
    if (memcmp(keystr, "pseudonym", strlen("pseudonym")) == 0) {
      memcpy(pseudonym->pseudonym, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "mac_np", strlen("mac_np")) == 0) {
      memcpy(pseudonym->mac_np, valueptr, DIGEST_SIZE);
    } else {
      return NULL;
    }
  
  SEXP_TO_OBJ_ATTR_PAIR_END(pseudonym)

SEXP_TO_OBJ_FN_END(pseudonym)

OBJ_TO_STR_FN(pseudonym)

STR_TO_OBJ_FN(pseudonym)

/*******************************************************/

OBJ_TO_SEXP_FN_START(credential)

  SEXP_LIST_ADDPAIR(credential_outer_list, seed, credential->seed, DIGEST_SIZE);

  sexpList *tickets_list = newSexpList();
  sexpSimpleString *tickets_ss = newSimpleString();
  free(tickets_ss->string);
  tickets_ss->length = tickets_ss->allocatedLength = strlen("tickets");
  tickets_ss->string = (u_char *)strdup("tickets");
  sexpString *tickets_s = newSexpString();
  setSexpStringString(tickets_s, tickets_ss);
  sexpAddSexpListObject(tickets_list, tickets_s);
  
  ticket_t *ticket = credential->tickets + 1; // FIXME: this +1 seems unnecessary, and means we're wasting the first ticket in each credential

  sexpList *tickets = newSexpList();
  
  while (ticket) {
    sexpAddSexpListObject(tickets, ticket_to_sexp(ticket));
    
    //fprintf(stderr, "%s", ticket_to_str(ticket, ADVANCED)->string);
    
    ticket = ticket->next;
  }
  
  sexpAddSexpListObject(tickets_list, tickets);
  
  sexpAddSexpListObject(credential_outer_list, tickets_list);

OBJ_TO_SEXP_FN_END(credential)


SEXP_TO_OBJ_FN_START(credential)

  SEXP_TO_OBJ_ATTR_PAIR_START(credential)

    if (memcmp(keystr, "seed", strlen("seed")) == 0) {
      if (!isObjectString(value)) {
        return NULL;
      }      
      memcpy(credential->seed, simpleStringString(sexpStringString(value)), DIGEST_SIZE);

    } else if (memcmp(keystr, "tickets", strlen("tickets")) == 0) {
      if (!isObjectList(value)) {
        return NULL;
      }      
      
      // Count up the items
      
      sexpIter *ticketsIter = sexpListIter(value);

      int count = 1;

      while (ticketsIter) {
        count++;

        ticketsIter = sexpIterNext(ticketsIter);
      }

      credential->tickets = malloc(sizeof(ticket_t) * count);


      // Start again

      ticketsIter = sexpListIter(value);

      ticket_t *last = &credential->tickets[0];
      
      count = 1;
      
      while (ticketsIter) {
        sexpObject *currentTicket = sexpIterObject(ticketsIter);

        ticket_t *tkt = sexp_to_ticket(currentTicket);

        memcpy(&credential->tickets[count], tkt, sizeof(ticket_t));

        last->next = &credential->tickets[count];        
        last = last->next;

        ticketsIter = sexpIterNext(ticketsIter);
        
        count++;
      }
      
    } else {
      return NULL;
    } 

  SEXP_TO_OBJ_ATTR_PAIR_END(credential)
    
SEXP_TO_OBJ_FN_END(credential)


OBJ_TO_STR_FN(credential)

STR_TO_OBJ_FN(credential)

/*******************************************************/

OBJ_TO_SEXP_FN_START(blacklist)

  SEXP_LIST_ADDPAIR(blacklist_outer_list, server_id, blacklist->server_id, DIGEST_SIZE);

  char *link_window_buf = malloc(11); sprintf(link_window_buf, "%d", blacklist->link_window);
  SEXP_LIST_ADDPAIR(blacklist_outer_list, link_window, (u_char *)link_window_buf, strlen(link_window_buf));
  free(link_window_buf);
  
  sexpList *cert_list = newSexpList();
  sexpSimpleString *cert_ss = newSimpleString();
  free(cert_ss->string);
  cert_ss->length = cert_ss->allocatedLength = strlen("cert");
  cert_ss->string = (u_char *)strdup("cert");
  sexpString *cert_s = newSexpString();
  setSexpStringString(cert_s, cert_ss);
  sexpAddSexpListObject(cert_list, cert_s);
  
  sexpAddSexpListObject(cert_list, blacklist_cert_to_sexp(&blacklist->cert));

  sexpAddSexpListObject(blacklist_outer_list, cert_list);

  sexpList *nymbles_list = newSexpList();
  sexpSimpleString *nymbles_ss = newSimpleString();
  free(nymbles_ss->string);
  nymbles_ss->length = nymbles_ss->allocatedLength = strlen("nymbles");
  nymbles_ss->string = (u_char *)strdup("nymbles");
  sexpString *nymbles_s = newSexpString();
  setSexpStringString(nymbles_s, nymbles_ss);
  sexpAddSexpListObject(nymbles_list, nymbles_s);


  sexpList *nymbles = newSexpList();
  
  nymblelist_t *nymble = blacklist->nymbles;
  
  while (nymble) {
    sexpString *nymble_value_s = newSexpString(); 
    sexpSimpleString *nymble_value_ss = newSimpleString(); 
    free(nymble_value_ss->string); 
    nymble_value_ss->length = nymble_value_ss->allocatedLength = DIGEST_SIZE; 
    nymble_value_ss->string = (u_char *)malloc(DIGEST_SIZE); 
    memcpy(nymble_value_ss->string, nymble->nymble, DIGEST_SIZE); 
    setSexpStringString(nymble_value_s, nymble_value_ss); 
    sexpAddSexpListObject(nymbles, nymble_value_s);
    
    nymble = nymble->next;
  }
  
  sexpAddSexpListObject(nymbles_list, nymbles);
  
  sexpAddSexpListObject(blacklist_outer_list, nymbles_list);
  
OBJ_TO_SEXP_FN_END(blacklist)


SEXP_TO_OBJ_FN_START(blacklist)
  
  SEXP_TO_OBJ_ATTR_PAIR_START(blacklist)

    if (memcmp(keystr, "server_id", strlen("server_id")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"server_id\" field in blacklist\n");
        return NULL;
      }      
      memcpy(blacklist->server_id, simpleStringString(sexpStringString(value)), DIGEST_SIZE);
      
    } else if (memcmp(keystr, "link_window", strlen("link_window")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"link_window\" field in blacklist\n");
        return NULL;
      }
      sscanf((char *)simpleStringString(sexpStringString(value)), "%ud", &blacklist->link_window);
      
    } else if (memcmp(keystr, "cert", strlen("cert")) == 0) {
      blacklist_cert_t *cert = sexp_to_blacklist_cert(value);
      memcpy(&blacklist->cert, cert, sizeof(blacklist_cert_t));
      free(cert);
      
    } else if (memcmp(keystr, "nymbles", strlen("nymbles")) == 0) {
      // The value of this one should definitely be a list
      if (!isObjectList(value)) {
        fprintf(stderr, "expecting list value for \"nymbles\" field in blacklist\n");
        return NULL;
      }

      sexpIter *valueIter = sexpListIter((sexpList *)value);

      nymblelist_t **last = &blacklist->nymbles;

      while (valueIter) {
        sexpObject *nymble = sexpIterObject(valueIter);
        
        if (!nymble) {
          break;
        }

        if (!isObjectString(nymble)) {
          fprintf(stderr, "expecting string value for nymble in blacklist\n");
          return NULL;
        }

        *last = (nymblelist_t *)malloc(sizeof(nymblelist_t));

        memcpy((*last)->nymble, simpleStringString(sexpStringString(nymble)), DIGEST_SIZE);
        (*last)->next = NULL;

        last = &(*last)->next;

        valueIter = sexpIterNext(valueIter);
      }
      
    } else {
      fprintf(stderr, "unexpected \"%s\" field in blacklist\n", keystr);
      return NULL;
    }

  SEXP_TO_OBJ_ATTR_PAIR_END(blacklist)

SEXP_TO_OBJ_FN_END(blacklist)


OBJ_TO_STR_FN(blacklist)

STR_TO_OBJ_FN(blacklist)

/*******************************************************/

OBJ_TO_SEXP_FN_START(blacklist_cert)

  SEXP_LIST_ADDPAIR(blacklist_cert_outer_list, bl_hash, blacklist_cert->bl_hash, DIGEST_SIZE);

  char *time_period_buf = malloc(11); sprintf(time_period_buf, "%d", blacklist_cert->time_period);
  SEXP_LIST_ADDPAIR(blacklist_cert_outer_list, time_period, (u_char *)time_period_buf, strlen(time_period_buf));
  free(time_period_buf);

  SEXP_LIST_ADDPAIR(blacklist_cert_outer_list, bmac_n, blacklist_cert->bmac_n, DIGEST_SIZE);
  SEXP_LIST_ADDPAIR(blacklist_cert_outer_list, sig, blacklist_cert->sig, SIGNATURE_SIZE);

OBJ_TO_SEXP_FN_END(blacklist_cert)


SEXP_TO_OBJ_FN_START(blacklist_cert)

  SEXP_TO_OBJ_ATTR_PAIR_START(blacklist_cert)

    // All our values are strings (i.e., not lists)
    if (!isObjectString(value)) {
      fprintf(stderr, "expecting string value for \"%s\" field in blacklist_cert\n", keystr);
      return NULL;
    }
    u_char *valueptr = simpleStringString(sexpStringString(value));
  
    if (memcmp(keystr, "bl_hash", strlen("bl_hash")) == 0) {
      memcpy(blacklist_cert->bl_hash, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "time_period", strlen("time_period")) == 0) {
      sscanf((char *)valueptr, "%ud", &blacklist_cert->time_period);
    } else if (memcmp(keystr, "bmac_n", strlen("bmac_n")) == 0) {
      memcpy(blacklist_cert->bmac_n, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "sig", strlen("sig")) == 0) {
      memcpy(blacklist_cert->sig, valueptr, SIGNATURE_SIZE);
    } else {
      fprintf(stderr, "unexpected \"%s\" field in blacklist_cert\n", keystr);
      return NULL;
    }
    
  SEXP_TO_OBJ_ATTR_PAIR_END(blacklist_cert)

SEXP_TO_OBJ_FN_END(blacklist_cert)


OBJ_TO_STR_FN(blacklist_cert)

STR_TO_OBJ_FN(blacklist_cert)

/*******************************************************/

OBJ_TO_SEXP_FN_START(linking_token)
  
  SEXP_LIST_ADDPAIR(linking_token_outer_list, server_id, linking_token->server_id, DIGEST_SIZE);

  char *buf = malloc(11);
  sprintf(buf, "%d", linking_token->link_window);
  SEXP_LIST_ADDPAIR(linking_token_outer_list, link_window, (u_char *)buf, strlen(buf));

  sprintf(buf, "%d", linking_token->time_period);
  SEXP_LIST_ADDPAIR(linking_token_outer_list, time_period, (u_char *)buf, strlen(buf));
  free(buf);

  SEXP_LIST_ADDPAIR(linking_token_outer_list, trapdoor, linking_token->trapdoor, DIGEST_SIZE);
  
  sexpList *nymble_ticket_list = newSexpList();
  sexpSimpleString *nymble_ticket_ss = newSimpleString();
  free(nymble_ticket_ss->string);
  nymble_ticket_ss->length = nymble_ticket_ss->allocatedLength = strlen("nymble_ticket");
  nymble_ticket_ss->string = (u_char *)strdup("nymble_ticket");
  sexpString *nymble_ticket_s = newSexpString();
  setSexpStringString(nymble_ticket_s, nymble_ticket_ss);
  sexpAddSexpListObject(nymble_ticket_list, nymble_ticket_s);
  
  sexpAddSexpListObject(nymble_ticket_list, ticket_to_sexp(linking_token->nymble_ticket));

  sexpAddSexpListObject(linking_token_outer_list, nymble_ticket_list);
  
OBJ_TO_SEXP_FN_END(linking_token)


SEXP_TO_OBJ_FN_START(linking_token)

  SEXP_TO_OBJ_ATTR_PAIR_START(linking_token)

    u_char *valueptr = simpleStringString(sexpStringString(value));

    if (memcmp(keystr, "server_id", strlen("server_id")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"server_id\" field in linking_token\n");
        return NULL;
      }
      
      memcpy(linking_token->trapdoor, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "link_window", strlen("link_window")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"link_window\" field in linking_token\n");
        return NULL;
      }
      
      sscanf((char *)valueptr, "%ud", &linking_token->link_window);
    } else if (memcmp(keystr, "time_period", strlen("time_period")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"time_period\" field in linking_token\n");
        return NULL;
      }
      
      sscanf((char *)valueptr, "%ud", &linking_token->time_period);
    } else if (memcmp(keystr, "trapdoor", strlen("trapdoor")) == 0) {
      if (!isObjectString(value)) {
        fprintf(stderr, "expecting string value for \"trapdoor\" field in linking_token\n");
        return NULL;
      }
      
      memcpy(linking_token->trapdoor, valueptr, DIGEST_SIZE);
    } else if (memcmp(keystr, "nymble_ticket", strlen("nymble_ticket")) == 0) {
      if (!isObjectList(value)) {
        fprintf(stderr, "expecting list value for \"nymble_ticket\" field in linking_token\n");
        return NULL;
      }
      
      linking_token->nymble_ticket = sexp_to_ticket(value);
    } else {
      fprintf(stderr, "unexpected \"%s\" field in linking_token\n", keystr);
      return NULL;
    }
  
  SEXP_TO_OBJ_ATTR_PAIR_END(blacklist_cert)

SEXP_TO_OBJ_FN_END(linking_token)


OBJ_TO_STR_FN(linking_token)

STR_TO_OBJ_FN(linking_token)
