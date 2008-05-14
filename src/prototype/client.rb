#!/usr/bin/env ruby

require 'rubygems'
require 'uri'
require 'net/http'
require 'cgi'
require 'optparse'
require 'openssl'

require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'libnymble')

def cur_link_window
  cur_time = Time.now.getutc
  366 * (cur_time.year - 1970) + cur_time.yday
end

def cur_time_period
  cur_time = Time.now.getutc
  (cur_time.hour * 60 + cur_time.min) / 5
end

SERVER_ID = Nymble.digest('localhost:3003')
$L        = 288

response = Net::HTTP.get_response(URI.parse("http://localhost:3000/pseudonym"))

fail(response.body) unless response.kind_of?(Net::HTTPOK) || response.kind_of?(String)

verify_key = File.read('nm.pub')
pseudonym, mac_np = Nymble.pseudonym_unmarshall(response.body)

USER_STATE = Nymble.user_initialize(pseudonym, mac_np, verify_key)

fail "Bad user state" unless USER_STATE

data = {
  :pseudonym  => Nymble.pseudonym_marshall(pseudonym, mac_np),
  :server_id  => SERVER_ID,
}

response = Net::HTTP.post_form(URI.parse("http://localhost:3001/credential"), data)

fail(response.body) unless response.kind_of?(Net::HTTPOK) || response.kind_of?(String)

credential = Nymble.user_credential_unmarshall(response.body)

Nymble.user_entry_initialize(USER_STATE, SERVER_ID, credential)

response = Net::HTTP.get_response(URI.parse("http://localhost:3003/blacklist"))

fail(response.body) unless response.kind_of?(Net::HTTPOK) || response.kind_of?(String)

blacklist = Nymble.blacklist_unmarshall(response.body)

fail "Unable to verify blacklist" unless Nymble.user_blacklist_update(USER_STATE, SERVER_ID, blacklist, cur_link_window, cur_time_period)
fail "Client is blacklisted" if Nymble.user_blacklist_check(USER_STATE, SERVER_ID)

data = {
  :ticket => Nymble.ticket_marshall(Nymble.user_credential_get(USER_STATE, SERVER_ID, cur_time_period))
}

response = Net::HTTP.post_form(URI.parse("http://localhost:3003/authenticate"), data)

puts response.body
