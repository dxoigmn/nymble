#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Pseudonym Manager' do
  before do
    @@pm = Nymble::PseudonymManager.new('mac_key_np')
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:PseudonymManager)
  end
  
  it 'should be creatable with a valid hmac key' do
    @@pm.should.not.be.nil
  end
  
  it 'should manage the link window' do
    @@pm.should.respond_to?(:link_window=)
    @@pm.should.respond_to?(:link_window)
    
    @@pm.link_window = 10
    @@pm.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @@pm.should.respond_to?(:time_period=)
    @@pm.should.respond_to?(:time_period)
    
    @@pm.time_period = 5
    @@pm.time_period.should.equal(5)
  end
  
  it 'should create pseudonyms' do
    @@pm.should.respond_to?(:create_pseudonym)
    
    pseudonym = @@pm.create_pseudonym('user_id')
    
    pseudonym.should.not.be.nil
    pseudonym.should.equal(@@pm.create_pseudonym('user_id'))
  end
end
