#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_times_nm.pdf"
    plot.terminal 'pdf fsize 11'
    #plot.title    'Nymble Manager Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    plot.yrange   '[0:32]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('nm_create_credential_benchmark.rb.dat', 'Credential acquisition', 1000)
    plot.data << graph('nm_update_server_benchmark.rb.dat', 'Blacklist update', 100)
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_times_server_user.pdf"
    plot.terminal 'pdf fsize 11'
    #plot.title    'Server & User Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    #plot.xrange   '[0:1000]'
    plot.yrange   '[0:5.9]'
    plot.key      '850,5.8'
    
    plot.data << graph('user_blacklisted_benchmark.rb.dat', 'User blacklist validation and privacy check', 1000)
    plot.data << graph('server_update_benchmark.rb.dat', 'Server blacklist update', 1000)
    plot.data << graph('server_update_2_benchmarks.rb.dat', 'Server update state', 1000)
    plot.data << graph('server_valid_ticket_benchmark.rb.dat', 'Server ticket examination', 1000)
  end
end
