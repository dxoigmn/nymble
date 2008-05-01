#!/usr/bin/env ruby
%w| rubygems ramaze json |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), 'libnymble_safe')

class MainController < Ramaze::Controller
  def pseudonym
    pseudonym, mac_np = Nymble.pm_pseudonym_create($pm_state, Nymble.hash(request.env['REMOTE_ADDR']), $cur_link_window)

    { :pseudonym => pseudonym, :mac_np => mac_np }.to_json
  end
end

if __FILE__ == $0
  $pm_state         = Nymble.pm_initialize(Nymble.hash('hmac_key_np'))
  $cur_link_window  = 0

  Ramaze.start(:adapter => :mongrel, :port => 3000)
end