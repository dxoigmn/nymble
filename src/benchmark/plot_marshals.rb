#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_marshal.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Nymble Data Structures Marshal'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('blacklist_marshal.dat', 'Blacklist', 1000)
    plot.data << graph('credential_marshal.dat', 'Credential', 1000)
    plot.data << graph('ticket_marshal.dat', 'Ticket', 1000)
    plot.data << graph('linking_token_marshal.dat', 'Linking Token', 1000)
  end
end
