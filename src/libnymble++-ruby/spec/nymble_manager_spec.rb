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
  
  it 'should generate verify_key_n' do
    @@nm.should.respond_to?(:write_verify_key_n)
    @@nm.should.write_verify_key_n(File.expand_path(File.join(File.dirname(__FILE__), '..', 'verify_key_n.pem')))
  end
  
  it 'should verify pseudonyms' do
    @@pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    @@pm.link_window = 10
    pseudonym = @@pm.create_pseudonym('user_id')
    
    @@nm.should.respond_to?(:valid_pseudonym?)
    @@nm.should.valid_pseudonym?(pseudonym)
  end
  
  it 'should not verify invalid pseudonyms' do
    pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    pm.link_window = 11
    pseudonym = pm.create_pseudonym('user_id')
    
    @@nm.should.not.valid_pseudonym?(pseudonym)
  end
  
  it 'should register servers' do
    @@nm.should.respond_to?(:register_server)
    @@nm.register_server('server_id').should.not.be(nil)
  end
  
  it 'should create credentials' do
    pseudonym = @@pm.create_pseudonym('user_id')
    
    @@nm.should.respond_to?(:create_credential)
    @@nm.create_credential('server_id', pseudonym).should.not.be(nil)
    @@nm.create_credential('', pseudonym).should.be(nil)
  end
  
  it 'should verify tickets' do
    @@user = Nymble::User.new(@@nm.create_credential('server_id', @@pm.create_pseudonym('user_id')))
    @@user.link_window = 10
    @@user.time_period = 5
    
    @@nm.should.respond_to?(:valid_ticket?)
    @@nm.should.valid_ticket?(@@user.ticket)
  end
end