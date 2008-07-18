#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'digest'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3000

configure do  
  PM_STATE = Nymble.pm_initialize(Nymble.digest('hmac_key_np'))
end

helpers do
  def cur_link_window
    cur_time = Time.now.getutc
    366 * (cur_time.year - 1970) + cur_time.yday
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

post '/pseudonym' do
  pseudonym, mac_np = Nymble.pm_pseudonym_create(PM_STATE, Nymble.digest(request.env['REMOTE_ADDR']), cur_link_window)
  
  hexencode(pseudonym + mac_np)
end