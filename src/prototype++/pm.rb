#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'json'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

set :port, 3001

configure do
  mac_key_np = RestClient.get('http://localhost:3000/mac_key_np')
  @@pm = Nymble::PseudonymManager.new(mac_key_np)
end

before do
  cur_time = Time.now.getutc
  @@pm.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@pm.time_period = (cur_time.hour * 60 + cur_time.min) / 10
end

get '/pseudonym' do
  @@pm.create_pseudonym(Nymble.digest(request.env['REMOTE_ADDR']))
end