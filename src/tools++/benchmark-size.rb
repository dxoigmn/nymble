#!/usr/bin/env ruby

require 'benchmark'
require '../libnymble++-ruby/nymble'

def benchmark(name, count = 1, &block)
  benchmark   = Benchmark.measure(&block)
  count       = 1 if count < 1
  benchmark  /= count
  
  @benchmarks << [name, benchmark]
end

fail unless ARGV.length > 0

user_count = ARGV[0].to_i


@benchmarks     = []
@cur_link_window = 1
@cur_time_period = 1
@hmac_key_np = Nymble.digest('hmac_key_np')
@sign_key_n = File.expand_path(File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'sign_n.key'))
@verify_key_n = File.expand_path(File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'verify_n.key'))
@server_id = Nymble.digest('server_id')

@nm = Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n)
@nm.link_window = @cur_link_window
@nm.time_period = @cur_time_period

@pm = Nymble::PseudonymManager.new(@hmac_key_np)
@pm.link_window = @cur_link_window
@pm.time_period = @cur_time_period

@server = Nymble::Server.new(@server_id)
@server.link_window = @cur_link_window
@server.time_period = @cur_time_period
@server.hmac_key_ns = @nm.add_server(@server_id)
@server.blacklist   = @nm.create_blacklist(@server_id)

@users = (1..user_count).map do
  @pm.create_pseudonym(Nymble.digest(rand))
end

@users = @users.map do |user|
  user = Nymble::User.new(user, @verify_key_n)
  user.link_window = @cur_link_window
  user.time_period = @cur_time_period
  user.add_blacklist @server.blacklist
  user.add_credential @nm.create_credential(@server.server_id, user.pseudonym, 1)
  user
end

tickets = @users.map { |user| user.ticket(@server.server_id) }

@server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, tickets)

@benchmarks << ['blacklist_size', @server.blacklist.marshal.size]

if @users.first
  credential = @nm.create_credential(@server.server_id, @users.first.pseudonym, user_count)

  @benchmarks << ['credential_size', credential.marshal.size]

  ticket = @users.first.ticket(@server.server_id)

  @benchmarks << ['ticket_size', ticket.marshal.size]
else
  @benchmarks << ['credential_size', 0]
  @benchmarks << ['ticket_size', 0]
end

puts "user_count," + @benchmarks.map { |label, bm| label }.join(",") if user_count == 0
puts user_count.to_s + "," + @benchmarks.map { |label, bm| bm }.join(",")
