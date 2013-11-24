#!/usr/bin/env ruby
#
# run all available tests in current directory

count = 0
pass_count = 0
fail_count = 0

working_directory = File.dirname(__FILE__)

testcases = Dir.glob("#{working_directory}/test*.rb")

testcases.sort.each { |test|
  if /test\d+\.rb$/ =~ test
    count += 1
    if system("ruby #{test}")
      pass_count += 1
    else
      fail_count += 1
    end
  end
}

puts ""
puts "test suite summary:"
puts "  testcases: #{count}"
puts "  passed:    #{pass_count}"
puts "  failed:    #{fail_count}"
