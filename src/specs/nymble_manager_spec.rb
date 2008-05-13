#!/usr/bin/env ruby
require File.join(File.dirname(__FILE__), '..', 'nymble_manager')
require 'ramaze/spec/helper'

module Nymble
  class << self
    def random_bytes(size)
      ('rand' * size)[0...size]
    end
  end
end

$cur_time_period  = 1
$cur_link_window  = 1
$L                = 10
$hmac_key_np      = Nymble.hash('hmac_key_np')
$user_id          = Nymble.hash('user_id')
$server_id        = Nymble.hash('server_id')

$nm_state = Nymble.nm_initialize($hmac_key_np)
$pm_state = Nymble.pm_initialize($hmac_key_np)



describe 'NymbleManager' do
  behaves_like 'http'
  ramaze
  
  it 'should have register url' do
    page = post('/register', { :server_id => $server_id })
    page.status.should == 200
    
    body = JSON.parse(page.body)

    body.should.be.kind_of(Hash)
    body.should.include('hmac_key_ns')
    body['hmac_key_ns'].should.not.be.nil
    body.should.include('blacklist')
    body['blacklist'].should.not.be.nil
    
    $blacklist            = body['blacklist']
    $psuedonym, $mac_np   = Nymble.pm_pseudonym_create($pm_state, $user_id, $cur_link_window)
    $credential           = Nymble.nm_credential_create($nm_state, $psuedonym, $server_id, $cur_link_window)
    $complaints           = $credential.tickets
    
    Nymble.nm_entry_update($nm_state, $server_id, $cur_time_period)
  end

  it 'should have credential url' do
    page = post('/credential', { :server_id => $server_id, 
                                 :pseudonym => $psuedonym, 
                                 :mac_np    => $mac_np})

    page.status.should == 200
    
    body = JSON.parse(page.body)

    body.should.not.be.nil
  end

  it 'should have complain url' do
    page = get('/complain', { :server_id  => $server_id,
                              :complaints => $complaints.to_json,
                              :blacklist  => $blacklist.to_json })

    page.status.should == 200
    body = JSON.parse(page.body)
    
    body.should.be.kind_of(Hash)
    body.should.include 'blacklist'
    body['blacklist'].should.not.be.nil
    body.should.include 'linking_tokens'
    body['linking_tokens'].should.not.be.nil
  end
end
