#!/usr/bin/env ruby
require File.join(File.dirname(__FILE__), '..', 'pseudonym_manager')
require 'ramaze/spec/helper'

$cur_time_period  = 1
$cur_link_window  = 1
$L                = 10
$hmac_key_np      = Nymble.hash('hmac_key_np')

$nm_state = Nymble.nm_initialize($hmac_key_np)
$pm_state = Nymble.pm_initialize($hmac_key_np)

describe 'PseudonymManager' do
  behaves_like 'http'
  ramaze
  
  it 'should have a pseudonym url' do
    page = get('/pseudonym')
    page.status.should.equal  200

    body = JSON.parse(page.body)

    body.should.be.kind_of Hash
    body.should.include 'pseudonym'
    body['pseudonym'].should.not.be.empty
    body.should.include 'mac_np'
    body['mac_np'].should.not.be.empty
  end
end
