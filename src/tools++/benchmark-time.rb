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

@users           = []

benchmark :pm_pseudonym_create_average, user_count do
  @users = (1..user_count).map do
    @pm.create_pseudonym(Nymble.digest(rand))
  end
end

@users = @users.map do |user|
  user = Nymble::User.new(user, @verify_key_n)
  user.link_window = @cur_link_window
  user.time_period = @cur_time_period
  user
end

benchmark :user_blacklist_update_average, @users.size do
  @users.each do |user|
    fail "user blacklist update shouldn't fail" unless user.add_blacklist(@server.blacklist)
  end
end

benchmark :nm_credential_create_user_entry_initialize_average, @users.size do
  @users.each do |user|
    credential = @nm.create_credential(@server.server_id, user.pseudonym, @users.size)
    fail "credential should not be NULL" unless credential
    user.add_credential(credential)
  end
end

benchmark :user_blacklist_check_pass_average, @users.size do
  @users.each do |user|
    fail "blacklisting check shouldn't pass" unless user.ticket(@server.server_id)
  end
end

tickets = @users.map do |user|
  user.ticket(@server.server_id)
end

benchmark :server_ticket_verify_pass_average, tickets.size do
  tickets.each do |ticket|
    fail "ticket verification shouldn't fail" unless @server.valid_ticket?(ticket)
  end
end

benchmark :nm_tokens_create_nm_blacklist_update do
  @server.add_linking_tokens @nm.create_linking_tokens(@server.server_id, @server.blacklist, tickets)
  @server.blacklist = @nm.update_blacklist(@server.server_id, @server.blacklist, tickets)
end

benchmark :user_blacklist_update_average_2, @users.size do
  @users.each do |user|
    fail "user blacklist update shouldn't fail" unless user.add_blacklist(@server.blacklist)
  end
end

benchmark :user_blacklist_check_fail_average, @users.size do
  @users.each do |user|
    fail "blacklisting check shouldn't fail" if user.ticket(@server.server_id)
  end
end

benchmark :server_iterate do
  @server.time_period = @server.time_period + 1
end

benchmark :server_ticket_verify_fail_average, tickets.size do
  tickets.each do |ticket|
    fail "ticket verification shouldn't pass" if @server.valid_ticket?(ticket)
  end
end

puts "user_count," + @benchmarks.map { |label, bm| label }.join(",") if user_count == 0
puts user_count.to_s + "," + @benchmarks.map { |label, bm| "%.6f" % bm.to_a[5] }.join(",")
