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