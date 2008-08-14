#!/usr/bin/env ruby

require 'benchmark_setup'

File.open("#{File.basename(__FILE__, '.rb')}.dat", 'w') do |f|
  user = @users.first
  
  1001.times do |number_of_credentials|
    credential = @nm.create_credential(@server.server_id, user.pseudonym, number_of_credentials)
    marshalled = credential.marshal
    
    bm = Benchmark.measure do
      RETEST_COUNT.times do
        Nymble::Credential::unmarshal(marshalled)
      end
    end
    
    bm /= RETEST_COUNT
    
    f << "#{number_of_credentials}\t#{bm.real}\n"
    
    puts "#{Time.now}\t#{number_of_credentials}" if number_of_credentials % 100 == 0
  end
end