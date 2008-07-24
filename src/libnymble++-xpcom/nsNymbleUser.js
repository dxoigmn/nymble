google.load('jquery', '1');

google.setOnLoadCallback(function() {
  netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
  
  jQuery('form').each(function() {
    var form = this;
    var pseudonym_manager = jQuery('link[rel="nymble.pseudonym_manager"]').get(0).href;
    var nymble_manager    = jQuery('link[rel="nymble.manager"]').get(0).href;
    var server_blacklist  = jQuery('link[rel="nymble.blacklist"]').get(0).href;
    var user = Components.classes['@nymble.cs.dartmouth.edu/nymble/NymbleUser;1'].createInstance().QueryInterface(Components.interfaces.nsINymbleUser);
    
    jQuery('p.status').append('Getting pseudonym...');
    
    jQuery.post(pseudonym_manager, null, function(data, textStatus) {
      netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
      
      user.setPseudonym(data.pseudonym);
      jQuery('p.status').append('done!<br/>Getting blacklist...');
      
      jQuery.get(server_blacklist, null, function(data, textStatus) {
        netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
        
        server_id = user.addBlacklist(data.blacklist);
        jQuery('p.status').append('done!</br>Getting credential...');
        
        jQuery.get(nymble_manager + server_id + '/', { 'pseudonym': user.getPseudonym() }, function(data, textStatus) {
          netscape.security.PrivilegeManager.enablePrivilege('UniversalXPConnect');
          
          user.addCredential(data.credential);
          
          console.log('done!');
          jQuery('input[id="nymble_ticket"]')[0].value = user.getTicket(server_id);
        }, 'json');
      }, 'json');
    }, 'json');
  });
});