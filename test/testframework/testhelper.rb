#
# = testhelper.rb: Test helper functions
#

# Generate red text
def red(text)
  "\e[31m#{text}\e[0m"
end

# Generate green text
def green(text)
  "\e[32m#{text}\e[0m"
end

# Generate a console colored FAIL string
def fail()
  red("FAIL")
end

# Generate a console colored PASS string
def pass()
  green("PASS")
end
