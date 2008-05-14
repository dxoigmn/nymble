#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

configure do
  NM_STATE  = Nymble.nm_initialize(Nymble.digest('hmac_key_np'))
  $L        = 288
  
  File.open('nm.pub', 'w') { |f| f << Nymble.nm_verify_key(NM_STATE) }
end

helpers do
  def cur_link_window
    cur_time = Time.now.getutc
    366 * (cur_time.year - 1970) + cur_time.yday
  end
  
  def cur_time_period
    cur_time = Time.now.getutc
    (cur_time.hour * 60 + cur_time.min) / 5
  end
end

post '/register' do
  server_id = params[:server_id]
  
  fail 'server_id already registered' if Nymble.nm_entry_exists(NM_STATE, server_id)
  
  hmac_key_ns = Nymble.nm_entry_add(NM_STATE, server_id)
  Nymble.nm_entry_update(NM_STATE, server_id, cur_time_period)
  blacklist   = Nymble.blacklist_marshall(Nymble.nm_blacklist_create(NM_STATE, server_id, cur_time_period, cur_link_window))
  
  "hmac_key_ns=#{CGI.escape(hmac_key_ns)}&blacklist=#{CGI.escape(blacklist)}"
end

post '/credential' do
  server_id         = params[:server_id]
  pseudonym, mac_np = Nymble.pseudonym_unmarshall(params[:pseudonym])
  
  fail 'invalid server_id' unless Nymble.nm_entry_exists(NM_STATE, server_id)
  fail 'invalid pseudonym' unless Nymble.nm_pseudonym_verify(NM_STATE, pseudonym, cur_link_window, mac_np)

  Nymble.nm_credential_marshall(Nymble.nm_credential_create(NM_STATE, pseudonym, server_id, cur_link_window))
end

post '/update' do
  server_id   = params[:server_id]
  blacklist_cert = Nymble.blacklist_cert_unmarshall(params[:blacklist_cert])

  fail 'invalid server_id'       unless Nymble.nm_entry_exists(NM_STATE, server_id)    
  fail 'invalid blacklist cert'  unless Nymble.nm_blacklist_cert_verify(NM_STATE, blacklist, server_id, cur_link_window)
end

post '/complain' do
  server_id   = params[:server_id]
  blacklist   = Nymble.blacklist_unmarshall(params[:blacklist])
  complaints  = [Nymble.ticket_unmarshall(params[:complaints])] if params[:complaints]

  fail 'invalid server_id' unless Nymble.nm_entry_exists(NM_STATE, server_id)    
  fail 'invalid blacklist' unless Nymble.nm_blacklist_verify(NM_STATE, blacklist, server_id, cur_link_window)

  if params[:complaints]
    linking_tokens = Nymble.linking_token_marshall(Nymble.nm_tokens_create(NM_STATE, server_id, blacklist, complaints, cur_time_period, cur_link_window)[0])
  end
  
  new_blacklist = Nymble.blacklist_marshall(Nymble.nm_blacklist_update(NM_STATE, blacklist, complaints, cur_time_period, cur_link_window))
  
  Nymble.nm_entry_update(NM_STATE, server_id, cur_time_period)
  
  response  = "blacklist=#{CGI.escape(new_blacklist)}"
  response << "&linking_tokens=#{CGI.escape(linking_tokens)}" if params[:complaints]
  response
end