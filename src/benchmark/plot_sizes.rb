#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "#{ARGV[1] || 'nymble_performance_size.pdf'}"
    plot.terminal 'pdf fsize 9'
    plot.title    'Nymble Performance (size)'
    plot.ylabel   'Size (bytes)'
    plot.xlabel   'Number of Users'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('blacklist_size.dat', 'Blacklist')
    plot.data << graph('credential_size.dat', 'Credential')
    plot.data << graph('ticket_size.dat', 'Ticket')
  end
end
