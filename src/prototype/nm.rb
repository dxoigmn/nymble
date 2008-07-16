#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'yaml'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

Sinatra::Application.default_options[:port] = 3001

configure do
  NM_STATE  = Nymble.nm_initialize(Nymble.digest('hmac_key_np'))
  $L        = 1440
  
  File.open('nm.pub', 'w') { |f| f << Nymble.nm_verify_key(NM_STATE) }
end

helpers do
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
end

post '/server/' do
  server_id   = Nymble.random_bytes(32)
  hmac_key_ns = Nymble.nm_entry_add(NM_STATE, server_id)

  Nymble.nm_entry_update(NM_STATE, server_id, cur_time_period)

  blacklist = Nymble.blacklist_marshall(Nymble.nm_blacklist_create(NM_STATE, server_id, cur_time_period, cur_link_window))

  { :server_id    => server_id,
    :hmac_key_ns  => hmac_key_ns,
    :blacklist    => blacklist }.to_yaml
end

get '/server/:server_id/' do
  server_id = hexdecode(params[:server_id])
  pseudonym = hexdecode(params[:pseudonym])[0..31]
  mac_np    = hexdecode(params[:pseudonym])[32..63]
  
  fail 'invalid server_id' unless Nymble.nm_entry_exists(NM_STATE, server_id)
  fail 'invalid pseudonym' unless Nymble.nm_pseudonym_verify(NM_STATE, pseudonym, cur_link_window, mac_np)

  credential = Nymble.nm_credential_marshall(Nymble.nm_credential_create(NM_STATE, pseudonym, server_id, cur_link_window))
  
  { :credential => credential }.to_yaml
end

post '/server/:server_id/' do
  server_id   = hexdecode(params[:server_id])
  blacklist   = Nymble.blacklist_unmarshall(params[:blacklist])
  complaints  = [Nymble.ticket_unmarshall(params[:complaints])] if params[:complaints]

  fail 'invalid server_id' unless Nymble.nm_entry_exists(NM_STATE, server_id)    
  fail 'invalid blacklist' unless Nymble.nm_blacklist_verify(NM_STATE, blacklist, server_id, cur_link_window)

  if params[:complaints]
    linking_tokens = Nymble.linking_token_marshall(Nymble.nm_tokens_create(NM_STATE, server_id, blacklist, complaints, cur_time_period, cur_link_window)[0])
  end
  
  new_blacklist = Nymble.blacklist_marshall(Nymble.nm_blacklist_update(NM_STATE, blacklist, complaints, cur_time_period, cur_link_window))
  
  Nymble.nm_entry_update(NM_STATE, server_id, cur_time_period)
  
  { :new_blacklist  => new_blacklist,
    :linking_tokens => linking_tokens }.to_yaml
end