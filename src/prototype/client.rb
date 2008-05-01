#!/usr/bin/env ruby
%w| rubygems uri net/http json optparse |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), 'libnymble_safe')

$cur_link_window = 0
$cur_time_period = 1

options = {}

OptionParser.new do |opts|
  opts.banner = "Usage: client.rb [options]"

  opts.on("-p", "--pseudonym", "Acquire psuedonym.") do
    options[:pseudonym] = true
  end
  
  opts.on("-c", "--credential SERVER_ID", "Acquire credential for SERVER_ID.") do |server_id|
    options[:server_id] = server_id
  end
end.parse!

verify_key = OpenSSL::PKey::RSA.new(File.read('nm.pub'))

if options[:pseudonym]
  response = Net::HTTP.get(URI.parse("http://localhost:3000/pseudonym"))
  
  fail(response) if response.kind_of?(Net::HTTPBadRequest)

  body = JSON.parse(response)

  $user_state = Nymble.user_initialize(body['pseudonym'], body['mac_np'], verify_key)
end

fail "Bad user state" unless $user_state

if options[:server_id]
  data = {
    :pseudonym  => Nymble.user_pseudonym($user_state),
    :mac_np     => Nymble.user_pseudonym_mac($user_state),
    :server_id  => options[:server_id],
  }
  
  response = Net::HTTP.post_form(URI.parse("http://localhost:3001/credential"), data)

  fail(response.body) if response.kind_of?(Net::HTTPBadRequest)

  credential = JSON.parse(response.body)
  
  Nymble.user_entry_initialize($user_state, options[:server_id], credential)

  response = Net::HTTP.get(URI.parse("http://#{options[:server_id]}/blacklist"))

  fail(response.body) if response.kind_of?(Net::HTTPBadRequest)

  blacklist = JSON.parse(response)

  fail "Unable to verify blacklist" unless Nymble.user_blacklist_update($user_state, options[:server_id], blacklist, $cur_link_window, $cur_time_period)
  fail "Client is blacklisted" if Nymble.user_blacklist_check($user_state, options[:server_id])

  data = {
    :ticket => Nymble.user_credential_get($user_state, options[:server_id], $cur_time_period).to_json
  }

  response = Net::HTTP.post_form(URI.parse("http://#{options[:server_id]}/authenticate"), data)
  
  puts response.body
end


#Net::HTTP.post_form('')