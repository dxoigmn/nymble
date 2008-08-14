#!/usr/bin/env ruby

require 'plot_setup'

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "nymble_performance_unmarshal.pdf"
    plot.terminal 'pdf fsize 12'
    plot.title    'Nymble Data Structures Unmarshal'
    plot.ylabel   'Time (ms)'
    plot.xlabel   'Number of Entries'
    plot.xrange   '[0:1000]'
    #plot.yrange   '[0:0.20]'
    plot.key      'top left'
    
    plot.data << graph('blacklist_unmarshal.dat', 'Blacklist', 1000)
    plot.data << graph('credential_unmarshal.dat', 'Credential', 1000)
    plot.data << graph('ticket_unmarshal.dat', 'Ticket', 1000)
    plot.data << graph('linking_token_unmarshal.dat', 'Linking Token', 1000)
  end
end
