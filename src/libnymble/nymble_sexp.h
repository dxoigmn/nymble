#ifndef __NYMBLE_SEXP_H__
#define __NYMBLE_SEXP_H__

#include "nymble.h"
#include "sexp/sexp.h"

#define OBJ_TO_SEXP_FN_START(TYPE) \
sexpObject *TYPE ## _to_sexp(TYPE ## _t *TYPE) { \
  sexpList *TYPE ## _outer_list = newSexpList(); \
  \
  sexpSimpleString *TYPE ## _name = newSimpleString(); \
  TYPE ## _name->length = TYPE ## _name->allocatedLength = strlen(#TYPE); \
  TYPE ## _name->string = (u_char *)strdup(#TYPE); \
  \
  sexpString *TYPE ## _name_string = newSexpString(); \
  setSexpStringString(TYPE ## _name_string, TYPE ## _name); \
  \
  sexpAddSexpListObject(TYPE ## _outer_list, TYPE ## _name_string);
  
#define OBJ_TO_SEXP_FN_END(TYPE) \
  return (sexpObject *)TYPE ## _outer_list; \
}


#define SEXP_LIST_ADDPAIR(list, key, value, valuelen) \
  sexpList *key ## _list = newSexpList(); \
  \
  sexpSimpleString *key ## _ss = newSimpleString(); \
  free(key ## _ss->string); \
  key ## _ss->length = key ## _ss->allocatedLength = strlen(#key); \
  key ## _ss->string = (u_char *)strdup(#key); \
  sexpString *key ## _s = newSexpString(); \
  setSexpStringString(key ## _s, key ## _ss); \
  sexpAddSexpListObject(key ## _list, key ## _s); \
  \
  sexpString *key ## _value_s = newSexpString(); \
  sexpSimpleString *key ## _value_ss = newSimpleString(); \
  free(key ## _value_ss->string); \
  key ## _value_ss->length = key ## _value_ss->allocatedLength = valuelen; \
  key ## _value_ss->string = (u_char *)malloc(valuelen); \
  memcpy(key ## _value_ss->string, value, valuelen); \
  setSexpStringString(key ## _value_s, key ## _value_ss); \
  sexpAddSexpListObject(key ## _list, key ## _value_s); \
  sexpAddSexpListObject((list), key ## _list);
  
  

  // We want to put the character into a simple string, to avoid having to worry
  // about allocating a string of the right size

  // Hackish, but it's the only field I really have control over and no use for 
  // TODO: take an approach like the one I use for the inputstream

  // We don't want it to wrap
#define OBJ_TO_STR_FN(TYPE) \
sexpSimpleString *TYPE ## _to_str(TYPE ## _t *TYPE, int mode) { \
  sexpObject *object = TYPE ## _to_sexp(TYPE); \
  \
  sexpOutputStream *os = newSexpOutputStream(); \
  os->putChar = ssPutChar; \
  \
  os->outputFile = (void *)newSimpleString(); \
  \
  os->maxcolumn = -1; \
  \
  switch (mode) { \
    case CANONICAL: \
      canonicalPrintObject(os, object); \
    break; \
    case BASE64: \
      base64PrintWholeObject(os, object); \
    break; \
    case ADVANCED: \
      advancedPrintObject(os, object); \
  } \
  \
  return (sexpSimpleString *)os->outputFile; \
}

#define STR_TO_OBJ_FN(TYPE) \
TYPE ## _t *str_to_ ## TYPE(sexpSimpleString *str) { \
  sexpObject *object = NULL; \
  \
  if (!characterTablesInitialized) { \
    initializeCharacterTables(); \
  } \
  \
  sexpStringInputStream *sis = newSexpStringInputStream((char *)str->string, str->length + 1); \
  \
  object = scanObject(sis); \
  \
  return sexp_to_ ## TYPE (object); \
}

#define SEXP_TO_OBJ_FN_START(TYPE) \
TYPE ## _t *sexp_to_ ## TYPE(sexpObject *object) { \
  TYPE ## _t *TYPE = malloc(sizeof(TYPE ## _t)); \
  memset(TYPE, 0, sizeof(TYPE ## _t)); \
  \
  if (!isObjectList(object)) { \
    return NULL; \
  } \
  \
  sexpList *outer_list = (sexpList *)object; \
  sexpIter *outer_iter = sexpListIter(outer_list); \
  \
  if (!isObjectString(sexpIterObject(outer_iter))) { \
    return NULL; \
  } \
  \
  if (memcmp(#TYPE,  (char *)simpleStringString(sexpStringString(sexpIterObject(outer_iter))), strlen(#TYPE)) != 0) { \
    return NULL; \
  } \
  outer_iter = sexpIterNext(outer_iter);
  
#define SEXP_TO_OBJ_ATTR_PAIR_START(TYPE) \
  while (outer_iter) { \
    sexpObject *current = sexpIterObject(outer_iter); \
    \
    if (!isObjectList(current)) { \
      fprintf(stderr, "moo 17"); \
      return NULL; \
    } \
    \
    sexpList *currentPairList = (sexpList *)current; \
    \
    sexpIter *currentPairIter = sexpListIter(currentPairList); \
    \
    sexpObject *key = sexpIterObject(currentPairIter); \
    \
    if (!isObjectString(key)) { \
      fprintf(stderr, "returning NULL prematurely from " #TYPE); \
      return NULL; \
    } \
    \
    char *keystr = (char *)simpleStringString(sexpStringString(key)); \
    \
    currentPairIter = sexpIterNext(currentPairIter); \
    \
    sexpObject *value = sexpIterObject(currentPairIter); \
    
    
#define SEXP_TO_OBJ_ATTR_PAIR_END(TYPE) \
    outer_iter = sexpIterNext(outer_iter); \
  }

#define SEXP_TO_OBJ_FN_END(TYPE) \
  return TYPE; \
}

typedef struct sexp_stringinputstream {
  sexpInputStream stream;
  char *string;
  int pos;
  int len;
} sexpStringInputStream;

/* This shouldn't be public, and whatever uses it should be destroyed! */
void ssPutChar(sexpOutputStream *os, int c);

/* sexp Functions */
ticket_t *sexp_to_ticket(sexpObject *);
pseudonym_t *sexp_to_pseudonym(sexpObject *);
credential_t *sexp_to_credential(sexpObject *);
blacklist_t *sexp_to_blacklist(sexpObject *);
blacklist_cert_t *sexp_to_blacklist_cert(sexpObject *);
linking_token_t *sexp_to_linking_token(sexpObject *);

sexpObject *blacklist_cert_to_sexp(blacklist_cert_t *);
sexpObject *ticket_to_sexp(ticket_t *);

sexpSimpleString *ticket_to_str(ticket_t *, int);
sexpSimpleString *pseudonym_to_str(pseudonym_t *, int);
sexpSimpleString *credential_to_str(credential_t *, int);
sexpSimpleString *blacklist_to_str(blacklist_t *, int);
sexpSimpleString *blacklist_cert_to_str(blacklist_cert_t *, int);
sexpSimpleString *linking_token_to_str(linking_token_t *, int);

ticket_t *str_to_ticket(sexpSimpleString *);
pseudonym_t *str_to_pseudonym(sexpSimpleString *);
credential_t *str_to_credential(sexpSimpleString *);
blacklist_t *str_to_blacklist(sexpSimpleString *);
blacklist_cert_t *str_to_blacklist_cert(sexpSimpleString *);
linking_token_t *str_to_linking_token(sexpSimpleString *);

#endif