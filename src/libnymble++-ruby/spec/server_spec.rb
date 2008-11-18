#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

require 'rubygems'
require 'test/spec'
require 'nymble'

context 'Server' do
  before(:each) do
    @@nm = Nymble::NymbleManager.new
    @@nm.link_window = 10
    @@nm.time_period = 5
    
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
end