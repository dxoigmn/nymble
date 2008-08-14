#!/usr/bin/env ruby

require 'benchmark'

def benchmark(name, count = 1, &block)
  benchmark   = Benchmark.measure(&block)
  count       = 1 if count < 1
  benchmark  /= count
  
  p name, benchmark
end

require File.expand_path(File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'nymble'))

@cur_link_window = 1
@cur_time_period = 1
@hmac_key_np = Nymble.digest('hmac_key_np')
@sign_key_n = File.expand_path(File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'sign_n.key'))
@verify_key_n = File.expand_path(File.join(File.dirname(__FILE__), '..', 'libnymble++-ruby', 'verify_n.key'))
@server_id = Nymble.digest('server_id')

def create_pm
  pm = Nymble::PseudonymManager.new(@hmac_key_np)
  pm.link_window = @cur_link_window
  pm.time_period = @cur_time_period
  pm  
end

def create_nm
  nm = Nymble::NymbleManager.new(@hmac_key_np, @sign_key_n)
  nm.link_window = @cur_link_window
  nm.time_period = @cur_time_period
  nm
end

def create_server(nm, server_id)
  server = Nymble::Server.new(server_id)
  server.link_window = @cur_link_window
  server.time_period = @cur_time_period
  server.hmac_key_ns = nm.add_server(server_id)
  server.blacklist   = nm.create_blacklist(server_id)
  server
end

def create_users(count, pm, nm, server)
  users = (1..count).map do |user_id|
    user = Nymble::User.new(pm.create_pseudonym(Nymble.digest(user_id)), @verify_key_n)
    user.link_window = @cur_link_window
    user.time_period = @cur_time_period
    user.add_blacklist(server.blacklist)
    user.add_credential(nm.create_credential(server.server_id, user.pseudonym, 2))
    user
  end
end

@pm = create_pm
@nm = create_nm
@server = create_server(@nm, @server_id)
@users = create_users(1000, @pm, @nm, @server)

RETEST_COUNT = 10