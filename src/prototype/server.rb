#!/usr/bin/env ruby

require 'rubygems'
require 'sinatra'
require 'rest_client'
require 'yaml'
require 'digest'
require File.join(File.dirname(__FILE__), '..', 'libnymble-ruby', 'nymble')

Sinatra::Application.default_options[:port] = 3002

configure do
  $L = 1440

  data = YAML.parse(RestClient.post('http://localhost:3001/server/', '')).transform

  SERVER_ID     = data[:server_id]
  SERVER_STATE  = Nymble.server_initialize(SERVER_ID, data[:hmac_key_ns], Nymble.blacklist_unmarshall(data[:blacklist]))
  SEEN_TICKETS  = {}
  COMPLAINTS    = {}
  
  fail unless SERVER_STATE
end

helpers do
  def cur_link_window
    cur_time = Time.now.getutc
    366 * (cur_time.year - 1970) + cur_time.yday
  end
  
  def cur_time_period
    cur_time = Time.now.getutc
    (cur_time.hour * 60 + cur_time.min) / 1
  end
  
  def hexencode(value)
    ret = ""
    value.each_byte { |byte| ret << ("00" + byte.to_s(16))[-2..-1] }
    ret
  end

  def hexdecode(value)
    ret = ""
    value = value.dup
    ret << value.slice!(0, 2).to_i(16).chr while value.size > 0
    ret
  end
end

before do
  unless Nymble.server_blacklist_finalized(SERVER_STATE, cur_time_period)
    data              = {}
    data[:blacklist]  = Nymble.blacklist_marshall(Nymble.server_blacklist(SERVER_STATE))
    data[:complaints] = COMPLAINTS.values.first if COMPLAINTS.size > 0
    
    data            = YAML.parse(RestClient.post("http://localhost:3001/server/#{hexencode(SERVER_ID)}/", data)).transform
    new_blacklist   = Nymble.blacklist_unmarshall(data[:new_blacklist])
    linking_tokens  = nil
    linking_tokens  = [Nymble.linking_token_unmarshall(data[:linking_tokens])] if data[:linking_tokens]
    
    Nymble.server_update(SERVER_STATE, new_blacklist, linking_tokens)
    Nymble.server_blacklist_finalize(SERVER_STATE)
    
    COMPLAINTS.clear
    
    puts "Unable to update blacklist" unless Nymble.server_blacklist_finalized(SERVER_STATE, cur_time_period)
  end
end

get '/' do
  complaint = SEEN_TICKETS[request[:ticket]]
  COMPLAINTS[hexencode(Nymble.digest(complaint))] = complaint if complaint

  @tickets    = SEEN_TICKETS.keys
  @complaints = COMPLAINTS.keys
  
  haml :index
end

get '/nymble/' do
  blacklist = Nymble.server_blacklist(SERVER_STATE)

  { :server_id  => SERVER_ID,
    :blacklist  => Nymble.blacklist_marshall(blacklist) }.to_yaml
end

post '/nymble/' do
  ticket = Nymble.ticket_unmarshall(request[:ticket])
  
  SEEN_TICKETS[hexencode(Nymble.digest(request[:ticket]))] = request[:ticket]
  
  return 'you are blacklisted' unless Nymble.server_ticket_verify(SERVER_STATE, ticket, cur_link_window, cur_time_period)
  
  'authenticated'
end

template :layout do
<<EOS
!!! XML
!!! Strict
%html{html_attrs}
  %head
    %meta{ :'http-equiv' => 'content-type', :content => 'text/html;charset=UTF-8' }
    %title= 'Nymble'
  %body
    #header
      %h1= 'Nymble'
    
    #main
      =yield
EOS
end

template :index do
<<EOS
%h2= 'Recent Sessions'
%ul
  - for ticket in @tickets
    %li
      %a{ :href => '/?ticket=' + ticket}= ticket
      
%h2= 'Pending Complaints'
%ul
  - for complaint in @complaints
    %li= complaint
EOS
end
