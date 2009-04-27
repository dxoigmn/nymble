#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'webrick/https'
require 'sinatra'
require 'sinatra_patch'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

set :port, 3000
set :server, 'webrick'
set :handler_options, { :SSLEnable => true, :SSLVerifyClient => OpenSSL::SSL::VERIFY_NONE, :SSLCertName => [["C","US"], ["O","NYMBLE"], ["CN", "NM"]] }

configure do
  @@nm = Nymble::NymbleManager.new
  @@nm.write_verify_key_n('verify_key_n.pem')
end

before do
  cur_time = Time.now.getutc
  @@nm.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@nm.time_period = (cur_time.hour * 60 + cur_time.min) / 10
end

get '/mac_key_np' do
  @@nm.mac_key_np
end

post '/server' do
  @@nm.register_server(params['server_id'])
end

put '/server/:server_id' do
  @@nm.update_server(params['server_id'], params['complaint'])
end

post '/server/:server_id' do
  @@nm.create_credential(params['server_id'], params['pseudonym'])
end
