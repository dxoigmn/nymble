#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'net/http'
require 'cgi'

require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

configure do
  $L = 288
  
  SERVER_ID = Nymble.digest('localhost:3003')
  
  response = Net::HTTP.post_form(URI.parse('http://localhost:3001/register'), { :server_id => SERVER_ID })
  
  fail(response.body) unless response.kind_of?(Net::HTTPOK) || response.kind_of?(String)

  data        = CGI.parse(response.body)
  hmac_key_ns = data['hmac_key_ns'].first
  blacklist   = Nymble.blacklist_unmarshall(data['blacklist'].first)

  SERVER_STATE = Nymble.server_initialize(SERVER_ID, hmac_key_ns, blacklist)
  
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
end

get '/blacklist' do
  unless Nymble.server_blacklist_finalized(SERVER_STATE)
    blacklist = Nymble.server_blacklist(SERVER_STATE)
    
    response = Net::HTTP.post_form(URI.parse('http://localhost:3001/complain'), { :server_id => SERVER_ID, :blacklist => Nymble.blacklist_marshall(blacklist) })
    
    fail(response.body) unless response.kind_of?(Net::HTTPOK) || response.kind_of?(String)
        
    data = CGI.parse(response.body)
    blacklist = data['blacklist']
    
    Nymble.server_update(SERVER_STATE, blacklist, nil)
    Nymble.server_blacklist_finalize(SERVER_STATE)
  end

  Nymble.blacklist_marshall(Nymble.server_blacklist(SERVER_STATE))
end

post '/authenticate' do
  ticket = Nymble.ticket_unmarshall(request[:ticket])
  
  fail 'invalid ticket' unless Nymble.server_ticket_verify(SERVER_STATE, ticket, cur_link_window, cur_time_period)
  
  'authenticated'
end