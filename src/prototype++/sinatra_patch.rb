module Sinatra
  class Base
    class << self
       def run!(options={})
         set options
         handler      = detect_rack_handler
         handler_name = handler.name.gsub(/.*::/, '')
         puts "== Sinatra/#{Sinatra::VERSION} has taken the stage " +
           "on #{port} for #{environment} with backup from #{handler_name}" unless handler_name =~/cgi/i
         handler.run self, handler_options.merge(:Host => host, :Port => port) do |server|
           trap(:INT) do
             ## Use thins' hard #stop! if available, otherwise just #stop
             server.respond_to?(:stop!) ? server.stop! : server.stop
             puts "\n== Sinatra has ended his set (crowd applauds)" unless handler_name =~/cgi/i
           end
         end
       rescue Errno::EADDRINUSE => e
         puts "== Someone is already performing on port #{port}!"
       end
    end
  end
end