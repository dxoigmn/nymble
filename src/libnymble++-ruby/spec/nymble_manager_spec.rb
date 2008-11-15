#!/usr/bin/env ruby

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Nymble Manager' do
  before(:each) do
    @cur_link_window = 10
    @cur_time_period = 2
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @sign_key_n = File.expand_path('sign_n.key')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @pseudonym = @pm.create_pseudonym(@user_id)
    @nm = Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n)
    @server_id = Nymble.digest('server_id')
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:NymbleManager)
  end
  
  it 'should be created with a valid hmac key' do
    Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n).should.not.be.nil
  end
  
  it 'should manage the link window' do
    @nm.should.respond_to?(:link_window=)
    @nm.link_window = @cur_link_window
    @nm.should.respond_to?(:link_window)
    @nm.link_window.should.equal(@cur_link_window)
  end
  
  it 'should manage the time period' do
    @nm.should.respond_to?(:time_period=)
    @nm.time_period = @cur_time_period
    @nm.should.respond_to?(:time_period)
    @nm.time_period.should.equal(@cur_time_period)
  end
  
  it 'should register servers with valid ids' do
    @nm.add_server(@server_id).should.not.be.nil
  end
  
  it 'should not register servers with invalid ids' do
    should.raise(ArgumentError) { @nm.add_server() }
    should.raise(ArgumentError) { @nm.add_server(@server_id * 2) }
    should.raise(ArgumentError) { @nm.add_server(@server_id.slice(0, @server_id.size / 2)) }
  end
  
  it 'should verify valid pseudonyms' do
    pseudonym = @pm.create_pseudonym(@user_id)
    @nm.should.valid_pseudonym?(pseudonym)
  end
  
  it 'should not verify invalid pseudonyms' do
    pseudonym = @pm.create_pseudonym(@user_id)
    should.raise(ArgumentError) { @nm.valid_pseudonym?() }
    should.raise(ArgumentError) { @nm.valid_pseudonym?(@nm) }
  end
  
  it 'should create blacklists' do
    @nm.add_server(@server_id)
    
    @nm.create_blacklist(@server_id).should.not.be.nil
  end
  
  it 'should verify blacklists' do
    @nm.add_server(@server_id)
    blacklist = @nm.create_blacklist(@server_id)
    
    @nm.should.valid_blacklist?(@server_id, blacklist)
  end
  
  it 'should update blacklists' do
    @nm.add_server(@server_id)
    blacklist = @nm.create_blacklist(@server_id)
    tickets = []
    
    @nm.update_blacklist(@server_id, blacklist, tickets).should.not.be.nil
  end
  
  it 'should create tokens' do
    @nm.add_server(@server_id)
    blacklist = @nm.create_blacklist(@server_id)
    tickets = []
    
    @nm.create_tokens(@server_id, blacklist, tickets).should.be.empty?
  end
  
  it 'should create credentials' do
    @nm.add_server(@server_id)
    pseudonym = @pm.create_pseudonym(@user_id)
    time_periods = 1440
    
    @nm.create_credential(@server_id, pseudonym, time_periods).should.not.be.nil
  end
end