#!/usr/bin/env ruby

require "pathname"
require "./testframework/testhelper"
require "./testframework/testenvironment"
require "./testframework/testdefinition"
require "./testframework/testoperator"

environment = TestEnvironment.new()
definition = TestDefinition.new()

pathname = Pathname.new(__FILE__).expand_path

definition.name                = pathname.basename.sub_ext("")
definition.description         = "level set to LOGGER_WARNING - only log messages with level above or equal to WARNING will be shown"
definition.compiler_flags      = [ "-DLOGGER_ENABLE" ]
definition.library_directories = [ ]
definition.libraries           = [ ]
definition.source_files        = [ "./#{definition.name}.c" ]
definition.executable          = "./#{definition.name}"
definition.arguments           = [ ]
definition.stdin               = ""
definition.stdout              = "#{definition.name}.template_stdout"
definition.stderr              = ""
definition.files               = [ [ "", ""] ]
definition.status              = 0
definition.temporary_files     = [ "#{definition.executable}" ]

operator = TestOperator.new(environment, definition)

if operator.buildTest()
  puts "#{definition.name} ... #{fail()} (build)"
  return_value = 1
else
  if operator.runTest()
    puts "#{definition.name} ... #{fail()} (behavior)"
    return_value = 1
  else
    puts "#{definition.name} ... #{pass()}"
    return_value = 0
  end
end

operator.cleanup()

exit return_value
