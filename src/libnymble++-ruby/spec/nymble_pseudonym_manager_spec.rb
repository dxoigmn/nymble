#!/usr/bin/env ruby

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Pseudonym Manager' do
  before(:each) do
    @cur_link_window = 10
    @cur_time_period = 2
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:PseudonymManager)
  end
  
  it 'should be creatable with a valid hmac key' do
    Nymble::PseudonymManager.new(@hmac_key_np).should.not.be.nil
  end
  
  it 'should not be creatable with a invalid hmac key' do
    should.raise(ArgumentError) { Nymble::PseudonymManager.new() }
    should.raise(ArgumentError) { Nymble::PseudonymManager.new(@hmac_key_np * 2) }
    should.raise(ArgumentError) { Nymble::PseudonymManager.new(@hmac_key_np.slice(0, @hmac_key_np.size / 2)) }
  end
  
  it 'should manage the link window' do
    @pm.should.respond_to?(:link_window=)
    @pm.link_window = @cur_link_window
    @pm.should.respond_to?(:link_window)
    @pm.link_window.should.equal(@cur_link_window)
  end
  
  it 'should manage the time period' do
    @pm.should.respond_to?(:time_period=)
    @pm.time_period = @cur_time_period
    @pm.should.respond_to?(:time_period)
    @pm.time_period.should.equal(@cur_time_period)
  end
  
  it 'should create pseudonyms' do
    @pm.should.respond_to?(:create_pseudonym)
    @pm.create_pseudonym(@user_id).should.not.be.nil
  end
end

context 'Pseudonym' do
  before(:each) do
    @cur_link_window = 10
    @cur_time_period = 2
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @pseudonym = @pm.create_pseudonym(@user_id)
  end
  
  it 'should be (un)marshalable' do
    bytes = @pseudonym.marshal
    bytes.should.not.be.nil
    pseudonym = Nymble::Pseudonym.unmarshal(bytes)
    pseudonym.should.not.be.nil
    bytes.should.equal(pseudonym.marshal)
  end
end