#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

configure do
  PM_STATE = Nymble.pm_initialize(Nymble.digest('hmac_key_np'))
end

helpers do
  def cur_link_window
    cur_time = Time.now.getutc
    366 * (cur_time.year - 1970) + cur_time.yday
  end
end

get '/pseudonym' do
  pseudonym, mac_np = Nymble.pm_pseudonym_create(PM_STATE, Nymble.digest(request.env['REMOTE_ADDR']), cur_link_window)
  Nymble.pseudonym_marshall(pseudonym, mac_np)
end