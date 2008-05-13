#!/usr/bin/env ruby
%w| benchmark |.each { |lib| require lib }

require 'libnymble'

def benchmark(name, count = 1, &block)
  benchmark   = Benchmark.measure(&block)
  count       = 1 if count < 1
  benchmark  /= count
  
  @benchmarks << [name, benchmark]
end

$L = 288

fail unless ARGV.length > 0

user_count = ARGV[0].to_i


@benchmarks     = []
cur_link_window = 0
cur_time_period = 1
server_id       = Nymble.hash('server_id')
hmac_key_np     = Nymble.hash('hmac_key_np')

nm_state        = Nymble.nm_initialize(hmac_key_np)
pm_state        = Nymble.pm_initialize(hmac_key_np)
server_state    = Nymble.server_initialize(server_id, 
                                         Nymble.nm_entry_add(nm_state, server_id), 
                                         Nymble.nm_blacklist_create(nm_state, server_id, cur_time_period, cur_link_window))
users           = []

benchmark :pm_pseudonym_create_average, user_count do
  users = (1..user_count).map do
    pseudonym, mac_np = Nymble.pm_pseudonym_create(pm_state, Nymble.hash(rand.to_s), cur_link_window)

    Nymble.user_initialize(pseudonym, mac_np, Nymble.nm_verify_key(nm_state))
  end
end

benchmark :nm_credential_create_user_entry_initialize_average, users.size do
  users.each do |user_state|
    credential = Nymble.nm_credential_create(nm_state, Nymble.user_pseudonym(user_state), server_id, cur_link_window)
    Nymble.user_entry_initialize(user_state, server_id, credential)
  end
end

tickets = users.map do |user_state|
  Nymble.user_credential_get(user_state, server_id, cur_time_period)
end

benchmark :server_ticket_verify_pass_average, tickets.size do
  tickets.each do |ticket|
    fail "ticket verification shouldn't fail" unless Nymble.server_ticket_verify(server_state, ticket, cur_link_window, cur_time_period)
  end
end

benchmark :user_blacklist_update_average, users.size do
  users.each do |user_state|
    fail "user blacklist update shouldn't fail" unless Nymble.user_blacklist_update(user_state, server_id, Nymble.server_blacklist(server_state), cur_link_window, cur_time_period)
  end
end


benchmark :user_blacklist_check_pass_average, users.size do
  users.each do |user_state|
    fail "blacklisting check shouldn't pass" if Nymble.user_blacklist_check(user_state, server_id)
  end
end

benchmark :nm_tokens_create_nm_blacklist_update do
  blacklist = Nymble.server_blacklist(server_state)
  complaint = tickets

  linking_tokens  = Nymble.nm_tokens_create(nm_state, server_id, blacklist, complaint, cur_time_period, cur_link_window)
  new_blacklist   = Nymble.nm_blacklist_update(nm_state, blacklist, complaint, cur_time_period, cur_link_window)

  Nymble.server_update(server_state, new_blacklist, linking_tokens)
end

benchmark :user_blacklist_update_average_2, users.size do
  users.each do |user_state|
    fail "user blacklist update shouldn't fail" unless Nymble.user_blacklist_update(user_state, server_id, Nymble.server_blacklist(server_state), cur_link_window, cur_time_period)
  end
end

benchmark :user_blacklist_check_fail_average, users.size do
  users.each do |user_state|
    fail "blacklisting check shouldn't fail" unless Nymble.user_blacklist_check(user_state, server_id)
  end
end

benchmark :server_ticket_verify_fail_average, tickets.size do
  tickets.each do |ticket|
    fail "ticket verification shouldn't pass" if Nymble.server_ticket_verify(server_state, ticket, cur_link_window, cur_time_period)
  end
end

benchmark :server_iterate do
  Nymble.server_iterate(server_state, 1)
end

puts "user_count," + @benchmarks.map { |label, bm| label }.join(",") if user_count == 0
puts user_count.to_s + "," + @benchmarks.map { |label, bm| "%.6f" % bm.to_a[5] }.join(",")
