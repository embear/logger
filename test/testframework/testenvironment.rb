#
# = testenvironment.rb: Test environment class
#
# TestEnvironment stores all environment specific settings to run tests.

class TestEnvironment
  attr_reader :include_directories
  attr_reader :source_directories
  attr_reader :sources
  attr_reader :library_directories
  attr_reader :libraries
  attr_reader :compiler
  attr_reader :compiler_flags
  attr_reader :verbose

  def initialize()
    @compiler            = "gcc"
    @compiler_flags      = [ "-std=c99", "-Wall", "-pedantic" ]
    @include_directories = [ "../include" ]
    @sources             = [ "../src/logger.c" ]
    @library_directories = Array.new(0, String.new())
    @libraries           = Array.new(0, String.new())
    @verbose             = true
  end
end
