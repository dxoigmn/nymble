#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  ticket = @users.first.ticket(@server.server_id)
  
  f << "#{ticket.marshall.size}\n"
end