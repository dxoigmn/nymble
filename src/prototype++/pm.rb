#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'webrick/https'
require 'sinatra'
require 'sinatra_patch'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

set :port, 3001
set :server, 'webrick'
set :handler_options, {}
#set :handler_options, { :SSLEnable => true, :SSLVerifyClient => OpenSSL::SSL::VERIFY_NONE, :SSLCertName => [["C","US"], ["O","NYMBLE"], ["CN", "PM"]] }

configure do
  mac_key_np = RestClient.get('http://localhost:3000/mac_key_np')
  @@pm = Nymble::PseudonymManager.new(mac_key_np)
  @@pm.link_window = 1
end

get '/pseudonym' do
  @@pm.create_pseudonym(Nymble.digest(request.env['REMOTE_ADDR']))
end
