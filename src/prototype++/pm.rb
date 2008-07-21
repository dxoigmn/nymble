#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'json'
require File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3000

configure do  
  @@pm = Nymble::PseudonymManager.new(Nymble.digest('hmac_key_np'))
end

before do
  cur_time = Time.now.getutc
  
  @@pm.link_window = 366 * (cur_time.year - 1970) + cur_time.yday
  @@pm.time_period = (cur_time.hour * 60 + cur_time.min) / 1
end

post '/pseudonym' do
  pseudonym = @@pm.create_pseudonym(Nymble.digest(request.env['REMOTE_ADDR']))
  
  {
    :pseudonym => pseudonym.marshall
  }.to_json
end