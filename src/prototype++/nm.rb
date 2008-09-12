#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3001

configure do
  @@nm = Nymble::NymbleManager.new(Nymble.digest('hmac_key_np'), File.expand_path('nm_priv.key'))
end

before do
  cur_time = Time.now.getutc
  
  @@nm.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@nm.time_period = (cur_time.hour * 60 + cur_time.min) / 1
end

post '/server/' do
  server_id = Nymble.hexdecode(params[:server_id])
  hmac_key_ns = @@nm.add_server(server_id)
  blacklist = @@nm.create_blacklist(server_id)

  { 
    :hmac_key_ns  => Nymble.hexencode(hmac_key_ns),
    :blacklist    => blacklist.marshal
  }.to_json
end

get '/server/:server_id/' do
  server_id = Nymble.hexdecode(params[:server_id])
  pseudonym = Nymble::Pseudonym.unmarshal(params[:pseudonym])
  
  fail 'invalid pseudonym' unless @@nm.valid_pseudonym?(pseudonym)
  
  credential = @@nm.create_credential(server_id, pseudonym, 1440)
  
  fail 'failed to create credential' unless credential
  
  { 
    :credential => credential.marshal 
  }.to_json
end

put '/server/:server_id/' do
  server_id = Nymble.hexdecode(params[:server_id])
  blacklist = Nymble::Blacklist.unmarshal(params[:blacklist])
  complaints = params[:complaints] unless params[:complaints].empty?
  
  if complaints.kind_of?(Array)
    complaints = complaints.map { |complaint| Nymble::Ticket::unmarshal(complaint) }
  elsif complaints.kind_of?(String)
    complaints = [ Nymble::Ticket::unmarshal(complaints) ]
  end
  
  unless @@nm.valid_blacklist?(server_id, blacklist)
    puts 'invalid blacklist'
    fail 'invalid blacklist'
  end
  
  tokens = @@nm.create_tokens(server_id, blacklist, complaints || [])
  blacklist = @@nm.update_blacklist(server_id, blacklist, complaints || [])
  
  { 
    :blacklist  => blacklist.marshal,
    :tokens => tokens.map { |token| token.marshal }
  }.to_json
end
