#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  @users.size.times do |number_of_users|
    @server = create_server(@nm, @server_id)
    
    tickets = []
    
    @users.each_with_index do |user, index|
      next unless index <= number_of_users
      
      user.time_period = @server.time_period # FIXME: Causes create_linking_tokens to go into an infinite loop if not present
      
      tickets << user.ticket(@server.server_id)
    end
    
    linking_tokens = @nm.create_linking_tokens(@server.server_id, @server.blacklist, tickets)
    @server.add_linking_tokens(linking_tokens)
    
    @server.time_period = @server.time_period + 1
    @users[number_of_users].time_period = @server.time_period
    
    ticket = @users[number_of_users].ticket(@server.server_id)
    
    bm = Benchmark.measure { @server.valid_ticket?(ticket) }
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end
