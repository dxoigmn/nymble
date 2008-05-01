#!/usr/bin/env ruby
%w| openssl |.each { |lib| require lib }

module Nymble
  def self.pm_initialize(hmac_key_np)
    return nil unless hmac_key_np &&
                      hmac_key_np.size == Digest.size
    
    pm_state                  = PseudonymManagerState.new
    pm_state.keyedhash_key_p  = hmac_keygen
    pm_state.hmac_key_np      = hmac_key_np
    
    pm_state
  end
    
  def self.pm_pseudonym_create(pm_state, user_id, link_window)
    return nil unless pm_state.kind_of?(PseudonymManagerState) &&
                      user_id &&
                      user_id.size == Digest.size &&
                      link_window.kind_of?(Integer)
                      
    pseudonym = keyed_hash(pm_state.keyedhash_key_p, user_id, link_window)
    mac_np    = hmac(pm_state.hmac_key_np, pseudonym, link_window)

    return pseudonym, mac_np
  end
    
  def self.nm_initialize(hmac_key_np)
    return nil unless hmac_key_np &&
                      hmac_key_np.size == Digest.size
    
    nm_state                  = NymbleManagerState.new
    nm_state.hmac_key_n       = hmac_keygen
    nm_state.hmac_key_np      = hmac_key_np
    nm_state.keyedhash_key_n  = hmac_keygen
    nm_state.encrypt_key_n    = random_bytes(16)
    nm_state.sign_key_n       = OpenSSL::PKey::RSA.generate(1024)
    nm_state.verify_key_n     = nm_state.sign_key_n.public_key
    nm_state.entries          = { }
    
    nm_state
  end
  
  def self.nm_verify_key(nm_state)
    return nil unless nm_state.kind_of?(NymbleManagerState)
    
    nm_state.verify_key_n
  end
    
  def self.nm_entry_exists(nm_state, server_id)
    return false unless nm_state.kind_of?(NymbleManagerState) &&
                        server_id &&
                        server_id.size == Digest.size
                        
    !nm_state.entries[server_id].nil?
  end
  
  def self.nm_entry_add(nm_state, server_id)
    return nil unless nm_state.kind_of?(NymbleManagerState) &&
                      server_id &&
                      server_id.size == Digest.size
                      
    nm_entry                  = Nymble::NymbleManagerEntry.new
    nm_entry.server_id        = server_id
    nm_entry.hmac_key_ns      = hmac_keygen
    nm_entry.bl_last_updated  = 0

    nm_state.entries[server_id] = nm_entry
    
    nm_entry.hmac_key_ns
  end
  
  def self.nm_entry_update(nm_state, server_id, time_period)
    return false unless nm_state.kind_of?(NymbleManagerState) &&
                        server_id &&
                        server_id.size == Digest.size &&
                        time_period.kind_of?(Integer) &&
                        nm_entry_exists(nm_state, server_id)
                        
    nm_state.entries[server_id].bl_last_updated = time_period
    
    true
  end
  
  def self.nm_pseudonym_verify(nm_state, pseudonym, link_window, mac_np)
    return false unless nm_state.kind_of?(NymbleManagerState) &&
                        pseudonym &&
                        pseudonym.size == Digest.size &&
                        link_window.kind_of?(Integer) &&
                        mac_np &&
                        mac_np.size == Digest.size
    
    hmac(nm_state.hmac_key_np, pseudonym, link_window) == mac_np
  end
  
  def self.nm_blacklist_create(nm_state, server_id, time_period, link_window)
    return nil unless nm_state.kind_of?(NymbleManagerState) &&
                      server_id &&
                      server_id.size == Digest.size &&
                      time_period.kind_of?(Integer) &&
                      link_window.kind_of?(Integer) &&
                      time_period <= $L
                            
    # create_blacklist(nm_state, server_id, time_period, link_window)
    blacklist = Blacklist.new
  
    # metadata
    blacklist.server_id   = server_id
    blacklist.time_period = time_period
    blacklist.link_window = link_window

    # tickets
    blacklist.tickets = []

    # tag
    blacklist.bmac_n  = hmac(nm_state.hmac_key_n, blacklist.metadata, blacklist.tickets)
    blacklist.sig     = sign(nm_state.sign_key_n, blacklist.metadata, blacklist.tickets)

    blacklist
  end
  
  def self.nm_blacklist_verify(nm_state, blacklist, server_id, link_window)
    return false unless nm_state.kind_of?(NymbleManagerState) &&
                        blacklist.kind_of?(Blacklist) &&
                        server_id &&
                        server_id.size == Digest.size
                        link_window.kind_of?(Integer) &&
                        nm_entry_exists(nm_state, server_id)
    
    nm_bl_check_integrity(blacklist, server_id, nm_state.hmac_key_n) &&
    bl_check_freshness(blacklist, nm_state.entries[server_id].bl_last_updated, link_window)
  end
  
  def self.nm_blacklist_update(nm_state, blacklist, complaints, time_period, link_window)
    return nil unless nm_state.kind_of?(NymbleManagerState) &&
                      blacklist.kind_of?(Blacklist) &&
                      time_period.kind_of?(Integer) &&
                      link_window.kind_of?(Integer) &&
                      (complaints.nil? ||
                        (complaints.kind_of?(Array) &&
                         complaints.inject(true) { |value, ticket| value && ticket.kind_of?(Ticket) }))

    complaints ||= []
    
    # update_blacklist(nm_state, blacklist, complaint, time_period, link_window)
    new_blacklist = nm_blacklist_create(nm_state, blacklist.server_id, time_period, link_window)

    # tickets
    new_blacklist.tickets.concat(blacklist.tickets)
    new_blacklist.tickets.concat(complaints)

    # tag
    new_blacklist.bmac_n  = hmac(nm_state.hmac_key_n, new_blacklist.metadata, new_blacklist.tickets)
    new_blacklist.sig     = sign(nm_state.sign_key_n, new_blacklist.metadata, new_blacklist.tickets)

    new_blacklist
  end
  
  def self.nm_credential_create(nm_state, pseudonym, server_id, link_window)
    return nil unless nm_state.kind_of?(NymbleManagerState) &&
                      pseudonym &&
                      server_id &&
                      server_id.size == Digest.size &&
                      link_window.kind_of?(Integer) &&
                      nm_entry_exists(nm_state, server_id)
    
    # create_credential(nm_state, pseudonym, server_id, link_window)
    tickets   = []
    trapdoor  = []

    trapdoor[0] = keyed_hash(nm_state.keyedhash_key_n, pseudonym, server_id, link_window)

    credential          = Credential.new
    credential.seed     = trapdoor[0]
    credential.tickets  = []

    (1..$L).each do |i|
      trapdoor[i] = evolve_trapdoor(trapdoor[i - 1], 1)

      ticket              = Ticket.new
      ticket.server_id    = server_id
      ticket.time_period  = i
      ticket.link_window  = link_window
      ticket.nymble       = compute_nymble(trapdoor[i])
      ticket.trapdoorenc  = encrypt(nm_state.encrypt_key_n, trapdoor[i], pseudonym)
      ticket.mac_n        = hmac(nm_state.hmac_key_n, ticket.content)
      ticket.mac_ns       = hmac(nm_state.entries[server_id].hmac_key_ns, ticket.content, ticket.mac_n)

      credential.tickets << ticket
    end

    credential
  end

  def self.nm_tokens_create(nm_state, server_id, blacklist, complaints, time_period, link_window)
    return nil unless nm_state.kind_of?(NymbleManagerState) &&
                      server_id &&
                      server_id.size == Digest.size &&
                      blacklist.kind_of?(Blacklist) &&
                      (complaints.nil? || (complaints.kind_of?(Array) && complaints.inject(true) { |value, ticket| value && ticket.kind_of?(Ticket) })) &&
                      time_period.kind_of?(Integer) &&
                      link_window.kind_of?(Integer)
    
    # compute_tokens(nm_state, server_id, blacklist.tickets, complaint, time_period, link_window)
    complaints        ||= []
    linking_tokens      = []
    next_time_period    = time_period + 1
    blacklisted_tickets = blacklist.tickets.dup

    complaints.each do |ticket|
      if user_already_blacklisted(blacklisted_tickets, ticket, nm_state.encrypt_key_n)
        trapdoor_for_next_time_period = random_bytes(Digest.size)
      else
        decrypted = decrypt(nm_state.encrypt_key_n, ticket.trapdoorenc)
        trapdoor  = decrypted[0...Digest.size]
        pseudonym = decrypted[Digest.size..-1]

        delta                         = next_time_period - ticket.time_period
        trapdoor_for_next_time_period = evolve_trapdoor(trapdoor, delta)

        blacklisted_tickets << ticket
      end

      linking_token               = LinkingToken.new
      linking_token.server_id     = server_id
      linking_token.link_window   = link_window
      linking_token.time_period   = next_time_period
      linking_token.trapdoor      = trapdoor_for_next_time_period
      linking_token.nymble_ticket = ticket

      linking_tokens << linking_token
    end

    linking_tokens
  end
  
  def self.server_initialize(server_id, hmac_key_ns, blacklist)
    return nil unless server_id && 
                      server_id.size == Digest.size &&
                      hmac_key_ns && 
                      hmac_key_ns.size == Digest.size &&
                      blacklist.kind_of?(Blacklist)

    server_state              = ServerState.new
    server_state.server_id    = server_id
    server_state.hmac_key_ns  = hmac_key_ns
    server_state.blacklist    = blacklist
    server_state.linking_list = {}
    server_state.finalized    = false
    
    server_state
  end
  
  def self.server_ticket_verify(server_state, nymble_ticket, link_window, time_period)
    return false unless server_state.kind_of?(ServerState) &&
                        nymble_ticket.kind_of?(Ticket) &&
                        link_window.kind_of?(Integer) &&
                        time_period.kind_of?(Integer)
    
    # Nymble.verify_ticket(ticket, server_state.server_id, time_period, link_window, server_state.hmac_key_ns) &&
    # !Nymble.link_ticket(ticket, server_state.linking_list)
    
    nymble_ticket.server_id   == server_state.server_id &&
    nymble_ticket.time_period == time_period &&
    nymble_ticket.link_window == link_window &&
    nymble_ticket.mac_ns      == hmac(server_state.hmac_key_ns, nymble_ticket.content, nymble_ticket.mac_n) &&
    !server_state.linking_list.has_key?(nymble_ticket.nymble)
  end
  
  def self.server_blacklist(server_state)
    return nil unless server_state.kind_of?(ServerState)
    
    server_state.blacklist
  end
  
  def self.server_blacklist_finalize(server_state)
    return false unless server_state.kind_of?(ServerState)
    
    server_state.finalized = true
    
    true
  end
  
  def self.server_blacklist_finalized(server_state)
    return false unless server_state.kind_of?(ServerState)
    
    server_state.finalized
  end
  
  def self.server_iterate(server_state, time_period_delta)
    return false unless server_state.kind_of?(ServerState) &&
                        time_period_delta.kind_of?(Integer)
    
    new_linking_list = {}
    
    server_state.linking_list.values.each do |linking_list_entry|
      linking_list_entry.trapdoor      = evolve_trapdoor(linking_list_entry.trapdoor, time_period_delta)
      linking_list_entry.nymble        = compute_nymble(linking_list_entry.trapdoor)
      linking_list_entry.time_period  += time_period_delta
      
      new_linking_list[linking_list_entry.nymble] = linking_list_entry
    end

    server_state.linking_list = new_linking_list
    server_state.finalized    = false
    
    true
  end

  def self.server_update(server_state, blacklist, linking_tokens)
    return false unless server_state.kind_of?(ServerState) &&
                        blacklist.kind_of?(Blacklist) &&
                        linking_tokens.kind_of?(Array) &&
                        linking_tokens.inject(true) { |value, token| value && token.kind_of?(LinkingToken) }
                        
    server_state.blacklist = blacklist
    
    linking_tokens.each do |linking_token|
      linking_list_entry              = LinkingListEntry.new
      linking_list_entry.time_period  = linking_token.time_period
      linking_list_entry.trapdoor     = linking_token.trapdoor
      linking_list_entry.nymble       = linking_token.nymble_ticket.nymble
      
      server_state.linking_list[linking_list_entry.nymble] = linking_list_entry
    end
    
    true
  end
  
  def self.user_initialize(pseudonym, mac_np, verify_key_n)
    return nil unless pseudonym &&
                      mac_np &&
                      mac_np.size == Digest.size &&
                      verify_key_n
    
    user_state              = UserState.new
    user_state.pseudonym    = pseudonym
    user_state.mac_np       = mac_np
    user_state.verify_key_n = verify_key_n
    user_state.entries      = {}
    
    user_state
  end
  
  def self.user_pseudonym(user_state)
    return nil unless user_state.kind_of?(UserState)

    user_state.pseudonym
  end
  
  def self.user_pseudonym_mac(user_state)
    return nil unless user_state.kind_of?(UserState)

    user_state.mac_np
  end
  
  def self.user_entry_initialize(user_state, server_id, credential)
    return false unless user_state.kind_of?(UserState) &&
                        server_id &&
                        server_id.size == Digest.size &&
                        credential.kind_of?(Credential)
    
    user_entry                    = UserEntry.new
    user_entry.server_id          = server_id
    user_entry.link_window        = 0
    user_entry.credential         = credential
    user_entry.blacklist          = nil
    user_entry.blacklisted        = false
    user_entry.last_authenticated = 0
    
    user_state.entries[server_id] = user_entry
    
    true
  end
  
  def self.user_entry_exists(user_state, server_id)
    return false unless user_state.kind_of?(UserState) &&
                        server_id &&
                        server_id.size == Digest.size
                        
    !user_state.entries[server_id].nil?
  end
  
  def self.user_blacklist_update(user_state, server_id, blacklist, link_window, time_period)
    return false unless user_state.kind_of?(UserState) && 
                        server_id &&
                        server_id.size == Digest.size &&
                        blacklist.kind_of?(Blacklist) &&
                        link_window.kind_of?(Integer) &&
                        time_period.kind_of?(Integer) &&
                        user_entry_exists(user_state, server_id)
                        
    return false unless bl_check_integrity(blacklist, server_id, user_state.verify_key_n) &&
                        bl_check_freshness(blacklist, time_period, link_window)

    user_state.entries[server_id].blacklist   = blacklist
    user_state.entries[server_id].blacklisted = false
    user_state.entries[server_id].link_window = link_window
    
    true
  end

  def self.user_blacklist_check(user_state, server_id)
    return nil unless user_state.kind_of?(UserState) &&
                      server_id &&
                      server_id.size == Digest.size
                      user_entry_exists(user_state, server_id)
    
    # i_am_blacklisted(user_state.entries[server_id].blacklist, user_state.entries[server_id].credential)
    blacklist   = user_state.entries[server_id].blacklist
    credential  = user_state.entries[server_id].credential
    flag        = false

    blacklist.tickets.each do |ticket|
      nymble      = ticket.nymble
      cred_ticket = credential.tickets.find {|tkt| tkt.time_period == ticket.time_period}
      cred_nymble = cred_ticket.nymble

      flag = true if nymble == cred_nymble
    end

    flag
  end
  
  def self.user_credential_get(user_state, server_id, time_period)
    return nil unless user_state.kind_of?(UserState) &&
                      server_id &&
                      server_id.size == Digest.size
                      time_period.kind_of?(Integer) &&
                      user_entry_exists(user_state, server_id)
    
    user_state.entries[server_id].credential.tickets.find do |ticket|
      ticket.time_period == time_period
    end
  end

  def self.hash(*values)
    digest = Digest.new
    values.flatten.each { |value| digest.update(value.to_s) }
    digest.digest
  end

private
  class Cipher < OpenSSL::Cipher::AES128
    def initialize()
      super('CBC')
    end
  end
  
  if OpenSSL::Digest.const_defined? 'SHA2'
    class Digest < OpenSSL::Digest::SHA2
      def self.size
        32
      end
    end
  elsif OpenSSL::Digest.const_defined? 'SHA256'
    class Digest < OpenSSL::Digest::SHA256
      def self.size
        32
      end
    end    
  else
    class Digest < OpenSSL::Digest::SHA1
      def self.size
        20
      end
    end
  end
  
  class Ticket
    attr_accessor :server_id
    attr_accessor :time_period
    attr_accessor :link_window
    attr_accessor :nymble
    attr_accessor :trapdoorenc
    attr_accessor :mac_n
    attr_accessor :mac_ns
    
    # content
    def content
      [ server_id, link_window, time_period, nymble, trapdoorenc ]
    end
    
    def to_s
      content.concat([mac_n, mac_ns]).to_s
    end
            
    def to_json(*a)
      {
        'json_class'  => self.class.name,
        'server_id'   => server_id,
        'time_period' => time_period,
        'link_window' => link_window,
        'nymble'      => nymble,
        'trapdoorenc' => trapdoorenc,
        'mac_n'       => mac_n,
        'mac_ns'      => mac_ns,
      }.to_json(*a)
    end
    
    def self.json_create(o)
      ticket = Ticket.new
      ticket.server_id    = o['server_id']
      ticket.time_period  = o['time_period']
      ticket.link_window  = o['link_window']
      ticket.nymble       = o['nymble']
      ticket.trapdoorenc  = o['trapdoorenc']
      ticket.mac_n        = o['mac_n']
      ticket.mac_ns       = o['mac_ns']
      ticket
    end
  end

  class Credential
    attr_accessor :seed
    attr_accessor :tickets
    
    def to_json(*a)
      {
        'json_class'  => self.class.name,
        'seed'        => seed,
        'tickets'     => tickets
      }.to_json(*a)
    end
    
    def self.json_create(o)
      credential          = Credential.new
      credential.seed     = o['seed']
      credential.tickets  = o['tickets']
      credential
    end
  end

  class Blacklist
    # metadata
    attr_accessor :server_id
    attr_accessor :time_period 
    attr_accessor :link_window

    def metadata
      [server_id, time_period, link_window]
    end

    # tag
    attr_accessor :bmac_n
    attr_accessor :sig
    
    def tag
      [bmac_n, sig]
    end

    # tickets
    attr_accessor :tickets
    
    def to_json(*a)
      {
        'json_class'  => self.class.name,
        'server_id'   => server_id,
        'time_period' => time_period,
        'link_window' => link_window,
        'bmac_n'      => bmac_n,
        'sig'         => sig,
        'tickets'     => tickets
      }.to_json(*a)
    end
    
    def self.json_create(o)
      blacklist             = Blacklist.new
      blacklist.server_id   = o['server_id']
      blacklist.time_period = o['time_period']
      blacklist.link_window = o['link_window']
      blacklist.bmac_n      = o['bmac_n']
      blacklist.sig         = o['sig']
      blacklist.tickets     = o['tickets']
      blacklist
    end
  end

  class LinkingToken
    attr_accessor :server_id
    attr_accessor :link_window
    attr_accessor :time_period
    attr_accessor :trapdoor
    attr_accessor :nymble_ticket
  end
  
  class UserState
    attr_accessor :pseudonym
    attr_accessor :mac_np
    attr_accessor :verify_key_n
    attr_accessor :entries
  end
  
  class UserEntry
    attr_accessor :server_id
    attr_accessor :link_window
    attr_accessor :credential
    attr_accessor :blacklist
    attr_accessor :blacklisted
    attr_accessor :last_authenticated
  end
  
  class ServerState
    attr_accessor :server_id
    attr_accessor :hmac_key_ns
    attr_accessor :blacklist
    attr_accessor :linking_list
    attr_accessor :finalized
  end
  
  class LinkingListEntry
    attr_accessor :time_period
    attr_accessor :trapdoor
    attr_accessor :nymble
  end
  
  class PseudonymManagerState
    attr_accessor :hmac_key_np
    attr_accessor :keyedhash_key_p
  end
  
  class NymbleManagerState
    attr_accessor :hmac_key_n
    attr_accessor :hmac_key_np
    attr_accessor :keyedhash_key_n
    attr_accessor :encrypt_key_n
    attr_accessor :sign_key_n    
    attr_accessor :verify_key_n
    attr_accessor :entries
  end
  
  class NymbleManagerEntry
    attr_accessor :server_id
    attr_accessor :bl_last_updated
    attr_accessor :hmac_key_ns
  end

  def self.evolve_trapdoor(trapdoor, delta)
    (1..delta).each { trapdoor = hash('f', trapdoor) }
    trapdoor
  end

  def self.compute_nymble(trapdoor)
    hash('g', trapdoor)
  end

  def self.nm_bl_check_integrity(blacklist, sid, hmac_key_n)
    hmac(hmac_key_n, blacklist.metadata, blacklist.tickets) == blacklist.bmac_n &&
    blacklist.server_id == sid
  end

  def self.bl_check_integrity(blacklist, sid, verify_key_n)
    verify(blacklist.sig, verify_key_n, blacklist.metadata, blacklist.tickets) &&
    blacklist.server_id == sid
  end

  def self.bl_check_freshness(blacklist, period, window)
    blacklist.time_period == period &&
    blacklist.link_window == window
  end

  def self.user_already_blacklisted(bl_tickets, ticket, encrypt_key_n)
    flag      = false
    pseudonym = decrypt(encrypt_key_n, ticket.trapdoorenc)[Digest.size..-1]

    bl_tickets.each do |bl_ticket|
      flag = true if decrypt(encrypt_key_n, bl_ticket.trapdoorenc)[Digest.size..-1] == pseudonym
    end

    return flag
  end

  def self.hmac_keygen()
    random_bytes(Digest.size)
  end

  def self.hmac(key, *values)
    hmac = OpenSSL::HMAC.new(key, Digest.new)
    values.flatten.each { |value| hmac.update(value.to_s) }
    hmac.digest
  end

  def self.keyed_hash(key, *values)
    hmac(key, *values)
  end
  
  def self.encrypt(key, *values)
    cipher = Cipher.new
    cipher.encrypt
    cipher.key  = key
    cipher.iv   = ciphertext = random_bytes(cipher.iv_len)
  
    values.flatten.each do |value| 
      ciphertext << cipher.update(value)
    end
  
    ciphertext << cipher.final
  end

  def self.decrypt(key, ciphertext)
    cipher = Cipher.new
    cipher.decrypt
    cipher.key = key
    cipher.iv  = ciphertext[0...cipher.iv_len]
  
    cipher.update(ciphertext[cipher.iv_len..-1]) << cipher.final
  end

  def self.sign(key, *values)
    key.sign(Digest.new, values.flatten.join)
  end

  def self.verify(sig, key, *values)
    key.verify(Digest.new, sig, values.flatten.join)
  end

  def self.random_bytes(size)
    OpenSSL::Random.random_bytes(size)
  end
end
