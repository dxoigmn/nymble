#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_size.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Nymble Data Structures'
    plot.ylabel   'Size (KB)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('blacklist_size.dat', 'Blacklist', 1.0/1024)
    plot.data << graph('credential_size.dat', 'Credential', 1.0/1024)
    plot.data << graph('ticket_size.dat', 'Ticket', 1.0/1024)
    plot.data << graph('linking_token_size.dat', 'Linking Token', 1.0/1024)
  end
end
