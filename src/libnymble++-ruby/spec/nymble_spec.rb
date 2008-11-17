#!/usr/bin/env ruby

$: << File.expand_path(File.join(File.dirname(__FILE__), '..'))

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
    s = (0..255).to_a.map { |i| i.chr }.join
    
    Nymble.hexdecode(Nymble.hexencode(s)).should.equal(s)
  end
end
