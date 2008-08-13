#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  user = @users.first
  
  pseudonym = user.pseudonym
  server_id = @server.server_id
  
  1001.times do |number_of_credentials|
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        fail unless @nm.create_credential(server_id, pseudonym, number_of_credentials)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_credentials}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_credentials}" if number_of_credentials % 100 == 0
  end
end