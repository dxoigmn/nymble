var nymble = {
  onLoad: function() {
    // initialization code
    this.initialized = true;
    this.strings = document.getElementById("nymble-strings");
    
    this.registerObservers();
  },
  
  onUnload: function() {
    this.unregisterObservers();
  },
  
  registerObservers: function() {
    var observerService = Components.classes["@mozilla.org/observer-service;1"]
                                    .getService(Components.interfaces.nsIObserverService);
    
    observerService.addObserver(this, "http-on-modify-request", false);
  },
  
  unregisterObservers: function() {
    var observerService = Components.classes["@mozilla.org/observer-service;1"]
                                    .getService(Components.interfaces.nsIObserverService);
    
    observerService.removeObserver(this, "http-on-modify-request");
  },
  
  getNymbleTicket: function(url) {
    return "ticket!";
  },
  
  onModifyRequest: function(channel) {
    //if (authenticate[channel.referrer]) {
    //  channel.setRequestHeader("X-NymbleTicket", getNymbleTicket(channel.referrer), false);
    //}
  },
  
  observe: function(subject, topic, data) {    
    if (topic == "http-on-modify-request") {
      this.onModifyRequest(subject.QueryInterface(Components.interfaces.nsIHttpChannel));
    }
    return null;               
  },
  
  onMenuItemCommand: function(e) {
    var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
                                  .getService(Components.interfaces.nsIPromptService);
    promptService.alert(window, "random box", "why");
    
    // this.strings.getString("helloMessage")
  },
};

window.addEventListener("load", function(e) { nymble.onLoad(e); }, false);
window.addEventListener("unload", function(e) { nymble.onUnload(e); }, false);
