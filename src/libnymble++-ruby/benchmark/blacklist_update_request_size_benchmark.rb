#!/usr/bin/env ruby

require 'benchmark_setup'

def benchmark
  nm = create_nm
  pm = create_pm(nm)
  server = create_server(nm, 'server_id')
  users = create_users(1500, nm, pm, 'server_id')
  
  yield 0, server.blacklist.size
  
  users.size.times do |count|
    next unless (count + 1) % 50 == 0
    
    nm.reset!
    server = create_server(nm, 'server_id')
    nm.time_period += 1
    
    users[0..count].each do |user|
      server.add_complaint(user.ticket('server_id'), user.time_period)
    end
    
    yield count + 1, server.complain!.size
  end
end

write __FILE__
