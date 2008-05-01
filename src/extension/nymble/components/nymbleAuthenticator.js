const COMPONENT_CONTRACTID = "@mozilla.org/network/http-authenticator;1?scheme=nymble"; 
const COMPONENT_CID = Components.ID("{68629EFA-E617-46F8-810F-9E31679E1FF2}"); 
const COMPONENT_PRETTY_NAME = "The Nymble Authentication Scheme"; 

var gNymbleAuthenticatorComponent = { 
  REQUEST_BASED: 1, 
  CONNECTION_BASED: 2, 
  REUSABLE_CREDENTIALS: 4, 
  REUSABLE_CHALLENGE: 8, 
  IDENTITY_IGNORED: 1024, 
  IDENTITY_INCLUDES_DOMAIN: 2048, 

  get authFlags() { 
    return this.CONNECTION_BASED | this.IDENTITY_IGNORED; 
  }
}; 

gNymbleAuthenticatorComponent.challengeReceived = function(channel, challenge, proxyAuth, sessionState, continuationState, invalidatesIdentity) {
  invalidatesIdentity.value = false;
};

gNymbleAuthenticatorComponent.generateCredentials = function(aChannel, aChallenge, aProxyAuth, aDomain, aUser, aPassword, aSessionState, aContinuationState) {   
  
  
  for each (challengeWord in aChallenge.split(/\s+/)) {
    if (challengeWord.indexOf('blacklist') == 0) {
      // TODO: do this more cleanly
      var blacklistURL = challengeWord.substr(11, challengeWord.length - 12);
      
      var ticket = this.getNymbleTicket(aChannel.URI.host + ":" + aChannel.URI.port, blacklistURL); 
      
      if (!ticket) {
        // Seems to be the only way to break out of the infinite authentication loop that occurs otherwise
        // Components.returnCode would do what we need, but it's been broken since 2005 (bug 287107) :(
        throw Components.Exception("Blacklisted");
      } else {
        //var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
        //                              .getService(Components.interfaces.nsIPromptService);
        //promptService.alert(null, "Nymble Authenticator", "Sending ticket: " + ticket);
        
        return ticket;
      }
    }
  }
  
  var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                .getService(Components.interfaces.nsIPromptService);
  promptService.alert(null, "Nymble Authenticator", "Invalid authentication challenge");
  throw "Invalid auth challenge";
}; 

gNymbleAuthenticatorComponent.initializeUser = function() {
  var nymbleService = Components.classes["@nymble.cs.dartmouth.edu/nymble/Nymble;1"]
                                .getService(Components.interfaces.INymble);

  var xhr = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance()

  xhr.open('GET', "http://localhost:3000/pseudonym", false);
  xhr.send(null);

  if (xhr.readyState == 4) {
    if (xhr.status == 200) {
      nymbleService.user_initialize(xhr.responseText, null, 0);
      
      this.pseudonym = xhr.responseText;
      this.initialized = true;
    } else {
      var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                    .getService(Components.interfaces.nsIPromptService);
                                  
      promptService.alert(null, "Nymble Authenticator", "could not get pseudonym");
      // throw some kind of error
    }
  }  
}

gNymbleAuthenticatorComponent.getNymbleTicket = function(server, blacklist) {
  var nymbleService = Components.classes["@nymble.cs.dartmouth.edu/nymble/Nymble;1"]
                                .getService(Components.interfaces.INymble);
  
  // Should only be called when necessary
  if (!this.initialized) {
    this.initializeUser();
  }
  
  if (!nymbleService.user_entry_exists(server)) {
    if (!this.getNymbleCredential(server)) {
      return null;
    }
  }
  
  if (this.checkBlacklist(server, blacklist)) {
    var now = new Date();
    
    var link_window = 366 * (now.getUTCFullYear() - 1970) + this.getUTCDayOfYear(now);
    var time_period  = (now.getUTCHours() * 60 + now.getUTCMinutes()) / 5;
    
    return nymbleService.user_credential_get(server, time_period);
  } else {
    var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                  .getService(Components.interfaces.nsIPromptService);
                                
    promptService.alert(null, "Nymble Authenticator", "You appear to have been blacklisted from this server, or its blacklist has been tampered with since it was signed.");
    
    // We actually want to break out of the infinite auth loop that will result from this.
    return null;
  }
}

gNymbleAuthenticatorComponent.checkBlacklist = function(server, blacklist) {
  var nymbleService = Components.classes["@nymble.cs.dartmouth.edu/nymble/Nymble;1"]
                                .getService(Components.interfaces.INymble);

  var xhr = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance()

  // Build URL more nicely
  xhr.open('GET', "http://" + server + blacklist, false);
  xhr.send(null);

  if (xhr.readyState == 4) {
    if (xhr.status == 200) {
      var now = new Date();
      
      var link_window = 366 * (now.getUTCFullYear() - 1970) + this.getUTCDayOfYear(now);
      var time_period  = (now.getUTCHours() * 60 + now.getUTCMinutes()) / 5;
      
      var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                    .getService(Components.interfaces.nsIPromptService);
      
      var updated = nymbleService.user_blacklist_update(server, xhr.responseText, link_window, time_period);
      
      if (!updated) {
        var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                      .getService(Components.interfaces.nsIPromptService);

        promptService.alert(null, "Nymble Authenticator", "could not update blacklist");
        
        return false;
      }
      
      var blacklisted = nymbleService.user_blacklist_check(server);
      
      return !blacklisted;
    } else {
      var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                    .getService(Components.interfaces.nsIPromptService);

      promptService.alert(null, "Nymble Authenticator", "could not retrieve blacklist from server");
    }
  }  
  
  return false;
}

// http://www.webdeveloper.com/forum/showthread.php?t=125428
gNymbleAuthenticatorComponent.getUTCDayOfYear = function(date) {   
  // d is a Date object
  var yn = date.getUTCFullYear();
  var mn = date.getUTCMonth();
  var dn = date.getUTCDate();
  var d1 = new Date(yn, 0, 1, 12, 0, 0); // noon on Jan. 1
  var d2 = new Date(yn, mn, dn, 12, 0, 0); // noon on input date
  var ddiff = Math.round((d2 - d1) / 864e5);
  return ddiff + 1; 
}

gNymbleAuthenticatorComponent.getNymbleCredential = function(server) {
  
  /*
  // Not needed, as we're processing the raw cert internally
  
  // TODO: Move me somewhere more permanent, as there's no need to recompute every time
  
  var sbs = Components.classes["@mozilla.org/intl/stringbundle;1"].getService(Components.interfaces.nsIStringBundleService);
  var props = sbs.createBundle("chrome://pipnss/locale/pipnss.properties");
  
  var enumerator = props.getSimpleEnumeration()
  
  var reverse_strings = new Array();
  
  while (enumerator.hasMoreElements()) {
    var pair = enumerator.getNext().QueryInterface(Components.interfaces.nsIPropertyElement);
    
    reverse_strings[pair.value] = pair.key;
    
  }
  */
  
  var nymbleService = Components.classes["@nymble.cs.dartmouth.edu/nymble/Nymble;1"]
                                .getService(Components.interfaces.INymble);

  var xhr = Components.classes["@mozilla.org/xmlextras/xmlhttprequest;1"].createInstance()
  
  xhr.open('POST', "https://localhost:3001/credential", false);
  
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  
  xhr.send("server_id=" + encodeURIComponent(server) + "&pseudonym=" + encodeURIComponent(this.pseudonym));
    
  if (xhr.readyState == 4) {
    if (xhr.status == 200) {
      var serverCert = xhr.channel.securityInfo.QueryInterface(Components.interfaces.nsISSLStatusProvider).SSLStatus.QueryInterface(Components.interfaces.nsISSLStatus).serverCert.QueryInterface(Components.interfaces.nsIX509Cert)
      
      var length = {};
      var bytes = serverCert.getRawDER(length);
      
      nymbleService.user_set_certificate(bytes, bytes.length);
      
      /*
      // There appears to be a bug in firefox's certificate decoding, where it returns to us a key that is too big, so
      // for now we just process the der ourselves
      
      var certDumpTree = Components.classes["@mozilla.org/security/nsASN1Tree;1"].createInstance(Components.interfaces.nsIASN1Tree);
      
      var ASN1Structure = serverCert.ASN1Structure.QueryInterface(Components.interfaces.nsIASN1Object)
      
      certDumpTree.loadASN1Structure(ASN1Structure)
      
      // Who the hell includes a GUI widget as a member variable of an x509 certificate class, as the 
      // only way of obtaining the full set of properties of the cert??
      var tree = certDumpTree.QueryInterface(Components.interfaces.nsIASN1Tree);
    
      var pubkey_text = ""

      for (i = 0; i < tree.rowCount; i++) {
        var property_name = reverse_strings[tree.getCellText(i, null)];
        
        if (property_name == "CertDumpSubjPubKey") {
          pubkey_text = tree.getDisplayData(i);
        }
      }
      
      var bytes = pubkey_text.substr(pubkey_text.indexOf("\n") + 1); //.split(/\s/);  
      
      var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                    .getService(Components.interfaces.nsIPromptService);                              
      promptService.alert(null, "Nymble Authenticator", bytes);
      */
      
      
      // TODO: replace the hardcoded value with a nicer global somewhere
      nymbleService.user_entry_initialize(server, xhr.responseText, 288);
            
      return true;
    } else {
      var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                    .getService(Components.interfaces.nsIPromptService);
      promptService.alert(null, "Nymble Authenticator", "Error: " + xhr.responseText);
      // throw some kind of error
      
      return false;
    }
  }
  
}

/* nsISupports */ 
gNymbleAuthenticatorComponent.QueryInterface = function(aIID) { 
  if (aIID.equals(Components.interfaces.nsISupports) || aIID.equals(Components.interfaces.nsIHttpAuthenticator) || aIID.equals(Components.interfaces.nsIFactory)) { 
    return this; 
  } 
  
  throw Components.results.NS_ERROR_NO_INTERFACE; 
  return null; 
}; 

/* nsIFactory */ 
gNymbleAuthenticatorComponent.createInstance = function(aOuter, aIID) { 
  if (aOuter != null) {
    throw Components.results.NS_ERROR_NO_AGGREGATION; 
  }
  
  this.last_accessed_times = new Array();
  
  return this.QueryInterface(aIID); 
}; 

gNymbleAuthenticatorComponent.lockFactory = function(aLock) {}; 

var gNymbleAuthenticatorModule = { 
  /* nsISupports */ 
  QueryInterface: function(aIID) { 
    if (aIID.equals(Components.interfaces.nsIModule) || aIID.equals(Components.interfaces.nsISupports)) {
      return this;
    }

    throw Components.results.NS_ERROR_NO_INTERFACE; 
  }, 

  /* nsIModule */ 
  getClassObject: function(compMgr, cid, iid) { 
    if (cid.equals(COMPONENT_CID)) {
      return gNymbleAuthenticatorComponent.QueryInterface(iid); 
    }
    
    throw Components.results.NS_ERROR_NO_INTERFACE; 
  }, 

  registerSelf: function(aCompMgr, aFileSpec, aLocation, aType) { 
    var compReg = aCompMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar); 

    compReg.registerFactoryLocation(COMPONENT_CID, 
                                    COMPONENT_PRETTY_NAME, 
                                    COMPONENT_CONTRACTID, 
                                    aFileSpec, aLocation, aType); 
  }, 

  unregisterSelf: function(aCompMgr, aLocation, aType) { 
    var compReg = aCompMgr.QueryInterface(nsIComponentRegistrar); 
    compReg.unregisterFactoryLocation(COMPONENT_CID, aLocation); 
  }, 

  canUnload: function(compMgr) { 
    return true; 
  } 
}; 

function NSGetModule(compMgr, fileSpec) { 
  return gNymbleAuthenticatorModule; 
}