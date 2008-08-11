#!/usr/bin/ruby

fail unless ARGV.length > 0

i = 0

while i <= 2000
  puts "benchmarking for #{i} users"
  `./benchmark.rb #{i} >> #{ARGV[0]}`
  i += 50
end
