#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Server' do
  before(:all) do
    @@nm = Nymble::NymbleManager.new
    @@nm.link_window = 10
    @@nm.time_period = 5
    @@nm.write_verify_key_n('verify_key_n.pem')
    
    @@pm = Nymble::PseudonymManager.new(@@nm.mac_key_np)
    @@pm.link_window = 10
    @@pm.time_period = 5
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:Server)
  end
  
  it 'should be creatable' do
    @@server = Nymble::Server.new(@@nm.register_server('server_id'))
    @@server.should.not.be.nil
  end
  
  it 'should manage the link window' do
    @@server.should.respond_to?(:link_window=)
    @@server.link_window = 10
    @@server.should.respond_to?(:link_window)
    @@server.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @@server.should.respond_to?(:time_period=)
    @@server.time_period = 5
    @@server.should.respond_to?(:time_period)
    @@server.time_period.should.equal(5)
  end
  
  it 'should manage blacklist' do
    @@server.should.respond_to?(:blacklist)
    @@server.blacklist.should.not.be.nil
  end
  
  it 'should manage cert' do
    @@server.should.respond_to?(:cert)
    @@server.cert.should.not.be.nil
  end
  
  it 'should validate tickets' do
    user = Nymble::User.new(@@pm.create_pseudonym('user_id'), 'verify_key_n.pem')
    user.link_window = 10
    user.time_period = 5
    user.add_credential('server_id', @@nm.create_credential('server_id', user.pseudonym))
    @@server.should.valid_ticket?(user.ticket('server_id'))
  end
end