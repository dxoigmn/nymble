abort "Do not use this unless you want to overwrite existing implementation files."

Dir['I*.h'].each do |header|
  p header
  contents = File.read header
  
  header_file = File.open(header[1..-1], 'w+')
  implementation_file = File.open(header[1..-2] + 'cpp', 'w+')
  
  header_file.write("#include \"#{header}\"\n\n")
  implementation_file.write("#include \"#{header[1..-1]}\"\n\n")
  
  in_header = false
  in_implementation = false
  
  contents.each_line do |line|
    in_header = false if in_header && line =~ /^\/\* Implementation file \*\/$/
    in_implementation = false if in_implementation && line =~ /^\/\* End of implementation class template\. \*\//
  
    if in_header
      header_file.write(line.gsub(/_MYCLASS_/, header[1..-3]))
    end
    
    if in_implementation
      implementation_file.write(line.gsub(/_MYCLASS_/, header[1..-3]))
    end

    in_header = true if line =~ /^\/\* Header file \*\/$/
    in_implementation = true if line =~ /^\/\* Implementation file \*\/$/
  end

  header_file.close
  implementation_file.close
end