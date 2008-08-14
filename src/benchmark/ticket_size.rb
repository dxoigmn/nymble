#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  f << "0\t0\n"
  
  @users.size.times do |number_of_users|
    tickets = @users[0..number_of_users].map { |user| user.ticket(@server.server_id) }
    tickets = tickets.map { |ticket| ticket.marshall }
    
    f << "#{number_of_users+1}\t#{tickets.inject(0) { |sum, ticket| sum + ticket.size }}\n"
    
    puts "#{Time.now}\t#{number_of_users}" if number_of_users % 100 == 0
  end
end