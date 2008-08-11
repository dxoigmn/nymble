#!/usr/bin/env ruby

require 'rubygems'
require 'gnuplot'

fail 'Usage: ./plot.rb [data filename] [output filename]' if ARGV.size == 0

@values = {}

File.open(ARGV[0]) do |f|
  lines = f.readlines
  
  titles = lines.shift.chomp.split(',')
  
  lines.each do |line|
    values      = line.chomp.split(',')
    user_count  = values.shift.to_i
    
    values.each_with_index do |value, index|
      title = titles[index + 1]
      
      @values[title] ||= {}
      @values[title][user_count] = value.to_f
    end
  end
end

Gnuplot.open do |gp|
  Gnuplot::Plot.new(gp) do |plot|
    plot.output   "#{ARGV[1] || 'nymble_performance_size.pdf'}"
    plot.terminal 'pdf fsize 9'
    plot.title    'Nymble Performance (size)'
    plot.ylabel   'Size (bytes)'
    plot.xlabel   'Number of users'
    #plot.xrange   '[1:1024]'
    #plot.yrange   '[0:0.18]'
    plot.key      'top left'
    #plot.logscale 'y'
    #plot.logscale 'x'

=begin
    @values.each do |title, values|
      values = values.to_a.sort_by { |x, y| x }
      x_values = values.map { |x, y| x }
      y_values = values.map { |x, y| y }
      
      plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
        ds.title = title
        ds.with  = 'linespoints'
      end
    end
=end

    values = @values['blacklist_size']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'Blacklist'
      ds.with   = 'linespoints 1'
    end

    values = @values['credential_size']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'Credential'
      ds.with   = 'linespoints 2'
    end
    
    values = @values['ticket_size']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'Ticket'
      ds.with   = 'linespoints 3'
    end
  end
end
