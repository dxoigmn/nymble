#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      @server.blacklist.marshal
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "0\t#{bm.real}\n"
  
  @users.each_with_index do |user, index|
    tickets = [ user.ticket(@server.server_id) ]
    @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, tickets)
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        @server.blacklist.marshal
      end
    end
    
    f << "#{index+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{index}" if index % 100 == 0
  end
end