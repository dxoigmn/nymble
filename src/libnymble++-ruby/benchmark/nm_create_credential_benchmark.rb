#!/usr/bin/env ruby

require 'benchmark_setup'

def benchmark
  nm = create_nm
  pm = create_pm(nm)
  server = create_server(nm, 'server_id')
  user = create_users(1, nm, pm, 'server_id').first
  
  1441.times do |time_periods|
    next unless time_periods == 0 || 1440 % time_periods == 0
    
    puts time_periods
    
    nm.reset!(time_periods)
    server = Nymble::Server.new(nm.register_server('server_id'))
    pseudonym = user.pseudonym
    
    fail unless nm.create_credential('server_id', pseudonym)
    
    bm = Benchmark::Tms.new
    
    10.times do
      bm += Benchmark.measure { nm.create_credential('server_id', pseudonym) }
    end
    
    yield time_periods, bm / 10
  end
end

write __FILE__