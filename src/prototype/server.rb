#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'rest_client'
require 'yaml'
require 'digest'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

Sinatra::Application.default_options[:port] = 3002

configure do
  $L = 288

  data = YAML.parse(RestClient.post('http://localhost:3001/server/', '')).transform

  SERVER_ID    = data[:server_id]
  SERVER_STATE = Nymble.server_initialize(SERVER_ID, data[:hmac_key_ns], Nymble.blacklist_unmarshall(data[:blacklist]))
  
  fail unless SERVER_STATE
end

helpers do
  def cur_link_window
    cur_time = Time.now.getutc
    366 * (cur_time.year - 1970) + cur_time.yday
  end
  
  def cur_time_period
    cur_time = Time.now.getutc
    (cur_time.hour * 60 + cur_time.min) / 5
  end
  
  def hexencode(value)
    ret = ""
    value.each_byte { |byte| ret << ("00" + byte.to_s(16))[-2..-1] }
    ret
  end

  def hexdecode(value)
    ret = ""
    value = value.dup
    ret << value.slice!(0, 2).to_i(16).chr while value.size > 0
    ret
  end
end

before do
  unless Nymble.server_blacklist_finalized(SERVER_STATE, cur_time_period)
    puts "Blacklist is not finalized, updating!"
    
    blacklist = Nymble.server_blacklist(SERVER_STATE)
    
    data = YAML.parse(RestClient.post("http://localhost:3001/server/#{hexencode(SERVER_ID)}/", { :blacklist => Nymble.blacklist_marshall(blacklist)})).transform

    new_blacklist = Nymble.blacklist_unmarshall(data[:new_blacklist])
    
    Nymble.server_update(SERVER_STATE, new_blacklist, nil)
    Nymble.server_blacklist_finalize(SERVER_STATE)
    
    puts "Unable to update blacklist" unless Nymble.server_blacklist_finalized(SERVER_STATE, cur_time_period)
  end
end

get '/nymble/' do
  blacklist = Nymble.server_blacklist(SERVER_STATE)

  { :server_id  => SERVER_ID,
    :blacklist  => Nymble.blacklist_marshall(blacklist) }.to_yaml
end

post '/nymble/' do
  ticket = Nymble.ticket_unmarshall(request[:ticket])
  
  return 'invalid ticket' unless Nymble.server_ticket_verify(SERVER_STATE, ticket, cur_link_window, cur_time_period)
  
  'authenticated'
end