#!/usr/bin/env ruby
%w| rubygems ramaze json |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), 'libnymble_safe')

class MainController < Ramaze::Controller
  def register
    server_id = request[:server_id]
    
    respond('server_id already registered', 400) if Nymble.nm_entry_exists($nm_state, server_id)
    
    hmac_key_ns = Nymble.nm_entry_add($nm_state, server_id)
    blacklist   = Nymble.nm_blacklist_create($nm_state, server_id, $cur_time_period, $cur_link_window)
    
    { :hmac_key_ns  => hmac_key_ns,
      :blacklist    => blacklist }.to_json
  end
    
  def credential
    server_id = request[:server_id]
    pseudonym = request[:pseudonym]
    mac_np    = request[:mac_np]
    
    respond('invalid server_id', 400) unless Nymble.nm_entry_exists($nm_state, server_id)
    respond('invalid pseudonym', 400) unless Nymble.nm_pseudonym_verify($nm_state, pseudonym, $cur_link_window, mac_np)

    Nymble.nm_credential_create($nm_state, pseudonym, server_id, $cur_link_window).to_json
  end
  
  def complain
    server_id   = request[:server_id]
    blacklist   = JSON.parse(request[:blacklist])
    complaints  = JSON.parse(request[:complaints])  if request[:complaints]

    respond('invalid server_id', 400) unless Nymble.nm_entry_exists($nm_state, server_id)
    respond('invalid blacklist', 400) unless Nymble.nm_blacklist_verify($nm_state, blacklist, server_id, $cur_link_window)

    linking_tokens  = Nymble.nm_tokens_create($nm_state, server_id, blacklist, complaints, $cur_time_period, $cur_link_window)
    new_blacklist   = Nymble.nm_blacklist_update($nm_state, blacklist, complaints, $cur_time_period, $cur_link_window)
    
    Nymble.nm_entry_update($nm_state, server_id, $cur_time_period)
    
    { :blacklist      => new_blacklist,
      :linking_tokens => linking_tokens }.to_json
  end
end

if __FILE__ == $0
  $nm_state         = Nymble.nm_initialize(Nymble.hash('hmac_key_np'))
  $cur_time_period  = 1
  $cur_link_window  = 0
  $L                = 10
  
  File.open('nm.pub', 'w') { |f| f << Nymble.nm_verify_key($nm_state) }
  
  Ramaze.start(:adapter => :mongrel, :port => 3001)
end