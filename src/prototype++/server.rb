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
set :handler_options, { :SSLEnable => true, :SSLVerifyClient => OpenSSL::SSL::VERIFY_NONE, :SSLCertName => [["C","US"], ["O","NYMBLE"], ["CN", "server_id"]] }

configure do
  data = RestClient.post('https://localhost:3000/server', :server_id => 'server_id')
  fail 'Failed to register server.' unless data
  @@server = Nymble::Server.new(data)
end

before do
  cur_time = Time.now.getutc
  @@server.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@server.time_period = (cur_time.hour * 60 + cur_time.min) / 10
  
  complaint = @@server.complain!
  
  unless complaint
    data = RestClient.put('https://localhost:3000/server/server_id', :complaint => complaint)
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
