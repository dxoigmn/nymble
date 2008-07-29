var nymble = {
  init: function() {
    var appcontent = window.document.getElementById("appcontent");
    
    if(appcontent) {
      appcontent.addEventListener("DOMContentLoaded", nymble.onPageLoad, false);
    }
  },
  
  run: function(doc) {
    if (!doc) {
      doc = window.top.getBrowser().selectedBrowser.contentWindow.document;
    }
    
    jQuery('form', doc).each(function() {
      var form = this;
      var pseudonym_manager = jQuery('link[rel="nymble.pseudonym_manager"]', doc).get(0).href;
      var nymble_manager    = jQuery('link[rel="nymble.manager"]', doc).get(0).href;
      var server_blacklist  = jQuery('link[rel="nymble.blacklist"]', doc).get(0).href;
      
      if (!pseudonym_manager || !nymble_manager || !server_blacklist || !confirm("Do you want to acquire a nymble to authenticate with?")) {
        return null;
      }
      
      var user = Components.classes['@nymble.cs.dartmouth.edu/nymble/NymbleUser;1'].createInstance().QueryInterface(Components.interfaces.nsINymbleUser);
      var cur_time = new Date();
      
      user.setLinkWindow(366 * (cur_time.getUTCFullYear() - 1970) + nymble.getUTCDayOfYear(cur_time));
      user.setTimePeriod(cur_time.getUTCHours() * 60 + cur_time.getUTCMinutes());
      
      var manager = Components.classes["@mozilla.org/extensions/manager;1"].getService(Components.interfaces.nsIExtensionManager);
      var file = manager.getInstallLocation('nymble@cs.dartmouth.edu').getItemFile('nymble@cs.dartmouth.edu', 'components/nm_pub.key');
      
      user.readVerifyKey(file.path);
      
      jQuery.post(pseudonym_manager, null, function(data, textStatus) {
        user.setPseudonym(data.pseudonym);
        
        jQuery.get(server_blacklist, null, function(data, textStatus) {
          server_id = user.addBlacklist(data.blacklist);
          
          jQuery.get(nymble_manager + server_id + '/', { 'pseudonym': user.getPseudonym() }, function(data, textStatus) {
            user.addCredential(data.credential);
            
            jQuery('input[id="nymble_ticket"]', doc)[0].value = user.getTicket(server_id);
          }, 'json');
        }, 'json');
      }, 'json');
    });
  },

  showAbout: function() {
    alert("Nymble, created by Nymble");
  },

  goHome: function() {
    var win = window.top.getBrowser().selectedBrowser.contentWindow;
    win.open("http://www.cs.dartmouth.edu/~nymble/");
  },
  
  onPageLoad: function(evt) {
    nymble.run(evt.originalTarget);
  },
  
  getUTCDayOfYear: function(date) {   
    // d is a Date object
    var yn = date.getUTCFullYear();
    var mn = date.getUTCMonth();
    var dn = date.getUTCDate();
    var d1 = new Date(yn, 0, 1, 12, 0, 0); // noon on Jan. 1
    var d2 = new Date(yn, mn, dn, 12, 0, 0); // noon on input date
    var ddiff = Math.round((d2 - d1) / 864e5);
    return ddiff + 1; 
  },
}

window.addEventListener("load", function() { nymble.init(); }, false);