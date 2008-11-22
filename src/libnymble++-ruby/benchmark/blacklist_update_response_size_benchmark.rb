#!/usr/bin/env ruby

require 'benchmark_setup'

plot __FILE__ do
  nm = create_nm
  pm = create_pm(nm)
  server = create_server(nm, 'server_id')
  users = create_users(100, nm, pm, 'server_id')
  
  benchmarks = {}
  
  benchmarks[0] = server.blacklist.size
  
  users.size.times do |count|
    nm.reset!
    server = create_server(nm, 'server_id')
    nm.time_period += 1
    
    users[0..count].each do |user|
      server.add_complaint(user.ticket('server_id'), user.time_period)
    end
    
    benchmarks[count + 1] = nm.update_server('server_id', server.complain!).size
  end
  
  benchmarks
end