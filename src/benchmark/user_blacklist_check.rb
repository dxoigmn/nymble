#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  user = @users.first
  index = 0
  
  user.add_blacklist(@server.server_id, @server.blacklist)
  
  server_id = @server.server_id
  
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      user.ticket(server_id)
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "#{index}\t#{bm.real}\n"
  
  @users.each_with_index do |user, index|
    complaints = [ user.ticket(@server.server_id).complain(user.time_period) ]
    @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, complaints)
    
    user.add_blacklist(@server.server_id, @server.blacklist)
    
    server_id = @server.server_id
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        fail if user.ticket(server_id)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{index+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{index}" if index % 100 == 0
  end
end
