#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3001

configure do
  @@nm = Nymble::NymbleManager.new(Nymble.digest('hmac_key_np'))

  File.open('nm.pub', 'w') { |f| f << @@nm.verify_key }
end

before do
  cur_time = Time.now.getutc
  
  @@nm.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@nm.time_period = (cur_time.hour * 60 + cur_time.min) / 1
end

post '/server/' do
  server_id   = Nymble.random_bytes(Nymble::DIGEST_SIZE)
  hmac_key_ns = @@nm.add_server(server_id)
  blacklist   = @@nm.create_blacklist(server_id)

  { 
    :server_id    => Nymble.hexencode(server_id),
    :hmac_key_ns  => Nymble.hexencode(hmac_key_ns),
    :blacklist    => blacklist.marshall
  }.to_json
end

get '/server/:server_id/' do
  server_id = Nymble.hexdecode(params[:server_id])
  pseudonym = Nymble::Pseudonym.unmarshall(params[:pseudonym])
  
  fail 'invalid pseudonym' unless @@nm.valid_pseudonym?(pseudonym)
  
  credential = @@nm.create_credential(server_id, pseudonym, 1440)
  
  fail 'failed to create credential' unless credential
  
  { 
    :credential => credential.marshall 
  }.to_json
end

post '/server/:server_id/' do
  server_id   = Nymble.hexdecode(params[:server_id])
  blacklist   = Nymble::Blacklist.unmarshall(params[:blacklist])
  complaints  = param[:complaints].map { |ticket| Nymble::Ticket.unmarshall(ticket) } if params[:complaints]
  
  fail 'invalid blacklist' unless @@nm.valid_blacklist?(server_id, blacklist)
  
  linking_tokens = @@nm.create_tokens(server_id, blacklist, complaints)
  new_blacklist = @@nm.update_blacklist(server_id, blacklist, complaints)
  
  { 
    :new_blacklist  => new_blacklist.marshall,
    :linking_tokens => linking_tokens.map { |linking_token| linking_token.marshall }
  }.to_json
end

# linking_token
