#!/usr/bin/env ruby

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Nymble' do
  it 'should define DIGEST_SIZE' do
    Nymble.const_defined?(:DIGEST_SIZE)
  end
  
  it 'should produce digests of DIGEST_SIZE' do
    digest = Nymble.digest('asdf')
    
    digest.should.not.be(nil)
    digest.size.should.be(Nymble::DIGEST_SIZE)
  end
  
  it 'should hash two equal strings to the same value' do
    Nymble.digest('asdf').should.equal(Nymble.digest('asdf'))
  end
  
  it 'should produce random bytes of a given size' do
    Nymble.random_bytes(20).size.should.equal(20)
  end
  
  it 'should produce sufficiently random bytes' do
    Nymble.random_bytes(20).should.not.equal(Nymble.random_bytes(20))
  end
  
  it 'should be able to hexencode & hexdecode' do
    s = ""
    (0..255).each { |i| s << i.chr }
    
    Nymble.hexdecode(Nymble.hexencode(s)).should.equal(s)
  end
end

context 'Blacklist' do
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
    @hmac_key_ns = @nm.add_server(@server_id)
    @blacklist = @nm.create_blacklist(@server_id)
  end
  
  it 'should be (un)marshalable' do
    bytes = @blacklist.marshal
    bytes.should.not.be.nil
    blacklist = Nymble::Blacklist.unmarshal(bytes)
    blacklist.should.not.be.nil
    bytes.should.equal(blacklist.marshal)
  end
end

context 'Credential' do
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
    @hmac_key_ns = @nm.add_server(@server_id)
    @blacklist = @nm.create_blacklist(@server_id)
    @credential = @nm.create_credential(@server_id, @pseudonym, 1)
  end
  
  it 'should be (un)marshalable' do
    bytes = @credential.marshal
    bytes.should.not.be.nil
    credential = Nymble::Credential.unmarshal(bytes)
    credential.should.not.be.nil
    bytes.should.equal(credential.marshal)
  end
end

context 'Credential' do
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
    @credential = @nm.create_credential(@server_id, @pseudonym, 10)
    
    @user = Nymble::User.new(@pseudonym, @verify_key_n)
    @user.link_window = @cur_link_window
    @user.time_period = @cur_time_period
    @user.add_blacklist(@server_id, @blacklist)
    @user.add_credential(@server_id, @credential)
    
    @ticket = @user.ticket(@server_id)
  end
  
  it 'should be (un)marshalable' do
    bytes = @ticket.marshal
    bytes.should.not.be.nil
    ticket = Nymble::Ticket.unmarshal(bytes)
    ticket.should.not.be.nil
    bytes.should.equal(ticket.marshal)
  end
end

context 'Token' do
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
    @server.link_window = @cur_link_window
    @server.time_period = @cur_time_period
    @token = @nm.create_tokens(@server_id, @blacklist, [ @complaint ]).first
  end
  
  it 'should be (un)marshalable' do
    bytes = @token.marshal
    bytes.should.not.be.nil
    token = Nymble::Token.unmarshal(bytes)
    token.should.not.be.nil
    bytes.should.equal(token.marshal)
  end
end