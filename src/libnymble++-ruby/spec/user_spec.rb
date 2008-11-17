#!/usr/bin/env ruby

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'User' do
  before(:each) do
    @cur_link_window = 10
    @cur_time_period = 2
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @sign_key_n = File.expand_path('sign_n.key')
    @verify_key_n = File.expand_path('verify_n.key')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @pseudonym = @pm.create_pseudonym(@user_id)
    @nm = Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n)
    @server_id = Nymble.digest('server_id')
    @hmac_key_ns = @nm.add_server(@server_id)
    @blacklist = @nm.create_blacklist(@server_id)
    @credential = @nm.create_credential(@server_id, @pseudonym, 1)
    @user = Nymble::User.new(@pseudonym, @verify_key_n)
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:User)
  end
  
  it 'should be created with a valid pseudonym and verify key' do
    Nymble::User.new(@pseudonym, @verify_key_n).should.not.be.nil
  end
  
  it 'should manage the link window' do
    @user.should.respond_to?(:link_window=)
    @user.link_window = @cur_link_window
    @user.should.respond_to?(:link_window)
    @user.link_window.should.equal(@cur_link_window)
  end
  
  it 'should manage the time period' do
    @user.should.respond_to?(:time_period=)
    @user.time_period = @cur_time_period
    @user.should.respond_to?(:time_period)
    @user.time_period.should.equal(@cur_time_period)
  end
  
  it 'should manage blacklist' do
    @user.should.respond_to?(:add_blacklist)
    @user.add_blacklist(@server_id, @nm.create_blacklist(@server_id)).should.be(true)
    @user.time_period = @cur_time_period + 1
    @user.add_blacklist(@server_id, @nm.create_blacklist(@server_id)).should.be(false)
  end
  
  it 'should manage credentials' do
    @user.should.respond_to?(:add_credential)
    @user.add_credential(@server_id, @credential)
  end
end