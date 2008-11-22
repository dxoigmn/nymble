#!/usr/bin/env ruby

require 'benchmark_setup'

plot __FILE__ do
  nm = create_nm
  pm = create_pm(nm)
  server = create_server(nm, 'server_id')
  user = create_users(1, nm, pm, 'server_id').first

  benchmarks = Hash.new(0.0)
  
  1441.times do |time_periods|
    next unless time_periods == 0 || 1440 % time_periods == 0
    
    puts time_periods
    
    nm.reset!(time_periods)
    server = Nymble::Server.new(nm.register_server('server_id'))
    pseudonym = user.pseudonym
    
    fail unless nm.create_credential('server_id', pseudonym)
    
    100.times do
      benchmarks[time_periods] += Benchmark.realtime { nm.create_credential('server_id', pseudonym) }
    end
    
    benchmarks[time_periods] /= 100
  end
  
  benchmarks
end
