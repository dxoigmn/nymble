google.load('jquery', '1');

function getUTCDayOfYear(date) {   
  // d is a Date object
  var yn = date.getUTCFullYear();
  var mn = date.getUTCMonth();
  var dn = date.getUTCDate();
  var d1 = new Date(yn, 0, 1, 12, 0, 0); // noon on Jan. 1
  var d2 = new Date(yn, mn, dn, 12, 0, 0); // noon on input date
  var ddiff = Math.round((d2 - d1) / 864e5);
  return ddiff + 1; 
}

google.setOnLoadCallback(function() {
  netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
  
  jQuery('form').each(function() {
    var form = this;
    var pseudonym_manager = jQuery('link[rel="nymble.pseudonym_manager"]').get(0).href;
    var nymble_manager    = jQuery('link[rel="nymble.manager"]').get(0).href;
    var server_blacklist  = jQuery('link[rel="nymble.blacklist"]').get(0).href;
    
    if (!pseudonym_manager || !nymble_manager || !server_blacklist || !confirm("Do you want to acquire a nymble to authenticate with?")) {
      return null;
    }
    
    var user = Components.classes['@nymble.cs.dartmouth.edu/nymble/NymbleUser;1'].createInstance().QueryInterface(Components.interfaces.nsINymbleUser);
    var cur_time = new Date();
    
    user.setLinkWindow(366 * (cur_time.getUTCFullYear() - 1970) + getUTCDayOfYear(cur_time));
    user.setTimePeriod(cur_time.getUTCHours() * 60 + cur_time.getUTCMinutes());
    
    jQuery('p#status').append('Getting pseudonym...');
    
    jQuery.post(pseudonym_manager, null, function(data, textStatus) {
      netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
      
      user.setPseudonym(data.pseudonym);
      jQuery('p#status').append('done!<br/>Getting blacklist...');
      
      jQuery.get(server_blacklist, null, function(data, textStatus) {
        netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
        
        server_id = user.addBlacklist(data.blacklist);
        jQuery('p#status').append('done!</br>Getting credential...');
        
        jQuery.get(nymble_manager + server_id + '/', { 'pseudonym': user.getPseudonym() }, function(data, textStatus) {
          netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
          
          user.addCredential(data.credential);
          
          jQuery('p#status').append('done!');
          jQuery('input[id="nymble_ticket"]')[0].value = user.getTicket(server_id);
        }, 'json');
      }, 'json');
    }, 'json');
  });
});