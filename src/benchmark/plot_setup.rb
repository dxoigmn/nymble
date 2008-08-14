require 'rubygems'
require 'gnuplot'

def graph(file, title, yscale=1)
  values = []
  
  File.open(file) do |f|
    values = f.readlines.map { |line| line.chomp.split("\t").map { |value| value.to_f } }
  end
  
  values.delete_if { |x, y| x % 50 != 0 }
  
  x_values = values.map { |x, y| x }
  y_values = values.map { |x, y| y * yscale.to_f }
  
  Gnuplot::DataSet.new([x_values, y_values]) do |ds|
    ds.title  = title
    ds.with   = 'linespoints'
  end
end
