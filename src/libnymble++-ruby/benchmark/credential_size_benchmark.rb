#!/usr/bin/env ruby

require 'benchmark_setup'

plot __FILE__ do
  nm = create_nm
  pm = create_pm(nm)
  server = create_server(nm, 'server_id')
  user = create_users(1, nm, pm, 'server_id').first
  
  benchmarks = {}
  
  1441.times do |time_periods|
    next unless time_periods == 0 || 1440 % time_periods == 0
    
    nm.reset!(time_periods)
    server = Nymble::Server.new(nm.register_server('server_id'))
    benchmarks[time_periods] = nm.create_credential('server_id', user.pseudonym).size
  end
  
  benchmarks
end
