#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'User' do
  before(:all) do
    @@nm = Nymble::NymbleManager.new
    @@nm.link_window = 10
    @@nm.time_period = 5
    @@nm.write_verify_key_n('verify_key_n.pem')
    
    @@pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    @@pm.link_window = 10
    @@pm.time_period = 5
    
    @@server = Nymble::Server.new(@@nm.register_server('server_id'))
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:User)
  end
  
  it 'should be created with a valid pseudonym and verify key' do
    @@user = Nymble::User.new(@@pm.create_pseudonym('user_id'), 'verify_key_n.pem')
    @@user.should.not.be.nil
  end
  
  it 'should manage the link window' do
    @@user.should.respond_to?(:link_window=)
    @@user.link_window = 10
    @@user.should.respond_to?(:link_window)
    @@user.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @@user.should.respond_to?(:time_period=)
    @@user.time_period = 5
    @@user.should.respond_to?(:time_period)
    @@user.time_period.should.equal(5)
  end
  
  it 'should manage credentials' do
    cred = @@nm.create_credential('server_id', @@user.pseudonym)
    @@user.should.add_credential('server_id', cred)
    @@user.ticket('server_id').should.not.be.nil
  end
  
  it 'should verify blacklists' do
    @@user.should.not.blacklisted?('server_id', @@server.blacklist, @@server.cert)
  end
end