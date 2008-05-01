#!/usr/bin/env ruby
require File.join(File.dirname(__FILE__), '..', 'server')
require 'ramaze/spec/helper'

$cur_time_period  = 1
$cur_link_window  = 1
$L                = 10
$hmac_key_np      = Nymble.hash('hmac_key_np')
$server_id        = Nymble.hash('server_id')
$user_id          = Nymble.hash('user_id')

$nm_state = Nymble.nm_initialize($hmac_key_np)
$pm_state = Nymble.pm_initialize($hmac_key_np)

$server_state = Nymble.server_initialize($server_id, 
                                         Nymble.nm_entry_add($nm_state, $server_id), 
                                         Nymble.nm_blacklist_create($nm_state, $server_id, $cur_time_period, $cur_link_window))

describe 'Server' do
  behaves_like 'http'
  ramaze
  
  before do
    pseudonym   = Nymble.pm_pseudonym_create($pm_state, $user_id, $cur_link_window)
    credential  = Nymble.nm_credential_create($nm_state, pseudonym, $server_id, $cur_link_window)
    @ticket     = credential.tickets.find { |ticket|  ticket.server_id == $server_id &&
                                                      ticket.time_period == $cur_time_period &&
                                                      ticket.link_window == $cur_link_window }
  end
  
  it 'should have a blacklist url' do
    page = get('/blacklist')
    page.status.should.equal  200

    body = JSON.parse(page.body)

    body.should.not.be.nil
  end
  
  it 'should have authenticate url' do
    page = post('/authenticate', { :ticket => @ticket.to_json })
    page.status.should.equal  200
  end
end
