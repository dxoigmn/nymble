#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'rest_client'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3002

configure do
  data = JSON.parse(RestClient.post('http://localhost:3001/server/', ''))
  
  server_id = Nymble.hexdecode(data['server_id'])
  hmac_key_ns = Nymble.hexdecode(data['hmac_key_ns'])
  blacklist = Nymble::Blacklist.unmarshall(data['blacklist'])
  
  @@server = Nymble::Server.new(server_id, hmac_key_ns, blacklist)
end

before do
  cur_time = Time.now.getutc

  @@server.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@server.time_period = cur_time.hour * 60 + cur_time.min
end

get '/nymble/' do
  blacklist = @@server.blacklist

  { 
    :blacklist  => blacklist.marshall
  }.to_json
end

post '/nymble/' do
  ticket = Nymble::Ticket.unmarshall(request[:ticket]) if request[:ticket]
  
  if @@server.valid_ticket?(ticket)
    'you are blacklisted'
  else
    'you are authenticated'
  end
end
