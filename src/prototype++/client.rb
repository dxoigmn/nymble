#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

def acquire_pseudonym
  data = JSON.parse(RestClient.post('http://localhost:3000/pseudonym', ''))
  pseudonym = Nymble::Pseudonym.unmarshall(data['pseudonym'])
  verify_key = File.read('nm.pub')

  Nymble::User.new(pseudonym, verify_key)
end

def acquire_blacklist_and_credential(user)
  data = JSON.parse(RestClient.get('http://localhost:3002/nymble/'))
  
  server_id  = Nymble.hexdecode(data['server_id'])
  blacklist  = Nymble::Blacklist.unmarshall(data['blacklist'])
  
  data = JSON.parse(RestClient.get("http://localhost:3001/server/#{Nymble.hexencode(server_id)}/?pseudonym=#{user.pseudonym.marshall}"))
  
  credential = Nymble::Credential.unmarshall(data['credential'])
  
  return nil unless user.add_server(server_id, blacklist, credential)
  
  server_id
end

def acquire_credential(user, server_id)
  
  
  entry = user.find_server(server_id)
  entry.credential = credential
end

def authenticate
  fail 'Unable to acquire pseudonym'                unless (user = acquire_pseudonym)
  fail 'Unable to acquire blacklist and credential' unless (server_id = acquire_blacklist_and_credential(user))
  
  ticket = user.ticket(server_id)
  
  fail 'Unable to get ticket' unless ticket
  
  RestClient.post('http://localhost:3002/nymble/', { :ticket => ticket.marshall })
end

puts authenticate