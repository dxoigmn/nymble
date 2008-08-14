#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  @server.blacklist.marshal
  marshalled = @server.blacklist.marshal
  
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      Nymble::Blacklist::unmarshal(marshalled)
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "0\t#{bm.real}\n"
  
  @users.each_with_index do |user, index|
    tickets = [ user.ticket(@server.server_id) ]
    @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, tickets)
    marshalled = @server.blacklist.marshal
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        Nymble::Blacklist::unmarshal(marshalled)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{index+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{index}" if index % 100 == 0
  end
end