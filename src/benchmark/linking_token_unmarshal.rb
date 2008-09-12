#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  f << "0\t0\n"
  
  @users.size.times do |number_of_users|
    @nm = create_nm
    @server = create_server(@nm, @server_id)
    complaints = @users[0..number_of_users].map { |user| user.ticket(@server.server_id).complain(user.time_period) }
    blacklist = @nm.create_blacklist(@server.server_id)
    tokens = @nm.create_tokens(@server.server_id, blacklist, complaints)
    marshalled = tokens.map { |token| token.marshal }
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        marshalled.map { |token| Nymble::Token::unmarshal(token) }
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end