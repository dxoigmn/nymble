#!/usr/bin/env ruby
%w| rubygems ramaze json openssl cgi webrick webrick/https active_support |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), '../libnymble/libnymble')

class MainController < Ramaze::Controller
  def register
    $now = Time.now.getutc
    # Assuming the link window is daily
    $cur_link_window = 366 * ($now.year - 1970) + $now.yday
    $cur_time_period = ($now.hour * 60 + $now.min) / 5
    
    server_id = request[:server_id]
    
    respond('server_id already registered', 400) if Nymble.nm_entry_exists($nm_state, server_id)
    
    hmac_key_ns = Nymble.nm_entry_add($nm_state, server_id)
    
    Nymble.nm_entry_update($nm_state, server_id, $cur_time_period)
    blacklist   = Nymble.nm_blacklist_create($nm_state, server_id, $cur_time_period, $cur_link_window)
    
    p Nymble.blacklist_marshall(blacklist)
    
    "hmac_key_ns=#{CGI.escape(hmac_key_ns)}&blacklist=#{CGI.escape(Nymble.blacklist_marshall(blacklist))}"
  end
    
  def credential
    $now = Time.now.getutc
    # Assuming the link window is daily
    $cur_link_window = 366 * ($now.year - 1970) + $now.yday
    $cur_time_period  = ($now.hour * 60 + $now.min) / 5

    server_id = Nymble.digest(request[:server_id])
    pseudonym, mac_np = Nymble.pseudonym_unmarshall(request[:pseudonym])
    
    respond('invalid server_id', 400) unless Nymble.nm_entry_exists($nm_state, server_id)
    respond('invalid pseudonym', 400) unless Nymble.nm_pseudonym_verify($nm_state, pseudonym, $cur_link_window, mac_np)
  
    c = Nymble.nm_credential_marshall(Nymble.nm_credential_create($nm_state, pseudonym, server_id, $cur_link_window))
    
    puts c
    
    c
  end
  
  def update
    $now = Time.now.getutc
    # Assuming the link window is daily
    $cur_link_window = 366 * ($now.year - 1970) + $now.yday
    $cur_time_period  = ($now.hour * 60 + $now.min) / 5

    server_id   = request[:server_id]
    blacklist_cert = Nymble.blacklist_cert_unmarshall(request[:blacklist_cert])
    
    respond('invalid server_id', 400) unless Nymble.nm_entry_exists($nm_state, server_id)    
    
    respond('invalid blacklist cert', 400) unless Nymble.nm_blacklist_cert_verify($nm_state, blacklist, server_id, $cur_link_window)
    
    
  end
  
  def complain
    $now = Time.now.getutc
    # Assuming the link window is daily
    $cur_link_window = 366 * ($now.year - 1970) + $now.yday
    $cur_time_period = ($now.hour * 60 + $now.min) / 5
   
    server_id   = request[:server_id]
    blacklist   = Nymble.blacklist_unmarshall(request[:blacklist])
    complaints  = [Nymble.ticket_unmarshall(request[:complaints])] if request[:complaints]

    p "checking nm_entry_exists"
    respond('invalid server_id', 400) unless Nymble.nm_entry_exists($nm_state, server_id)    

    p "checking nm_blacklist_verify"    
    # FIXME: shouldn't be doing this cur_link_window - 1 business here
    respond('invalid blacklist', 400) unless (Nymble.nm_blacklist_verify($nm_state, blacklist, server_id, $cur_link_window) || Nymble.nm_blacklist_verify($nm_state, blacklist, server_id, $cur_link_window - 1))

    p "done"

    if request[:complaints]
      linking_tokens  = Nymble.linking_token_marshall(Nymble.nm_tokens_create($nm_state, server_id, blacklist, complaints, $cur_time_period, $cur_link_window)[0])
    end
    
    new_blacklist   = Nymble.blacklist_marshall(Nymble.nm_blacklist_update($nm_state, blacklist, complaints, $cur_time_period, $cur_link_window))
    
    Nymble.nm_entry_update($nm_state, server_id, $cur_time_period)
    
    if request[:complaints]
      "blacklist=#{CGI.escape(new_blacklist)}&linking_tokens=#{CGI.escape(linking_tokens)}"
    else
      "blacklist=#{CGI.escape(new_blacklist)}"
    end
  end
end

module Ramaze
  module Adapter
    class WEBrickSSL < Base
      def self.run_server(host, port)
        ca = OpenSSL::X509::Certificate.new(File.read("ca-cert.pem"))
        
        key_data = Nymble.nm_verify_private_key($nm_state)
        
        rsa_key = OpenSSL::PKey::RSA.new(key_data)
        
        cert            = OpenSSL::X509::Certificate.new
        cert.subject    = OpenSSL::X509::Name.new({'OU' => 'Nymble', 'CN' => 'localhost'}.to_a)
        cert.issuer     = ca.subject
        cert.not_before = Time.now
        cert.not_after  = Time.now + 10.years
        cert.public_key = rsa_key.public_key
        cert.serial     = rand(100000) # TODO: fill me in!
        cert.version    = 2 # x509v3
        
        ef                      = OpenSSL::X509::ExtensionFactory.new
        ef.subject_certificate  = cert
        ef.issuer_certificate   = ca

        cert.add_extension ef.create_extension("basicConstraints", "CA:FALSE", true)
        cert.add_extension ef.create_extension("subjectKeyIdentifier", "hash")
        cert.add_extension ef.create_extension("extendedKeyUsage", "clientAuth,serverAuth")
        
        cert.sign OpenSSL::PKey::RSA.new(File.read("ca-key.pem")), OpenSSL::Digest::SHA256.new
                
        options = {
          :Port        => port,
          :BindAddress => host,
          :SSLEnable   => true, 
          :SSLCACertificateFile => "ca-cert.pem",
          :SSLPrivateKey        => rsa_key,
          :SSLCertificate       => cert,
          :Logger      => Log,
          :AccessLog   => [
            [Log, ::WEBrick::AccessLog::COMMON_LOG_FORMAT],
            [Log, ::WEBrick::AccessLog::REFERER_LOG_FORMAT]
          ]
        }

        server = ::WEBrick::HTTPServer.new(options)
        server.mount("/", ::Rack::Handler::WEBrick, self)
        thread = Thread.new(server) do |adapter|
          Thread.current[:adapter] = adapter
          adapter.start
        end
      end
    end
  end
end
  
if __FILE__ == $0
  $nm_state         = Nymble.nm_initialize(Nymble.digest('hmac_key_np'))
  $L                = 288

  

  Ramaze.start(:adapter => :WEBrickSSL, :port => 3001)
end