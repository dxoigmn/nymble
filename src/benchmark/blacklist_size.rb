#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  f << "0\t#{@server.blacklist.marshall.size}\n"
  
  @users.each_with_index do |user, index|
    tickets = [ user.ticket(@server.server_id) ]
    @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, tickets)
    
    f << "#{index+1}\t#{@server.blacklist.marshall.size}\n"
    
    puts "#{Time.now}\t#{index}" if index % 100 == 0
  end
end