#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

# Get pseudonym
pseudonym = RestClient.get('https://localhost:3001/pseudonym')
@@user = Nymble::User.new(pseudonym, 'verify_key_n.pem')

# Set time
cur_time = Time.now.getutc
@@user.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
@@user.time_period = (cur_time.hour * 60 + cur_time.min) / 10

# Get credential
credential = RestClient.post("https://localhost:3000/server/server_id", :pseudonym => @@user.pseudonym)
@@user.add_credential('server_id', credential)

# Check if blacklisted
blacklist   = RestClient.get('https://localhost:3002/blacklist')
cert        = RestClient.get('https://localhost:3002/cert')
blacklisted = @@user.blacklisted?('server_id', blacklist, cert)

puts "Blacklisted: #{blacklisted}"

# Authenticate
ticket        = @@user.ticket('server_id')
authenticated = RestClient.post('https://localhost:3002/authenticate', :ticket => ticket)

puts "Authenticated: #{authenticated}"
