#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_times_nm.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Nymble Manager Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    plot.yrange   '[0:35]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('nm_create_credential_benchmark.rb.dat', 'Create Credential', 1000)
    plot.data << graph('nm_update_server_benchmark.rb.dat', 'Update Server', 100)
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_times_server.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Server Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    #plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('server_update_benchmark.rb.dat', 'Blacklist Update', 1000)
    plot.data << graph('server_update_2_benchmarks.rb.dat', 'Update State', 1000)
    plot.data << graph('server_valid_ticket_benchmark.rb.dat', 'Verify Ticket', 1000)
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_times_user.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'User Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    #plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('user_blacklisted_benchmark.rb.dat', 'Check Blacklist', 1000)
  end
end
