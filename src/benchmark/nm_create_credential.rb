#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  user = @users.first
  
  pseudonym = user.pseudonym
  server_id = @server.server_id
  
  1000.times do |number_of_credentials|
    bm = Benchmark.measure do
      50.times do
        @nm.create_credential(server_id, pseudonym, number_of_credentials)
      end
    end
    
    bm /= 50
    
    f << "#{number_of_credentials+1}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_credentials}" if number_of_credentials % 100 == 0
  end
end