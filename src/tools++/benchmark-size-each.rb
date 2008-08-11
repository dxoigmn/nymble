#!/usr/bin/ruby

fail unless ARGV.length > 0

i = 0

while i <= 1000
  puts "benchmarking for #{i} users"
  `./benchmark-size.rb #{i} >> #{ARGV[0]}`
  
  i += 50
end
