#!/usr/bin/env ruby
require File.join(File.dirname(__FILE__), 'libnymble')

module Nymble
  class Digest
    class << self
      alias_method :__size, :size
      
      def size
        __size * 2
      end
    end
  end
  
  class << self
    alias_method :__random_bytes, :random_bytes
    def random_bytes(size)
      __encode(__random_bytes(size))[0...size]
    end
    
    alias_method :__hmac, :hmac
    
    def hmac(key, *values)
      __encode(__hmac(key, *values)).strip
    end

    alias_method :keyed_hash, :hmac

    alias_method :__hash, :hash

    def hash(*values)
      __encode(__hash(*values)).strip
    end

    alias_method :__sign, :sign
    
    def sign(key, *values)
      __encode(__sign(key, *values)).strip
    end

    alias_method :__verify, :verify

    def verify(sig, key, *values)
      __verify(__decode(sig), key, *values)
    end

    alias_method :__encrypt, :encrypt

    def encrypt(key, *values)
      __encode(__encrypt(key, *values)).strip
    end

    alias_method :__decrypt, :decrypt

    def decrypt(key, ciphertext)
      __decrypt(key, __decode(ciphertext))
    end
    
private

    def __encode(value)
      ret = ""
      value.each_byte { |byte| ret << ("00" + byte.to_s(16))[-2..-1] }
      ret
    end
    
    def __decode(value)
      ret = ""
      value = value.dup
      ret << value.slice!(0, 2).to_i(16).chr while value.size > 0
      ret
    end
  end
end