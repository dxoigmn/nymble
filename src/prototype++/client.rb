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

def acquire_blacklist(user)
  data = JSON.parse(RestClient.get('http://localhost:3002/nymble/'))
  blacklist  = Nymble::Blacklist.unmarshal(data['blacklist'])
  
  user.add_blacklist(blacklist)
end

def acquire_credential(user, server_id)
  data = JSON.parse(RestClient.get("http://localhost:3001/server/#{Nymble.hexencode(server_id)}/?pseudonym=#{user.pseudonym.marshal}"))
  credential = Nymble::Credential.unmarshal(data['credential'])
  
  user.add_credential(credential)
end

def authenticate
  fail 'Unable to acquire pseudonym'  unless (user = acquire_pseudonym)
  fail 'Unable to acquire blacklist'  unless (server_id = acquire_blacklist(user))
  fail 'Unable to acquire credential' unless acquire_credential(user, server_id)
  
  fail 'Unable to get ticket' unless (ticket = user.ticket(server_id))
  
  RestClient.post('http://localhost:3002/nymble/', { :ticket => ticket.marshal })
end

puts authenticate