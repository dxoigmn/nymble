#!/usr/bin/env ruby

require 'benchmark_setup'

def benchmark
  nm = create_nm
  pm = create_pm(nm)

  1001.times do |count|
    next unless count % 50 == 0
    puts count
    
    bm = Benchmark::Tms.new
    
    10.times do
      nm.reset!
      server = create_server(nm, 'server_id')
      users = create_users(count, nm, pm, 'server_id')
      
      users.each { |user| server.add_complaint(user.ticket('server_id'), user.time_period) }
      
      nm.time_period += 1
      update = nm.update_server('server_id', server.complain!)
      
      bm += Benchmark.measure { server.update!(update) }
    end
    
    yield count, bm / 10
  end
end

write __FILE__