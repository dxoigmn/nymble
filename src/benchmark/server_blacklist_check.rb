#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  ticket = @users.first.ticket(@server.server_id)
  
  bm = Benchmark.measure do
    RETEST_COUNT.times do
      fail unless @server.valid_ticket?(ticket)
    end
  end
  
  bm /= RETEST_COUNT
  
  f << "0\t#{bm.real}\n"
  
  @users.size.times do |number_of_users|
    @server = create_server(@nm, @server_id)
    
    complaints = []
    
    @users.each_with_index do |user, index|
      next unless index <= number_of_users
      
      user.time_period = @server.time_period # FIXME: Causes create_tokens to go into an infinite loop if not present
      
      complaints << user.ticket(@server.server_id).complain(user.time_period)
    end
    
    tokens = @nm.create_tokens(@server.server_id, @server.blacklist, complaints)
    @server.add_tokens(tokens)
    
    @server.time_period = @server.time_period + 1
    @users[number_of_users].time_period = @server.time_period
    
    ticket = @users[number_of_users].ticket(@server.server_id)
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        fail if @server.valid_ticket?(ticket)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end
