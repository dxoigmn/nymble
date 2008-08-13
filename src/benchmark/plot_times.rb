#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "#{ARGV[1] || 'nymble_performance_time.pdf'}"
    plot.terminal 'pdf fsize 9'
    plot.title    'Nymble Performance (time)'
    plot.ylabel   'Time (seconds)'
    plot.xlabel   'Number of Blacklist Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('nm_credential_create.dat', 'NM Credential Create')
    plot.data << graph('nm_blacklist_verify.dat', 'NM Blacklist Verify')
    plot.data << graph('nm_blacklist_update.dat', 'NM Blacklist Update')
    plot.data << graph('nm_linking_tokens_create.dat', 'NM Linking Tokens Create')
    
    plot.data << graph('server_blacklist_check.dat', 'Server Blacklist Check')
    plot.data << graph('server_iterate.dat', 'Server Time Period Change')
    
    plot.data << graph('user_blacklist_verify.dat', 'User Blacklist Verify')
    plot.data << graph('user_blacklist_check.dat', 'User Blacklist Check')
  end
end