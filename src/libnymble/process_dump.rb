#!/usr/bin/env ruby

File.open(ARGV[0]) do |f|
  lines = f.readlines
  
  @mallocs  = []
  @frees    = []

  lines.each do |line|
    line.chomp!
    
    if line =~ /^malloc\((.*),(.*)\)$/
      @mallocs << [$1, $2]
    end
    
    if line =~ /^free\((.*),(.*)\)$/
      @frees << [$1, $2]
    end
  end
  
  @mallocs.each do |malloc_addr, malloc_type|
    free = @frees.find { |free_addr, free_type| malloc_addr == free_addr }

    
    if free
      puts "#{malloc_type} @ #{malloc_addr} matches #{free[1]}" unless free[1] == malloc_type
    else
      puts "Unable to find free for #{malloc_type} @ #{malloc_addr}"
    end
  end
end