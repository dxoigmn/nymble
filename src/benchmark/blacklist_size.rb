#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  f << "0\t#{@server.blacklist.marshal.size}\n"
  
  @users.each_with_index do |user, index|
    complaints = [ user.ticket(@server.server_id).complain(user.time_period) ]
    @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, complaints)
    
    f << "#{index+1}\t#{@server.blacklist.marshal.size}\n"
    
    puts "#{Time.now}\t#{index}" if index % 100 == 0
  end
end