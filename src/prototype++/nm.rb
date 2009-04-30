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
set :handler_options, {}
#set :handler_options, { :SSLEnable => true, :SSLVerifyClient => OpenSSL::SSL::VERIFY_NONE, :SSLCertName => [["C","US"], ["O","NYMBLE"], ["CN", "NM"]] }

configure do
  @@nm = Nymble::NymbleManager.new
  @@nm.write_verify_key_n('verify_key_n.pem')
  @@nm.link_window = 1
  @@nm.time_period = 1
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

post '/transition' do
  @@nm.time_period += 1
end