#
# = testdefinition.rb: Test definition class
#
# TestDefinition stores all test specific settings to run tests.

class TestDefinition
  attr_accessor :name
  attr_accessor :description
  attr_accessor :compiler_flags
  attr_accessor :source_files
  attr_accessor :library_directories
  attr_accessor :libraries
  attr_accessor :executable
  attr_accessor :arguments
  attr_accessor :stdin
  attr_accessor :stdout
  attr_accessor :stderr
  attr_accessor :files
  attr_accessor :status
  attr_accessor :temporary_files

  def initialize()
    @name                = String.new()
    @description         = String.new()
    @compiler_flags      = String.new()
    @source_files        = Array.new(0, String.new())
    @library_directories = Array.new(0, String.new())
    @libraries           = Array.new(0, String.new())
    @executable          = String.new()
    @arguments           = Array.new(0, String.new)
    @stdin               = String.new()
    @stdout              = String.new()
    @stderr              = String.new()
    @files               = Array.new(0, Array.new(0, String.new))
    @status              = 0
    @temporary_files     = Array.new(0, String.new())
  end
end
