$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'nymble'
require 'benchmark'

def create_nm
  nm = Nymble::NymbleManager.new
  nm.write_verify_key_n('verify_key_n.pem')
  nm
end

def create_pm(nm)
  Nymble::PseudonymManager.new(nm.mac_key_np)
end

def create_server(nm, server_id)
  Nymble::Server.new(nm.register_server('server_id'))
end

def create_users(count, nm, pm, server_id)
  users = (1..count).to_a.map { |user_id| Nymble::User.new(pm.create_pseudonym(user_id.to_s), 'verify_key_n.pem') }
  users.each { |user| user.add_credential(server_id, nm.create_credential(server_id, user.pseudonym)) }
  users
end

def plot(name, options = {}, &block)
  data = block.call
  
  File.open("#{name}.dat", 'w') do |f|
    f << data.to_a.sort_by { |x, y| x }.map { |a| a.flatten.join(',') }.join("\n")
  end
end