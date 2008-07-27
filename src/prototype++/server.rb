#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'rest_client'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3002

configure do
  @@server = Nymble::Server.new(Nymble.digest('http://localhost:3001'))
end

before do
  cur_time = Time.now.getutc
  
  @@server.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@server.time_period = (cur_time.hour * 60 + cur_time.min) / 1
  
  unless @@server.finalized?
    if @@server.blacklist
      # Time period change
      puts "Time period changed!"
      data = { :blacklist => @@server.blacklist.marshall, :complaints => @@complaints.map { |complaint| complaint.marshall } }
      data = JSON.parse(RestClient.put("http://localhost:3001/server/#{Nymble.hexencode(@@server.server_id)}/", data))
      
      blacklist = Nymble::Blacklist::unmarshall(data['blacklist'])
      linking_tokens = data['linking_tokens'].map { |linking_token| Nymble::LinkingToken.unmarshall(linking_token) } unless data['linking_tokens'].empty?
    else
      # Link window change
      puts "Link window changed!"
      data = { :server_id => Nymble.hexencode(@@server.server_id) }
      data = JSON.parse(RestClient.post('http://localhost:3001/server/', data))
      
      hmac_key_ns = Nymble.hexdecode(data['hmac_key_ns'])
      blacklist = Nymble::Blacklist.unmarshall(data['blacklist'])
    end
    
    if (@@server.blacklist = blacklist)
      @@server.hmac_key_ns = hmac_key_ns if hmac_key_ns
      @@server.add_linking_tokens(linking_tokens) if linking_tokens
      @@complaints = []
    else
      puts 'Unable to verify blacklist!'
      fail 'Unable to verify blacklist!'
    end
  end
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
    'you are authenticated'
  else
    'you are blacklisted'
  end
end
