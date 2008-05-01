#!/usr/bin/env ruby
%w| rubygems ramaze json net/http |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), 'libnymble_safe')

class MainController < Ramaze::Controller
  def blacklist
    unless Nymble.server_blacklist_finalized($server_state)
      blacklist = Nymble.server_blacklist($server_state).to_json
      
      # TODO: Complain if necessary...
    end

    Nymble.server_blacklist($server_state).to_json
  end

  def authenticate
    ticket = JSON.parse(request[:ticket])
    
    respond('invalid ticket', 400) unless Nymble.server_ticket_verify($server_state, ticket, $cur_link_window, $cur_time_period)
    
    'authenticated'
  end
end

if __FILE__ == $0
  server_id = Nymble.hash('localhost:3003') 
  
  Net::HTTP.get_response('localhost', "/register?server_id=#{server_id}", 3001) do |response|
    fail(response.body) if response.kind_of?(Net::HTTPBadRequest)
    data = JSON.parse(response.body)

    $server_state = Nymble.server_initialize(server_id, data['hmac_key_ns'], data['blacklist'])
    
    fail unless $server_state
  end
  
  $cur_link_window  = 0
  $cur_time_period  = 1
  
  Ramaze.start(:adapter => :mongrel, :port => 3003)
end