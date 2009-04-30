#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'webrick/https'
require 'sinatra'
require 'sinatra_patch'
require 'rest_client'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

set :port, 3002
set :server, 'webrick'
set :handler_options, {}
#set :handler_options, { :SSLEnable => true, :SSLVerifyClient => OpenSSL::SSL::VERIFY_NONE, :SSLCertName => [["C","US"], ["O","NYMBLE"], ["CN", "server_id"]] }

configure do
  data = RestClient.post('http://localhost:3000/server', :server_id => 'server_id')
  fail 'Failed to register server.' unless data
  @@server = Nymble::Server.new(data)
  @@server.link_window = 1
  @@server.time_period = 1
  
  # Manufacture 500 users by impersonating the PM :)
  mac_key_np = RestClient.get('http://localhost:3000/mac_key_np')
  @@pm = Nymble::PseudonymManager.new(mac_key_np)
  @@pm.link_window = 1
  
  500.times do |user_id|
    user = Nymble::User.new(@@pm.create_pseudonym(user_id.to_s), 'verify_key_n.pem')
    user.link_window = 1
    user.time_period = 1
    credential = RestClient.post("http://localhost:3000/server/server_id", :pseudonym => user.pseudonym)
    fail "Failed to get credential for user #{user_id}" if credential.empty?
    user.add_credential('server_id', credential)
    ticket = user.ticket('server_id')
    fail "Failed to get ticket for user #{user_id}" if ticket.empty?
    @@server.add_complaint(ticket, @@server.time_period)
  end
  
  RestClient.post('http://localhost:3000/transition', '')
  
  # Now complain
  complaint = @@server.complain!
  
  if complaint
    data = RestClient.put('http://localhost:3000/server/server_id', :complaint => complaint)
    fail 'Failed to complain!' unless data
    @@server.update!(data)
  end
end

get '/blacklist' do
  @@server.blacklist
end

get '/cert' do
  @@server.cert
end

post '/authenticate' do
  @@server.valid_ticket?(params['ticket']).to_s
end
