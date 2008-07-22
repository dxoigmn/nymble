netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
Components.utils.import("resource://gre/modules/JSON.jsm");

function acquire_pseudonym() {
  var xpcom = Components.classes['@nymble.cs.dartmouth.edu/nymble/NymbleUser;1'].createInstance();
  var user = xpcom.QueryInterface(Components.interfaces.nsINymbleUser);
  var req = new XMLHttpRequest();
  
  req.open('POST', 'http://0.0.0.0:3000/pseudonym', false);
  req.send(null);

  if (req.status != 200) {
    return null;
  }
  
  response = JSON.fromString(req.responseText);
  user.setPseudonym(response.pseudonym);
  
  return user;
}

function acquire_blacklist(user) {
  var req = new XMLHttpRequest();
  
  req.open('GET', 'http://localhost:3002/nymble/', false);
  req.send(null);
  
  if (req.status != 200) {
    return null;
  }
  
  response = JSON.fromString(req.responseText);
  
  return user.addBlacklist(response.blacklist);
}

function acquire_credential(user, server_id) {
  var req = new XMLHttpRequest();
  
  req.open('GET', 'http://localhost:3001/server/' + server_id + '/?pseudonym=' + user.getPseudonym(), false);
  req.send(null);
  
  if (req.status != 200) {
    return null;
  }
  
  response = JSON.fromString(req.responseText);
  
  return user.addCredential(response.credential);
}

function authenticate() {
  var user = acquire_pseudonym();
  
  if (!user) {
    return 'Unable to acquire pseudonym';
  }
  
  var server_id = acquire_blacklist(user);
  
  if (!server_id) { 
    return 'Unable to acquire blacklist';
  }
  
  if (!acquire_credential(user, server_id)) {
    return 'Unable to acquire credential';
  }
  
  var ticket = user.getTicket(server_id);

  if (!ticket) {
    return 'Unable to get ticket';
  }
  
  var req = new XMLHttpRequest();
  var params = 'ticket=' + escape(ticket);
  
  req.open('POST', 'http://localhost:3002/nymble/', false);
  req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  req.setRequestHeader("Content-length", params.length);
  req.setRequestHeader("Connection", "close");
  
  req.send(params);
  
  if (req.status != 200) {
    return null;
  }
  
  return req.responseText;
}

document.write(authenticate());