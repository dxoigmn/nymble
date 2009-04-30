#!/usr/bin/env ruby

require 'rubygems'
require 'benchmark'
require 'openssl'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

repeats = 10

Benchmark.benchmark(" "*15 + Benchmark::Tms::CAPTION, 15, Benchmark::Tms::FMTSTR, "total:") do |bm|
  # Get pseudonym
  pt = bm.report("pseudonym:") do
    repeats.times do 
      pseudonym = RestClient.get('http://localhost:3001/pseudonym')
      fail "Failed to get pseudonym" if pseudonym.empty?
      @@user = Nymble::User.new(pseudonym, 'verify_key_n.pem')
      @@user.link_window = 1
      @@user.time_period = 2
    end
  end

  # Get credential
  ct = bm.report("credential:") do
    repeats.times do 
      credential = RestClient.post("http://localhost:3000/server/server_id", :pseudonym => @@user.pseudonym)
      fail "Failed to get credential" if credential.empty?
      @@user.add_credential('server_id', credential)
    end
  end
  
  # Check if blacklisted
  bt = bm.report("blacklisted:") do
    repeats.times do
      blacklist   = RestClient.get('http://localhost:3002/blacklist')
      cert        = RestClient.get('http://localhost:3002/cert')
      fail "Failed to get cert" if cert.empty?
      blacklisted = @@user.blacklisted?('server_id', blacklist, cert)
      fail "User is blacklisted" if blacklisted
    end
  end
  
  # Authenticate
  at = bm.report("authenticate:") do
    repeats.times do 
      ticket        = @@user.ticket('server_id')
      fail "Failed to get ticket" unless ticket
      authenticated = RestClient.post('http://localhost:3002/authenticate', :ticket => ticket)
      fail "Authentication failed: #{authenticated}" unless authenticated == "true"
    end
  end
  
  [pt + ct + bt + at]
end
