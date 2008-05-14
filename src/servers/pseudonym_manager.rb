#!/usr/bin/env ruby
%w| rubygems ramaze json |.each { |lib| require lib }
require File.join(File.dirname(__FILE__), '../libnymble-ruby/libnymble')

class MainController < Ramaze::Controller
  def pseudonym
    $now = Time.now.getutc
    # Assuming the link window is daily
    $cur_link_window = 366 * ($now.year - 1970) + $now.yday
    
    pseudonym, mac_np = Nymble.pm_pseudonym_create($pm_state, Nymble.digest(request.env['REMOTE_ADDR']), $cur_link_window)

    p [pseudonym, mac_np]

    a = Nymble.pseudonym_marshall(*[pseudonym, mac_np])
    puts a
    #{ :pseudonym => pseudonym, :mac_np => mac_np }.to_json
    a
  end
end

if __FILE__ == $0
  $pm_state         = Nymble.pm_initialize(Nymble.digest('hmac_key_np'))
  $cur_link_window  = 0

  Ramaze.start(:adapter => :mongrel, :port => 3000)
end