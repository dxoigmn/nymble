#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      @server.time_period = @server.time_period + 1
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "0\t#{bm.real}\n"
  
  @users.size.times do |number_of_users|
    @server = create_server(@nm, @server_id)
    
    tickets = []
    
    @users.each_with_index do |user, index|
      next unless index <= number_of_users
      
      tickets << user.ticket(@server.server_id)
    end
    
    linking_tokens = @nm.create_linking_tokens(@server.server_id, @server.blacklist, tickets)
    @server.add_linking_tokens(linking_tokens)
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        @server.time_period = @server.time_period + 1
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end