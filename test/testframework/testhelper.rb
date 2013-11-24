#
# = testhelper.rb: Test helper functions
#

# Generate a console colored FAIL string
def fail()
  "\e[31mFAIL\e[0m"
end

# Generate a console colored PASS string
def pass()
  "\e[32mPASS\e[0m"
end
