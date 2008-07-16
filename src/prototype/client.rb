#!/usr/bin/env ruby

require 'rubygems'
require 'openssl'
require 'yaml'
require 'rest_client'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

$L = 1440

def cur_link_window
  cur_time = Time.now.getutc
  366 * (cur_time.year - 1970) + cur_time.yday
end

def cur_time_period
  cur_time = Time.now.getutc
  (cur_time.hour * 60 + cur_time.min) / 1
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

def acquire_pseudonym
  @pseudonym = RestClient.post('http://localhost:3000/pseudonym', '')

  pseudonym   = hexdecode(@pseudonym)[0..31]
  mac_np      = hexdecode(@pseudonym)[32..63]
  verify_key  = File.read('nm.pub')

  @user_state = Nymble.user_initialize(pseudonym, mac_np, verify_key)
end

def acquire_blacklist
  response = YAML.parse(RestClient.get('http://localhost:3002/nymble/')).transform
  
  @server_id  = response[:server_id]
  @blacklist  = Nymble.blacklist_unmarshall(response[:blacklist])
end

def acquire_credential
  response = YAML.parse(RestClient.get("http://localhost:3001/server/#{hexencode(@server_id)}/?pseudonym=#{@pseudonym}")).transform
  
  @credential = Nymble.user_credential_unmarshall(response[:credential])

  Nymble.user_entry_initialize(@user_state, @server_id, @credential)
  Nymble.user_blacklist_update(@user_state, @server_id, @blacklist, cur_link_window, cur_time_period)
end

def authenticate
  fail "Unable to acquire pseudonym"  unless acquire_pseudonym
  fail "Unable to acquire blacklist"  unless acquire_blacklist
  fail "Unable to acquire credential" unless acquire_credential
  
  RestClient.post('http://localhost:3002/nymble/', { :ticket => Nymble.ticket_marshall(Nymble.user_credential_get(@user_state, @server_id, cur_time_period)) })
end

puts authenticate