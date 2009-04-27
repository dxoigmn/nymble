#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'rest_client'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

set :port, 3002

configure do
  data = RestClient.post('http://localhost:3000/server', :server_id => 'server_id')
  fail 'Failed to register server.' unless data
  @@server = Nymble::Server.new(data)
end

before do
  cur_time = Time.now.getutc
  @@server.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@server.time_period = (cur_time.hour * 60 + cur_time.min) / 10
  
  complaint = @@server.complain!
  
  unless complaint
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
