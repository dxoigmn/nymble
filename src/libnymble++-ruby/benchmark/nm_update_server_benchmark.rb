#!/usr/bin/env ruby

require 'benchmark_setup'

plot __FILE__ do
  nm = create_nm
  pm = create_pm(nm)
  
  benchmarks = Hash.new(0.0)
  
  101.times do |count|
    puts count
    
    100.times do
      nm.reset!
      server = create_server(nm, 'server_id')
      users = create_users(count, nm, pm, 'server_id')
      
      users.each { |user| server.add_complaint(user.ticket('server_id'), user.time_period) }
      
      nm.time_period += 1
      complaint = server.complain!
      benchmarks[count] += Benchmark.realtime { nm.update_server('server_id', complaint) }
    end
    
    benchmarks[count] /= 100
  end
  
  benchmarks
end