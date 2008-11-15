#!/usr/bin/env ruby

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Server' do
  before(:each) do
    @cur_link_window = 10
    @cur_time_period = 2
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @sign_key_n = File.expand_path('sign_n.key')
    @verify_key_n = File.expand_path('verify_n.key')
    @user_id = Nymble.digest('user_id')

    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @pm.link_window = @cur_link_window
    @pm.time_period = @cur_time_period
    @pseudonym = @pm.create_pseudonym(@user_id)
    
    @nm = Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n)
    @nm.link_window = @cur_link_window
    @nm.time_period = @cur_time_period
    @server_id = Nymble.digest('server_id')
    @hmac_key_ns = @nm.add_server(@server_id)
    @blacklist = @nm.create_blacklist(@server_id)
    @credential = @nm.create_credential(@server_id, @pseudonym, 5)
    
    @user = Nymble::User.new(@pseudonym, @verify_key_n)
    @user.link_window = @cur_link_window
    @user.time_period = @cur_time_period
    @user.add_blacklist(@server_id, @blacklist)
    @user.add_credential(@server_id, @credential)
    @ticket = @user.ticket(@server_id)
    @complaint = @ticket.complain(@user.time_period)
    
    @server = Nymble::Server.new(@server_id)
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:Server)
  end
  
  it 'should be creatable' do
    Nymble::Server.new(@server_id).should.not.be.nil
  end
  
  it 'should manage server id' do
    @server.should.respond_to?(:server_id)
    @server.server_id.should.equal(@server_id)
  end
  
  it 'should manage the link window' do
    @server.should.respond_to?(:link_window=)
    @server.link_window = @cur_link_window
    @server.should.respond_to?(:link_window)
    @server.link_window.should.equal(@cur_link_window)
  end
  
  it 'should manage the time period' do
    @server.should.respond_to?(:time_period=)
    @server.time_period = @cur_time_period
    @server.should.respond_to?(:time_period)
    @server.time_period.should.equal(@cur_time_period)
  end
  
  it 'should manage the hmac key' do
    @server.should.respond_to?(:hmac_key_ns=)
    @server.hmac_key_ns = @hmac_key_ns
  end
  
  it 'should manage the blacklist' do
    @server.should.respond_to?(:blacklist=)
    @server.should.blacklist = @blacklist
  end
  
  it 'should manage finalization' do
    @server.time_period = @cur_time_period
    @server.link_window = @cur_link_window
    @server.should.respond_to?(:finalized?)
    @server.should.not.be.finalized?
    @server.blacklist = @blacklist
    @server.should.be.finalized?
    @server.time_period = @cur_time_period + 1
    @nm.time_period = @cur_time_period + 1
    @server.should.not.be.finalized?
    @blacklist = @nm.create_blacklist(@server_id)
    @server.blacklist = @blacklist
    @server.should.be.finalized?
  end
  
  it 'should verify tickets' do
    @server.time_period = @cur_time_period
    @server.link_window = @cur_link_window
    @server.hmac_key_ns = @hmac_key_ns
    @server.blacklist = @blacklist
    @server.should.respond_to?(:valid_ticket?)
    
    @server.should.valid_ticket?(@ticket)
  end
  
  it 'should not verify tickets with incorrect link windows' do
    @server.link_window = @cur_link_window + 1
    
    @server.should.not.valid_ticket?(@ticket)
  end
  
  it 'should not verify tickets with incorrect time periods' do
    @server.time_period = @cur_time_period
    @server.link_window = @cur_link_window
    
    @user.time_period = @cur_time_period - 1
    @server.should.not.valid_ticket?(@user.ticket(@server_id))
    
    @user.time_period = @cur_time_period + 1
    @server.should.not.valid_ticket?(@user.ticket(@server_id))
  end
  
  it 'should manage tokens' do
    @nm.time_period = @cur_time_period + 1
    @server.time_period = @cur_time_period + 1
    @user.time_period = @cur_time_period + 1
    
    @server.should.respond_to(:add_tokens)
    tokens = @nm.create_tokens(@server_id, @blacklist, [ @complaint ])
    tokens.should.not.be.empty?
    
    @server.add_tokens(tokens)
    @server.should.not.valid_ticket?(@user.ticket(@server_id))
  end
end