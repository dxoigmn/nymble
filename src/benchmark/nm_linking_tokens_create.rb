#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  server_id = @server.server_id
  blacklist = @server.blacklist
  
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      fail unless @nm.create_tokens(server_id, blacklist, [])
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "0\t#{bm.real}\n"
  
  @users.size.times do |number_of_users|
    @server = create_server(@nm, @server_id)
    
    complaints = []
    
    @users.each_with_index do |user, index|
      next unless index <= number_of_users
      
      complaints << user.ticket(@server.server_id).complain(user.time_period)
    end
    
    server_id = @server.server_id
    blacklist = @server.blacklist
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        fail unless @nm.create_tokens(server_id, blacklist, complaints)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end