Nymble 1.0 System Specification
===============================

*** DRAFT ***

May 2, 2008

  * Cory Cornelius
  * Peter C. Johnson
  * Apu Kapadia
  * Daniel Peebles
  * Patrick P. Tsang

System overview
---------------

The Nymble system enables service providers (e.g., websites) to offer anonymous access to their services, while allowing them to blacklist anonymous users (i.e., block future accesses by that user) who are deemed to have misbehaved.  This revocation affects only future accesses, and a blacklisted user's previous activities remain anonymous and unlinkable; most importantly, blacklisted users remain anonymous.  Furthermore, a user may determine whether s/he is on the blacklist prior to committing to an action, thereby protecting the privacy of blacklisted users.  All blacklists expire at the end of the system-wide, administrator-defined "linkability window"; within a particular "time period"---a subdivision of the linkability window---a user's activities are linkable, and therefore time periods limit the rate of anonymous connections by users.

For further details regarding security, refer to the paper:

    Peter C. Johnson, Apu Kapadia, Patrick P. Tsang, and Sean W. Smith, "Nymble: Anonymous IP-Address Blocking," In Proceedings of the Seventh International Symposium on Privacy Enhancing Technologies (PET '07), pp. 113–-133, Ottawa, Canada, June 20-–22, 2007. Springer-Verlag, LNCS 4776.

### System configuration parameters

#### T, the duration of a time period (in minutes)

Default: 5

##### Implications of varying T

Since nymbles are associated with time periods, resulting in pseudonymous access within a time period, it is desirable to keep the duration `T` small. On the other hand, larger values of `T` can be used to limit the rate of anonymous connections by a user, thereby limiting the amount of damage an anonymous user can cause before being blacklisted.

#### W, the duration of a linkability window (in minutes)

Default: 1440 (i.e. 1 day).

##### Implications of varying W

Blacklists are cleared at the end of each linkability window. Since users remain blacklisted for the remainder of the linkability window after a complaint, it is desirable to keep the duration of the linkability window `L` long enough to curtail the malicious user’s activities, but not so long as to punish that user (or honest users to whom the IP address may get reassigned) indefinitely.

#### L, the number of time periods in one linkability window

Default: 288 i.e. `L=W/T`. 

We assume that `T` divides `W` evenly, i.e., `W` is a multiple of `T`. 

Since nymbles are used to authenticate users to perform actions such as editing a page or posting a comment, we suggest `T=5` min and `L=1` day as a trade-off. Further experimentation is needed to determine reasonable values for these parameters.


### Components

There are four actors in the Nymble system: a "user" wishing to access a service provided by a "server", and the administratively-separate "Pseudonym Manager" (PM) and "Nymble Manager" (NM) which provide the anonymity.

When a user wishes to access a service, that user must present a valid "nymble ticket" along with the access request.  This ticket is part of a "credential", which the user acquires from the Nymble Manager.  To acquire a credential from the Nymble Manager, the user must first acquire a "pseudonym" from the Pseudonym Manager; pseudonyms are valid for a single linkability window.  Each credential is tied to a specific pseudonym-server pair, and contains one ticket for each time period in the current linkability window.

When a server wishes to complain about a user, that server must present to the Nymble Manager the ticket associated with the offending access.  The Nymble Manager responds with a "linking token" which the server uses to blacklist that user.

Before requesting a service, a user downloads the server's blacklist, verifies that s/he is not blacklisted, and then presents a nymble ticket for authenticating to the service.

A note on security: The PM only assigns users pseudonyms, and does not know the identities of servers that the user connects to. The user's connections are pseudonymous to the NM (at the granularity of time periods). The PM and NM must, therefore, be administratively separate. A cascade of PMs may be used (in a future version) so that as long as one PM is honest, a user's connections cannot be deanonymized.

### Time and Synchronization

We assume a trusted time server. All actors are time synchronized. 

Time in our system is referenced to the system's up time:

    time_0           : the instant of time when the system starts

which is known to all entities

> FIXME : how?
>> ack: I propose that the NM hold the values `time_0`. Actors can obtain time from the time server, along with L and T, and compute the relevant window and period.

Hence, by sync'ing with the time server, all actors can infer:

    cur_time_period  : current time period
    prev_time_period : previous time period
    cur_link_window  : current linkability window

By convention, the first linkability window is indexed by 1 and the first time period in a linkability window is indexed by 1.

> FIXME : need more work on this section

### Communication Channels

Depending on the security requirements of the necessary communication channels between various actors, we have:

- a mutually-authenticated, confidential channel
- a one-side-authenticated, confidential channel
- a one-side-authenticated, other-side-anonymous, confidential channel

To realize the mutual and one-sided authenticity and confidentiality, we assume the NM, PM and all servers have certified private/public-key pair. In (3), the "other side" uses Tor to achieve anonymity.


Preliminaries
-------------

### Notations

Strings enclosed within double quotes ("") are literals.

`NBO` stands for Network Byte Order.

`INT` stands for a 32-bit integer in `NBO`.

`TRUE` is an `INT` with value 1

`FALSE` is an `INT` with value 0

`BOOL` is `TRUE` or `FALSE`

Concatenation is denoted by ||.

`HASH(s)` denotes the cryptographic hash of binary data `s`.

`KEYED_HASH(s, k)` denotes the keyed hash of binary data `s` using key `k`.

`HMAC(s, k)` denotes the Hash MAC (HMAC) of binary data `s` using key `k`.

`ENC(s, k)` denotes the symmetric encryption of binary data `s` using key `k`.

`DEC(s, k)` denotes the symmetric decryption of binary data `s` using key `k`.

`SIGN(s, sk)` denotes the digital signature of binary data `s` using private signing key `sk`.

`VERIFY(s, sig, pk)` denotes the verification of signature `sig` over binary data `s` using public verification key `pk`.

> FIXME : explain the [] notation

> FIXME : explain the underscore notation

### Cryptographic primitives

For cryptographic hash (`HASH`), we use SHA-256 (FIPS-180-2), which produces a 256-bit (32-octet) hash value.

For HMAC (`HMAC`) and keyed-hash (`KEYED_HASH`), we use HMAC-SHA-256 (FIPS-198) with a 256-bit (32-octet) key length, producing a 256-bit (32-octet) MAC.

For symmetric encryption (`ENC` and `DEC`), we use AES under CBC Mode (AES-CBC) with 128-bit (16-octet) key-size (FIPS-197). We use 128-bit nonces for IVs.

For digital signatures (`SIGN` and `VERIFY`), we use RSASSA-PSS With 2048-bit key-size (PKCS#1 v2.1, RFC 3447, Section 8.1) and a fixed public exponent of 65537.

### Protocol Messages

FIXME

Data Structures
---------------

### Nymble Tickets and Credentials

#### Nymble Tickets

A nymble is a pseudorandom number that serves as a pseudonym for a particular time period. nymbles (presented by a user) across periods are unlinkable unless a server has blacklisted that user. Nymbles are presented as part of a nymble ticket, as described next.

A nymble ticket is an authenticator that a user presents to a a server to gain access to some service. It contains a nymble specific to a server (`server_id`), time period (`time_period`), and linkability window (`link_window`). `trapdoorenc` is encrypted data that the NM can use during a complaint involving the nymble ticket. `mac_n` and `mac_ns` are `HMAC`s used to verify the integrity of the nymble ticket.

A nymble ticket has the following format:

    TICKET = server_id || time_period || link_window || nymble || trapdoorenc || mac_n || mac_ns

> dgp: we should consider moving server_id to credential to avoid unnecessary duplication of implicit information
>> ack: leave such optimizations for later

where:

    server_id     : a HASH() output
    time_period   : an INT
    link_window   : an INT
    nymble        : a HASH() output
    trapdoorenc   : an ENC() output
    mac_n, mac_ns : an HMAC() output

#### Credentials

A credential contains all the nymble tickets for a particular linkability window that a user can present to a particular server. As detailed later, the seed is a function of the user's id, the server's id, and the time period and linkability window for which the nymbles are valid. A collection of nymble tickets, therefore, are specific to user-server pairs, and cannot be transferred to other anonymous users, or be used at a different server.

A credential has the following format:

    CREDENTIAL = seed || ticket[1] || ticket[2] || ... || ticket[cred_size]

where:

    seed     : a HMAC() output
    ticket[] : a NYMBLE_TICKET

### Blacklists and their metadata

#### Blacklist Metadata and Certs

`bmac_n` is used by the NM to quickly verify the integrity of the blacklist cert. `sig` is used by users to verify the integrity and freshness of the blacklist. The blacklist freshness is guaranteed up to the end of `time_period`.

A Blacklist metadata has the following format:

    BL_METADATA = server_id || link_window 

where:

    server_id   : a HASH() output
    link_window : an INT

A cert has the following format:

    CERT = bl_hash || time_period || bmac_n || sig

where:

    bl_hash     : a HASH() output
    time_period : an INT
    bmac_n      : an HMAC() output
    sig         : a SIG() output

#### Blacklists


A server's blacklist is a list of blacklist entries, corresponding to all the nymbles that the server has complained about.

> FIXME: need to complete the remainder of this section

A blacklist has the following format:

    BLACKLIST = metadata || nymble[1] || nymble[2] || ... || nymble[bl_size] || cert

where:

    metadata : a BL_METADATA
    cert     : a CERT
    nymble[] : a HASH() output


### Linking Tokens and Linking Lists

#### Linking Tokens

A linking token is issued by the Nymble Manager for each nymble ticket that has been complained about. The linking token includes a trapdoor, which can be evolved every subsequent time period, and then be used to compute the blacklisted nymble for that time period.

A linking token has the following format:

    LINKING_TOKEN = server_id || link_window || time_period || trapdoor || nymble_ticket

where:

    server_id       : a HASH() output
    link_window     : an INT
    time_period     : an INT
    trapdoor        : an ENC() output
    nymble_ticket   : a TICKET 

#### Linking-list entries

Each entry in a linking list corresponds to a linking token obtained from the nymble manager. The nymble for the blacklisted user is computed every time period using the trapdoor in the entry. `time_period` is the time period for which the nymble has been computed.

A linking-list entry has the following format:

    LL_ENTRY = time_period || trapdoor || nymble

where:

    time_period : an INT
    trapdoor    : a HASH() output
    nymble      : a HASH() output

#### Linking Lists

Each server maintains a linking list composed of linking-list entries. When a user presents a nymble ticket for authentication, the server checks the nymble within the ticket against the nymbles in the linking-list entries. A match indicates that the user has been blacklisted.

A linking list has the following format:

    LINKING_LIST = entry[1] || entry[2] || ... || entry[ll_size]

where:

    entry[] : an LL_ENTRY


State
-----

> FIXME : verbal descriptions to be added

All actors in the system keep an internal state.

### NM

NM maintains `NM_STATE`.


A `NM_STATE` contains:

    cur_link_window : an INT
    cur_time_period : an INT
    hmac_key_n      : a HMAC key
    hmac_key_np     : a HMAC key
    keyedhash_key_n : a keyed hash key
    encrypt_key_n   : a symmetric encryption key 
    sign_key_n      : a public key
    verify_key_n    : the private key associated with sign_key_n
    entries[]       : an array of NM_ENTRY

where an `NM_ENTRY` contains:

    server_id       : an INT
    bl_last_updated : an INT
    hmac_key_ns     : a HMAC key

`bl_last_updated` is the most recent time period in which the server's blacklist was updated via the `COMPLAIN` and `UPDATE` protocol. The NM uses this variable to check that it is updating the most recent version of the blacklist with additional blacklist entries. 

### PM

PM maintains `PM_STATE`.

A `PM_STATE` contains:

    cur_link_window  : an INT
    hmac_key_np      : an HMAC key
    keyedhash_key_p  : a keyed-hash key
    verify_key_n     : a digital signature public key

### Server

Each Server maintains `SERVER_STATE`.

A `SERVER_STATE` contains:

    cur_time_period : an INT
    cur_link_window : an INT
    server_id       : an INT
    hmac_key_ns     : a HMAC key
    blacklist       : a BLACKLIST
    linking_list    : a LINKING_LIST
    finalized       : a BOOL

> patrick : please check for consistency with the new blacklisting mechanisms

### User

Each user maintains `USER_STATE`.

A `USER_STATE` contains:

    cur_time_period : an INT
    cur_link_window : an INT
    pseudonym       : a KEYED_HASH output
    mac_np          : a HMAC output
    entries[]       : an array of USER_ENTRY

where a `USER_ENTRY` contains:

    server_id          : an INT
    link_window        : an INT
    credential         : a CREDENTIAL
    blacklist          : a BLACKLIST
    blacklisted        : an INT
    last_authenticated : an INT

> patrick : please check for consistency with the new blacklisting mechanisms

For simplicity, we will refer to fields in individual entries as `field[server_id]`, e.g., `blacklisted[id]` refers to the blacklisted field in an entry with `server_id = id`

> patrick : this happens in other places as well, might be good to put this in notations

> Q : is `link_window[server_id]` redundant?
>> ack: implementation might use it, but technically the credential has the linkability window information within it


Algorithms
----------

### Evolving a trapdoor

A trapdoor is evolved delta time periods by hashing the trapdoor delta times.

    EVOLVE_TRAPDOOR(trapdoor, delta)
      
      FOR i FROM 1 TO delta DO
        trapdoor = HASH("f" || trapdoor)
      END FOR
      
      RETURN trapdoor

### Computing a nymble

A nymble for a particular time period is computed by hashing the trapdoor for that time period.

    COMPUTE_NYMBLE(trapdoor)
      
      RETURN HASH("g" || trapdoor)

### Checking the integrity of a blacklist (NM's approach)

The NM verifies a blacklist's integrity by using HMAC `bmac_n`.

    NM_BL_CHECK_INTEGRITY(blacklist, sid, hmac_key_n)
      
      PARSE blacklist INTO metadata || nymble[1] || nymble[2] ||... || nymble[size] || cert
      PARSE metadata  INTO server_id || link_window 
      PARSE cert      INTO bl_hash || time_period || bmac_n || sig
      
      IF 
          bl_hash = HASH(metadata || nymble[1] || nymble[2] || ... || nymble[size], hmac_key_n)
        AND
          bmac_n = HMAC(bl_hash || time_period) 
        AND 
          server_id = id
      THEN
        RETURN TRUE 
      ELSE
        RETURN FALSE
      END IF

### Checking the integrity of a blacklist (Everyone else's approach)

Users can check a blacklist's integrity using the digital signature `sig` and the NM's public key `verify_key_n`.

    BL_CHECK_INTEGRITY(blacklist, sid, verify_key_n)
      
      PARSE blacklist INTO metadata || nymble[1] || nymble[2] || ... || nymble[size] || cert
      PARSE metadata  INTO server_id || link_window
      PARSE cert      INTO bl_hash || time_period || bmac_n || sig
      
      IF
          bl_hash = HASH(metadata || nymble[1] || nymble[2] || ... || nymble[size], hmac_key_n)
        AND
          VERIFY(bl_hash || time_period, sig, verify_key_n) = TRUE
        AND 
          server_id = sid
      THEN
        RETURN TRUE
      ELSE 
        RETURN FALSE
      END IF

### Checking the Freshness of a blacklist

A user must check the the received blacklist is the most current blacklist. This function checks if a blacklist is current for the specified time period (`period`) and linkability window (`window`).

    BL_CHECK_FRESHNESS(blacklist, period, window)
      
      PARSE blacklist INTO metadata || nymble[1] || nymble[2] || ... || nymble[size] || cert
      PARSE metadata  INTO server_id || link_window
      PARSE cert INTO bl_hash || time_period || bmac_n || sig
      
      IF
          time_period = period
        AND
          link_window = window
      THEN
        RETURN TRUE
      ELSE
        RETURN FALSE
      END IF

### Verifying a Nymble Ticket

A server verifies the integrity of a nymble ticket by checking the HMAC `mac_ns` using the key `hmac_key_ns`. The server also ensures that the server id, time period, and linkability window match the supplied parameters.

    VERIFY_TICKET(nymble_ticket, sid, period, window, hmac_key_ns)
      
      PARSE nymble_ticket INTO server_id || time_period || link_window || nymble || trapdoorenc || mac_n || mac_ns
      
      IF
          server_id   = sid
        AND
          time_period = period
        AND
          link_window = window
        AND
          mac_ns      = HMAC(server_id || time_period || link_window || nymble || trapdoorenc || mac_n, hmac_key_ns)
      THEN
        RETURN TRUE
      ELSE
        RETURN FALSE
      END IF

### Linking a Nymble Ticket

The server determines whether a user is blacklisted by checking if the user's nymble appears in its linking list.

    LINK_TICKET(nymble_ticket, linking_list)
      
      EXTRACT nymble FROM nymble_ticket
      
      IF
        linking_list CONTAINS nymble
      THEN
        RETURN TRUE
      ELSE
        RETURN FALSE
      END IF

### Checking if blacklisted

A user determines whether s/he is blacklisted by checking if one of his/her nymbles appears in the blacklist. 

    I_AM_BLACKLISTED(blacklist, credential)
      
      EXTRACT seed FROM credential
      EXTRACT nymble[1], nymble[2], ..., nymble[size] FROM blacklist
      
      flag    = FALSE
      nymble0 = COMPUTE_NYMBLE(seed)
      
      FOR i FROM 1 TO size DO
      
        IF
          nymble0 = nymble[i]
        THEN
          flag = TRUE
        END IF
        
      END FOR
      
      RETURN flag

### NM Checking if user is already blacklisted

The NM must not issue a trapdoor for the same user twice. For example, a server might complain about two nymble tickets that belong to the same user. Obtaining two or more trapdoors for the same user allows the server to link the trapdoors and violate the users unlinkability. Therefore, only one trapdoor must be issued to a server per user in a particular linkability window. The NM uses this algorithm to check whether the user has already been blacklisted. 

    USER_ALREADY_BLACKLISTED(bl_nymbles, nymble)
      
      flag = FALSE
      
      FOR i FROM 1 TO size_of(bl_tickets) DO
      
        IF
          bl_nymbles[i] = nymble
        THEN
          flag = TRUE
        END IF
        
      END FOR
      
      RETURN flag

### Creating a credential

> FIXME : add blurb

    CREATE_CREDENTIAL(nm_state, pseudonym, server_id, cur_link_window)
      
      PARSE nm_state INTO ...
      
      trapdoor[0] = KEYED_HASH(pseudonym || server_id || cur_link_window, keyedhash_key_n)
      seed        = trapdoor[0]
      size        = L
      
      FOR i FROM 1 TO size DO
      
        trapdoor[i]    = EVOLVE_TRAPDOOR(trapdoor[i-1], 1)
        nymble[i]      = COMPUTE_NYMBLE(trapdoor[i])
        trapdoorenc[i] = ENC(trapdoor[i] || pseudonym, encrypt_key_n)
        content[i]     = server_id || i || cur_link_window || nymble[i] || trapdoorenc[i]
        mac_n[i]       = HMAC(content[i], hmac_key_n)
        mac_ns[i]      = HMAC(content[i] || mac_n[i], hmac_key_ns[server_id])
        ticket[i]      = content[i] || mac_n[i] || mac_ns[i]
        
      END FOR
      
      credential = seed || ticket[1] || ticket[2] || ... || ticket[size]
      
      RETURN credential

### Creating a blacklist

> FIXME : define XXX_KEYGEN()

    CREATE_BLACKLIST(nm_state, server_id, time_period, link_window)
      
      PARSE nm_state INTO ...
      
      hmac_key_ns = HMAC_KEYGEN()
      metadata    = server_id || link_window
      bl_hash     = HASH(metadata)
      bmac_n      = HMAC(bl_hash || time_period, hmac_key_n)
      sig         = SIGN(bl_hash || time_period, sign_key_n)
      cert        = bl_hash || time_period || bmac_n || sig
      nymbles     = NULL (i.e. empty list)
      blacklist   = metadata || nymbles || cert
      
      RETURN blacklist

### Updating a blacklist

> FIXME : size_of() returns the number of entires in an array, and needs to be defined more formally somewhere.

    UPDATE_BLACKLIST(nm_state, blacklist, complaint_tickets, time_period, link_window)
      
      PARSE nm_state INTO ...
      
      EXTRACT nymbles FROM blacklist
      
      new_bl_size = size_of(nymbles) + size_of(complaint_tickets)
      
      FOR i FROM 1 to size_of(complaint_tickets) DO
      
        EXTRACT trapdoorenc FROM ticket
        EXTRACT pseudonym FROM DEC(trapdoorenc, encrypt_key_n)
        
        seed    = KEYED_HASH(pseudonym || server_id || cur_link_window, keyedhash_key_n)
        nymble0 = COMPUTE_NYMBLE(seed)
        
        EXTRACT blacklisted_nymbles FROM blacklist
        
        IF
          USER_ALREADY_BLACKLISTED(blacklisted_nymbles, nymble0)
        THEN
          complaint_nymbles[i] = rand()
        ELSE
          
          EXTRACT trapdoorenc FROM complaint_tickets[i]
          EXTRACT trapdoor, pseudonym FROM DEC(trapdoorenc, encrypt_key_n)
          
          seed                 = KEYED_HASH(pseudonym || server_id || link_window, keyedhash_key_n)
          complaint_nymbles[i] = COMPUTE_NYMBLE(seed)
          
        END
        
      END FOR
      
      new_metadata  = server_id || link_window || new_bl_size 
      new_nymbles   = nymbles || complaint_nymbles
      new_bl_hash   = HASH(new_metadata || new_nymbles)
      new_bmac_n    = HMAC(new_bl_hash || time_period, hmac_key_n)
      new_sig       = SIGN(new_bl_hash || time_period, sign_key_n)
      new_cert      = new_bl_hash || time_period || new_bmac_n || new_sig
      new_blacklist = new_metadata || new_nymbles || new_cert
      
      RETURN new_blacklist

### Computing linking tokens

    COMPUTE_TOKENS(nm_state, server_id, blacklisted_nymbles, complaint_tickets, time_period, link_window)
    
      PARSE nm_state INTO ...
      
      size             = sizeof(complaint_tickets)
      next_time_period = time_period + 1
      
      FOR i FROM 1 TO size DO
      
        EXTRACT trapdoorenc FROM complaint_tickets[i]
        EXTRACT pseudonym FROM DEC(trapdoorenc, encrypt_key_n)
        
        seed    = KEYED_HASH(pseudonym || server_id || cur_link_window, keyedhash_key_n)
        nymble0 = COMPUTE_NYMBLE(seed)
        
        IF 
          USER_ALREADY_BLACKLISTED(blacklisted_nymbles, nymble0)
        THEN 
          trapdoor_for_next_time_period[i] = RAND()
        ELSE
        
          EXTRACT time_period FROM complaint_tickets[i] AS complaint_time_period
          EXTRACT trapdoorenc FROM complaint_tickets[i]
          EXTRACT trapdoor, pseudonym FROM DEC(trapdoorenc, encrypt_key_n)
          
          delta                            = next_time_period - complaint_time_period
          trapdoor_for_next_time_period[i] = EVOLVE_TRAPDOOR(trapdoor, delta)
          blacklisted_tickets              = blacklisted_tickets || complaint_tickets[i]
          
        END IF
        
        linking_tokens[i] = server_id || link_window || next_time_period || trapdoor_for_next_time_period[i] || complaint_tickets[i]
        
        where:
            linking_tokens[i] : a LINKING_TOKEN
            
      END FOR
      
      RETURN linking_tokens

Protocols
---------

Note that because DENY payloads are `NULL`, they are not explicitly defined within each message section.

> Big question: how do parties authenticate each other?

> FIXME: add return code for each algorithm in each protocol

> patrick : describe at a high level what each of the protocols does

### SYSTEM_SETUP

This is a protocol executed between NM and PM over a confidential, mutually-authenticated channel upon initial system setup.  This happens once during the life of the system. The NM generates key material and establishes a shared key with the PM. 

  1.  NM generates all the keys using the respective key generation algorithm in `NM_STATE` and set `NM_SERVER_LIST` in `NM_STATE` to an empty list.
        
  2.  NM -> PM: `REQUEST`

      The payload of `REQUEST` is:

        hmac_key_np || verify_key_n
          
  3.  PM generates keyed hash key `keyedhash_key_p` in `PM_STATE` and sets `hmac_key_np` and `verify_key_n` in `PM_STATE` to those in `REQUEST`
      
  4.  PM -> NM: `GRANT`
    
      The payload of `GRANT` is:
      
        NULL
        
      NM and PM terminate as success.

### SERVER_REGISTRATION

This is a protocol executed between a Server and the NM over a confidential, mutually-authenticated link.  This protocol must occur once for each server before that server may accept nymble tokens. In this protocol, the server sets up a shared key with the NM, and the NM initializes the server's blacklist.


> CTC: How does the server and NM establish this confidential, mutually-authenticated link? If we assume SSL, how are the key's exchanged?
>> patrick : we assume X509 PKI, the details of which should probably be documented elsewhere.
> ack: does it set finalized = true for the current time period?

  1.  Server -> NM: `REQUEST`
  
      The payload of `REQUEST` is:
    
        NULL
      
      NM extracts the hostname of the server [ from the server's cert ? ]
    
      NM hashes the hostname of the server to get `server_id`.
    
      If `server_i`d  appears in `NM_SERVER_LIST` in `NM_STATE`, go to (2). Otherwise, go to (3).
    
  2.  NM -> Server: `DENY`
  
      Both NM and the Server then terminate the protocol as failure.
    
  3.  NM does the following:
  
        blacklist       = CREATE_BLACKLIST(nm_state, server_id, cur_time_period, cur_link_window)
        bl_last_updated = 0
        nm_entry        = server_id || bl_last_updated || hmac_key_ns
        NM_STATE        = NM_STATE || nm_entry
      
  4.  NM -> Server: GRANT
  
      The payload of the GRANT reply is a

        hmac_key_ns || blacklist

      NM terminates as success
    
  5.  Server sets `hmac_key_ns` and blacklist `SERVER_STATE` to those in the payload of `GRANT`, and initializes other state to the following:
      
        linking_list[server_id] = NULL (i.e. empty list)
        finalized[server_id]    = false
      
      Server terminates as success.

### USER_REGISTER

This is a protocol executed between a User and the PM over a PM-authenticated, confidential channel.  A user must register once each linkability window, before acquiring a credential, and before requesting a service.

  1.  User -> PM: `REQUEST`
  
      The payload of `REQUEST` is:
    
        NULL
      
      PM infers the registering user's IP address `user_ip` from the channel. 
    
      PM checks if the IP is allowed to register, according to some policies, such as: it's a new IP address, it's not a Tor exit node address, etc. If the check fails, goto (2). Otherwise go to (3).
      
  2.  PM -> User: `DENY`
  
      PM terminates as failure. Upon receiving `DENY`, User also terminates as failure.
      
  3.  PM computes:
  
        pseudonym   = KEYED_HASH(user_id || cur_link_window, keyedhash_key_p)
        mac_np      = HMAC(pseudonym || cur_link_window, hmac_key_np)
        
  4.  PM -> User: `GRANT`
  
      The payload of the `GRANT` reply is:
      
        pseudonym || mac_np
        
      User stores these in `USER_STATE`.
      
      They both terminate as success.

### CREDENTIAL_ACQUISITION

This is a protocol executed between a User and the NM over a NM-authenticated, confidential, User-anonymous channel.  This protocol must occur once in every linkability window during which a user wants to access a specific server that requires Nymble-based authentication.

User wants to get a credential to connect to server with id `server_id`.

> CTC: What does the user do with this credential?

  1.  User -> NM: `REQUEST`
  
      The payload of the request is:
      
        pseudonym || mac_np || server_id
        
  2.  NM uses `hmac_key_np` in `NM_STATE` to verify the request by checking that:
      
        mac_np = HMAC(pseudonym || cur_link_window, hmac_key_np)
      
      If `server_id` is invalid, or the verification fails, go to (3), otherwise go to (4).  
      
  3.  NM -> User: `DENY`
  
      They both terminate as failure
      
  4.  NM creates and computes a credential as follows:
  
        credential = CREATE_CREDENTIAL(nm_state, pseudonym, server_id, cur_link_window)
      
  5.  NM -> User: `GRANT`
  
      The payload of the `GRANT` reply is:
      
        credential
        
      They terminate as success.

### GET_BLACKLIST

This is a protocol executed between a registered User and a registered Server over an User-anonymous channel. This protocol must occur once during each time period (`access_time_period`) in which a user submits a `GET_SERVICE` request to a particular server, before the request is submitted.

> Q: should the server be authenticated in the channel? what are the security and efficiency impact?
> ack: Patrick, make more concrete after you've fleshed out the time synchronization protocol

  1.  User -> Server: `REQUEST`
  
      The payload of the request is:
      
        NULL
        
  2.  Server -> User: `GRANT`
  
      If finalized in `SERVER_STATE` is false, i.e. if the server does not have a finalized blacklist for the `cur_time_period`, then Server first executes `COMPLAIN and UPDATE` and obtains a finalized blacklist from the NM.
      
      The payload of a `GRANT` reply is: 
      
        blacklist in NM_STATE
        
  3.  Now, an entry for `server_id` must already exist because a user gets a credential for a server before he/she gets a blacklist of the server.
      
  4.  User verifies the integrity and freshness of the blacklist.
  
        ASSERT TRUE = BL_CHECK_INTEGRITY(blacklist, server_id, verify_key_n)
        ASSERT TRUE = BL_CHECK_FRESHNESS(blacklist, server_id, cur_time_period, cur_link_window)
        
      If any of the assertions fail, User removes the blacklist from the entry. He terminates as failure.
      
      Else, the user replaces `blacklist[server_id]` with this new blacklist. He terminates as success.

### AUTHENTICATE

This is a protocol executed between a registered User and a registered Server with identity `server_id` over a Server-authenticated, confidential, User-anonymous channel during time period `access_time_period = cur_time_period`.

  1.  User extracts from `USER_STATE` the entry for `server_id`, and `ticket[cur_time_period]` from the `credential` in that entry.
      
  2.  User -> Server: `REQUEST`
  
      The payload of the `REQUEST` is:
      
        the ticket extracted above
        
  3.  Server does the following:
  
      1. Extract `hmac_key_ns`, `server_id`, `linking_list` from its `SERVER_STATE`
      
      2. Asserts:
      
          1 = VERIFY_TICKET(ticket, server_id, cur_time_period, cur_link_window,hmac_key_ns)
          0 = LINK_TICKET(ticket, linking_list)
          
      If any of the assertions fail, go to (4), otherwise go to (5).
      
  4.  Server -> User : `DENY`
  
      Both terminate as failure
      
  5.  Server -> User : `GRANT`
  
      The payload of `GRANT` is:
      
        NULL
        
      Both terminate as success.

### REQUEST FOR SERVICE

This section describes the procedure a user follows to request a service.

> patrick : we want to allow for the option for still connecting even when the user has become linkable, but we prefer not to ask for user input within this protocol. One way of doing it would be to surface an API call for the application to query the current linkability status of the user, and not check if the user is linkable in SERVICE_REQUEST.
> CTC: Where does blacklisted[server_id] ever get set?

The User does the following:

  1.  Record the time of access:
  
        access_time_period = cur_time_period
        
  2.  Self-censoring:
  
      1. If `cur_time_period = last_authenticated` in the entry for `server_id` in `USER_STATE`, then terminate as `FAILURE`.
         
      2. If `blacklisted[server_id]` in `USER_STATE` is `TRUE`, then terminate as `FAILURE`.
         
  3.  Getting Blacklist:
  
      Invoke `GET_BLACKLIST(server_id, access_time_period)`. If it returns `FAILURE`, the User terminates as `FAILURE`.
      
  4.  Blacklist Inspection:
  
      If `I_AM_BLACKLISTED(blacklist, credential[server_id])`, then terminate as `FAILURE`.
         
  5.  Ready to be authenticated
  
      If the blacklist for `access_time_period` has been verified for integrity and freshness, and the user is not on the blacklist, the user authenticates with the server.
      
      User invokes `AUTHENTICATE(server_id, access_time_period)`.

### COMPLAIN and UPDATE

This protocol is executed between a registered Server and the NM over a mutually-authenticated, confidential channel. 

  1.  By authenticating the server, the NM knows the server_id of the server.
  
  2.  Server -> NM: `REQUEST`
  
      The payload of `REQUEST` is:
      
        blacklist in SERVER_STATE and a complaint
        
      where:
      
        complaint = ticket[1] || ticket[2] || ... || ticket[complaint_size]
        
      where:
      
        ticket[i] is the ticket associated with the offending access and the blacklist is from SERVER_STATE.
        
      complaint could be `NULL`.
      
  3.  NM does the following:
  
        ASSERT 1 = NM_BL_CHECK_INTEGRITY(blacklist, server_id, hmac_key_n)
        ASSERT 1 = BL_CHECK_FRESHNESS(blacklist, bl_last_updated[server_id], cur_link_window)
        ASSERT NOT bl_last_updated[server_id] = cur_time_period
      
      If any of the assertions fail, go to (4), otherwise, go to (5).
      
  4.  NM -> Server: `DENY`
  
      Both terminate as failure.
      
  5.  NM does the following:
  
      EXTRACT `tickets` FROM `blacklist`
      
        linking_tokens = COMPUTE_TOKENS(nm_state, tickets, complaint, cur_time_period)
        new_blacklist = UPDATE_BLACKLIST(nm_state, blacklist, complaint, cur_time_period, cur_link_window)
        bl_last_updated[server_id] = cur_time_period
        
  6.  NM -> Server: `GRANT`:
  
      The payload of the `GRANT` reply is an blacklist and a list of tokens:
      
        complaint_reply = new_blacklist || linking_tokens
        
  7.  Upon receiving a `GRANT` reply, the server replaces blacklist in `SERVER_STATE` with `new_blacklist`, and appends each linking token to `linking_list` in `SERVER_STATE` as an `LL_ENTRY`:
      
      For each `linking_token[i]`, the server creates a linking list entry as `time_period[i] || trapdoor[i] || nymble[i]`. All these entries can be extracted from `linking_token[i]`.
  
      The SERVER sets `finalized[server_id] = true`.

State Update
------------
 
### Time period iteration

When transitioning from time period `j` to time period `j+1`, where `j<L`:

  * The NM must increment its `cur_time_period` by `1`.
  * Each server must update every entry in its linking list as follows:
    * Parse the entry into `time_period || trapdoor || nymble`
    * Set trapdoor to `EVOLVE_TRAPDOOR(trapdoor, 1)`
    * Set nymble to `COMPUTE_NYMBLE(trapdoor)`
    * Increment `cur_time_period` by `1`.
    * Set `finalized = FALSE`
  * Each user must increment its `cur_time_period` by `1`.

### Linkability window iteration

  * Each actor increments its `cur_link_window` by `1`.
  * The NM removes all entries in `NM_STATE`.
  * Each server invokes `SERVER_REGISTRATION` again for the new linkability window thereby refreshing its `SERVER_STATE`.
  * Each user removes all entries in `USER_STATE`.
