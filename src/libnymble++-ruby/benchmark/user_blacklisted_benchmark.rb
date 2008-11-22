#!/usr/bin/env ruby

require 'benchmark_setup'

def benchmark
  nm = create_nm
  pm = create_pm(nm)
  
  1001.times do |count|
    next unless count % 50 == 0
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
    
    bm = Benchmark::Tms.new
    
    10.times do
      bm += Benchmark.measure { test_user.blacklisted?('server_id', blacklist, cert) }
    end
    
    yield count, bm
  end
end

write __FILE__