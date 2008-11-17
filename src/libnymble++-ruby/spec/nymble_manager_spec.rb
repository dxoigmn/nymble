#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Nymble Manager' do
  before(:all) do
    @@nm = Nymble::NymbleManager.new
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:NymbleManager)
  end
  
  it 'should be createable' do
    @@nm.should.not.be.nil
  end
  
  it 'should manage the link window' do
    @@nm.should.respond_to?(:link_window=)
    @@nm.link_window = 10
    @@nm.should.respond_to?(:link_window)
    @@nm.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @@nm.should.respond_to?(:time_period=)
    @@nm.time_period = 5
    @@nm.should.respond_to?(:time_period)
    @@nm.time_period.should.equal(5)
  end
  
  it 'should generate mac_key_np' do
    @@nm.should.respond_to?(:mac_key_np)
    @@nm.mac_key_np.should.not.be.nil
  end
  
  it 'should verify pseudonyms' do
    pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    pm.link_window = 10
    pseudonym = pm.create_pseudonym('user_id')
    
    @@nm.should.respond_to?(:valid_pseudonym?)
    @@nm.valid_pseudonym?(pseudonym).should.be(true)
  end
  
  it 'should not verify invalid pseudonyms' do
    pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    pm.link_window = 11
    pseudonym = pm.create_pseudonym('user_id')
    
    @@nm.valid_pseudonym?(pseudonym).should.be(false)
  end
  
  it 'should register servers' do
    @@nm.should.respond_to?(:register_server)
    @@nm.register_server('server_id').should.not.be(nil)
  end
  
  it 'should create credentials' do
    pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    pm.link_window = 10
    pseudonym = pm.create_pseudonym('user_id')
    
    @@nm.should.respond_to?(:create_credential)
    @@nm.create_credential('server_id', pseudonym).should.not.be(nil)
    @@nm.create_credential('', pseudonym).should.be(nil)
  end
  
=begin  
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
=end
end