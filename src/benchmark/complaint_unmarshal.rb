#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  f << "0\t0\n"
  
  @users.size.times do |number_of_users|
    complaints = @users[0..number_of_users].map { |user| user[@server.server_id].ticket(user.time_period).complain(user.time_period) }
    marshalled = complaints.map { |complaint| complaint.marshal }
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        marshalled.map { |complaint| Nymble::Complaint::unmarshal(complaint) }
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_users+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end