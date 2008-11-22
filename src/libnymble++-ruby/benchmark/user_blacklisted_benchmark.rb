#!/usr/bin/env ruby

require 'benchmark_setup'

plot __FILE__ do
  nm = create_nm
  pm = create_pm(nm)
  
  benchmarks = Hash.new(0.0)

  101.times do |count|
    puts count
    
    nm.reset!
    server = create_server(nm, 'server_id')
    users = create_users(count + 1, nm, pm, 'server_id')
    test_user = users.shift
    
    users.each { |user| server.add_complaint(user.ticket('server_id'), user.time_period) }
    
    nm.time_period += 1
    
    server.update!(nm.update_server('server_id', server.complain!))
    
    test_user.time_period = server.time_period
    blacklist = server.blacklist
    cert = server.cert
    
    fail if test_user.blacklisted?('server_id', blacklist, cert)
    
    100.times do
      benchmarks[count] += Benchmark.realtime { test_user.blacklisted?('server_id', blacklist, cert) }
    end
    
    benchmarks[count] /= 100
  end
  
  benchmarks
end