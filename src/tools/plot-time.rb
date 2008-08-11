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
    plot.output   "#{ARGV[1] || 'nymble_authentication.pdf'}"
    plot.terminal 'pdf fsize 9'
    plot.title    'Nymble Performance'
    plot.ylabel   'Time (sec)'
    plot.xlabel   'Number of blacklist entries'
    plot.xrange   '[0:2000]'
    #plot.yrange   '[0:0.18]'
    plot.key      'top left'

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

    values = @values['user_blacklist_update_average_2']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'User blacklist verify'
      ds.with   = 'linespoints 1'
    end

    values = @values['user_blacklist_check_fail_average']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'User blacklist check'
      ds.with   = 'linespoints 2'
    end
    
    values = @values['server_ticket_verify_pass_average']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'Server authentication'
      ds.with   = 'linespoints 3'
    end
    
    values = @values['server_iterate']
    values   = values.to_a.sort_by { |x, y| x }
    x_values = values.map { |x, y| x }
    y_values = values.map { |x, y| y }
    
    plot.data << Gnuplot::DataSet.new([x_values, y_values]) do |ds|
      ds.title  = 'Server iterate'
      ds.with   = 'linespoints 4'
    end
  end
end
