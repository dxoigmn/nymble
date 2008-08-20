#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

def acquire_pseudonym
  data = JSON.parse(RestClient.post('http://localhost:3000/pseudonym', ''))
  pseudonym = Nymble::Pseudonym.unmarshal(data['pseudonym'])
  
  user = Nymble::User.new(pseudonym, File.expand_path('nm_pub.key'))
  cur_time = Time.now.getutc
  
  user.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  user.time_period = (cur_time.hour * 60 + cur_time.min) / 1
  
  user
end

def acquire_blacklist(server)
  data = JSON.parse(RestClient.get("#{server}/nymble/"))
  Nymble::Blacklist.unmarshal(data['blacklist'])
end

def acquire_credential(server, pseudonym)
  data = JSON.parse(RestClient.get("http://localhost:3001/server/#{Nymble.hexencode(Nymble.digest(server))}/?pseudonym=#{pseudonym.marshal}"))
  Nymble::Credential.unmarshal(data['credential'])
end

def authenticate
  server = 'http://localhost:3002'
  server_id = Nymble.digest(server)
  
  user = acquire_pseudonym
  user.add_blacklist(server_id, acquire_blacklist(server))
  user.add_credential(server_id, acquire_credential(server, user.pseudonym))
  
  ticket = user.ticket(server_id)
  
  fail 'Unable to get ticket' unless ticket
  
  RestClient.post('http://localhost:3002/nymble/', { :ticket => ticket.marshal })
end

puts authenticate