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
    @pm.link_window = 10
    @pm.should.respond_to?(:link_window)
    @pm.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @pm.should.respond_to?(:time_period=)
    @pm.time_period = 21
    @pm.should.respond_to?(:time_period)
    @pm.time_period.should.equal(21)
  end
  
  it 'should create pseudonyms' do
    @pm.should.respond_to?(:create_pseudonym)
    @pm.create_pseudonym(@user_id).should.not.be.nil
  end
end

context 'Pseudonym' do
  before(:each) do
    pm = Nymble::PseudonymManager.new(Nymble.digest('hmac_key_np'))
    @pseudonym = pm.create_pseudonym(Nymble.digest('user_id'))
  end
  
  it 'should be (un)marshallable' do
    bytes = @pseudonym.marshall
    bytes.should.not.be.nil
    pseudonym = Nymble::Pseudonym.unmarshall(bytes)
    pseudonym.should.not.be.nil
    bytes.should.equal(pseudonym.marshall)
  end
end

context 'Nymble Manager' do
  before(:each) do
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @server_id = Nymble.digest('server_id')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @nm = Nymble::NymbleManager.new(@hmac_key_np)
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:NymbleManager)
  end
  
  it 'should be created with a valid hmac key' do
    Nymble::NymbleManager.new(@hmac_key_np).should.not.be.nil
  end
  
  it 'should not be creatable with an invalid hmac key' do
    should.raise(ArgumentError) { Nymble::NymbleManager.new() }
    should.raise(ArgumentError) { Nymble::NymbleManager.new(@hmac_key_np * 2) }
    should.raise(ArgumentError) { Nymble::NymbleManager.new(@hmac_key_np.slice(0, @hmac_key_np.size / 2)) }
  end
  
  it 'should manage the link window' do
    @nm.should.respond_to?(:link_window=)
    @nm.link_window = 10
    @nm.should.respond_to?(:link_window)
    @nm.link_window.should.equal(10)
  end
  
  it 'should manage the time period' do
    @nm.should.respond_to?(:time_period=)
    @nm.time_period = 12
    @nm.should.respond_to?(:time_period)
    @nm.time_period.should.equal(12)
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
    nm = Nymble::NymbleManager.new(Nymble.digest('hmac_key_np'))
    nm.add_server(Nymble.digest('server_id'))
    @blacklist = nm.create_blacklist(Nymble.digest('server_id'))
  end
  
  it 'should be (un)marshallable' do
    bytes = @blacklist.marshall
    bytes.should.not.be.nil
    blacklist = Nymble::Blacklist.unmarshall(bytes)
    blacklist.should.not.be.nil
    bytes.should.equal(blacklist.marshall)
  end
end

context 'Credential' do
  before(:each) do
    pm = Nymble::PseudonymManager.new(Nymble.digest('hmac_key_np'))
    nm = Nymble::NymbleManager.new(Nymble.digest('hmac_key_np'))
    nm.add_server(Nymble.digest('server_id'))
    pseudonym = pm.create_pseudonym(Nymble.digest('user_id'))
    @credential = nm.create_credential(Nymble.digest('server_id'), pseudonym, 1440)
  end
  
  it 'should be (un)marshallable' do
    bytes = @credential.marshall
    bytes.should.not.be.nil
    credential = Nymble::Credential.unmarshall(bytes)
    credential.should.not.be.nil
    bytes.should.equal(credential.marshall)
  end
end

context 'Server' do
  before(:each) do
    @hmac_key_np = Nymble.digest('hmac_key_np')
    @server_id = Nymble.digest('server_id')
    @user_id = Nymble.digest('user_id')
    @pm = Nymble::PseudonymManager.new(@hmac_key_np)
    @nm = Nymble::NymbleManager.new(@hmac_key_np)
    @hmac_key_ns = @nm.add_server(@server_id)
    @blacklist = @nm.create_blacklist(@server_id)
    @server = Nymble::Server.new(@server_id, @hmac_key_ns, @blacklist)
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:Server)
  end
  
  it 'should be created with a valid hmac key' do
    Nymble::Server.new(@server_id, @hmac_key_ns, @blacklist).should.not.be.nil
  end
  
  it 'should manage the link window' do
    @server.should.respond_to?(:link_window=)
    @server.link_window = 67
    @server.should.respond_to?(:link_window)
    @server.link_window.should.equal(67)
  end
  
  it 'should manage the time period' do
    @server.should.respond_to?(:time_period=)
    @server.time_period = 4545
    @server.should.respond_to?(:time_period)
    @server.time_period.should.equal(4545)
  end
  
  it 'should verify tickets' do
    @server.should.respond_to?(:valid_ticket?)
  end
end

context 'User' do
  before(:each) do
    hmac_key_np = Nymble.digest('hmac_key_np')
    user_id = Nymble.digest('user_id')
    @nm = Nymble::NymbleManager.new(hmac_key_np)
    
    @server_id = Nymble.digest('server_id')
    @pseudonym = Nymble::PseudonymManager.new(hmac_key_np).create_pseudonym(user_id)
    @user = Nymble::User.new(@pseudonym, @nm.verify_key)
    @nm.add_server(@server_id).should.not.be.nil
  end
  
  it 'should be defined under Nymble' do
    Nymble.should.const_defined?(:User)
  end
  
  it 'should be created with a valid pseudonym and verify key' do
    Nymble::User.new(@pseudonym, @nm.verify_key).should.not.be.nil
  end
  
  it 'should manage the link window' do
    @user.should.respond_to?(:link_window=)
    @user.link_window = 4
    @user.should.respond_to?(:link_window)
    @user.link_window.should.equal(4)
  end
  
  it 'should manage the time period' do
    @user.should.respond_to?(:time_period=)
    @user.time_period = 32
    @user.should.respond_to?(:time_period)
    @user.time_period.should.equal(32)
  end
  
  it 'should manage servers' do
    @user.should.respond_to?(:add_server)
    entry = @user.add_server(@server_id)
    entry.should.blacklist = @nm.create_blacklist(@server_id)
    entry.credential = @nm.create_credential(@server_id, @pseudonym, 1440)
  end
end