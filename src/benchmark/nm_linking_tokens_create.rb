#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  @users.size.times do |number_of_users|
    @server = create_server(@nm, @server_id)
    
    tickets = []
    
    @users.each_with_index do |user, index|
      next unless index <= number_of_users
      
      tickets << user.ticket(@server.server_id)
    end
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        fail unless @nm.create_linking_tokens(@server.server_id, @server.blacklist, tickets)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end