#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_time_nm.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Nymble Manager Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('nm_blacklist_update.dat', 'UpdateBL', 1000)
    plot.data << graph('nm_credential_create.dat', 'CreateCredential', 1000)
    plot.data << graph('nm_linking_tokens_create.dat', 'ComputeTokens', 1000)
    plot.data << graph('nm_blacklist_verify.dat', 'NMCheckBLIntegrity', 1000)
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_time_server.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Server Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('server_iterate.dat', 'ServerUpdateState', 1000)
    plot.data << graph('server_blacklist_check.dat', 'UserIsBlacklisted', 1000)
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_time_user.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'User Performance'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('user_blacklist_verify.dat', 'CheckBLIntegrity', 1000)
    plot.data << graph('user_blacklist_check.dat', 'IAmBlacklisted', 1000)
  end
end
