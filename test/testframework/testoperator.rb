#
# = testoperator.rb: Test operator class
#
# TestOperator takes an environment and a test definiton on creation. It
# provides a metohd to build a test executable and to run a test and compare
# its outputs to an excepted output

require "open3"

class TestOperator
  def initialize(environment, definition)
    @environment = environment
    @definition = definition
  end

  def compare(expect, result)
    mismatch = false

    if expect.length != result.length
      if @environment.verbose
        puts "EXPECT LINES: #{expect.length}"
        puts "RESULT LINES: #{result.length}"
      end
      mismatch = true
    end
    expect.zip(result).map do |expect, result|
      if expect != result
        if @environment.verbose
          puts "EXPECT: #{expect}"
          puts "RESULT: #{result}"
        end
        mismatch = true
      end
    end

    return mismatch
  end

  # Setup test
  def setup()
    if @environment.verbose
      puts "========== BEGIN #{@definition.name} - #{@definition.description} =========="
    end
  end

  # Build test executable using information in +environment+ and +description+
  def buildTest()
    return_value = false

    arguments = Array.new()
    arguments.concat(@environment.compiler_flags)
    arguments.concat(@definition.compiler_flags)
    arguments.concat(@environment.include_directories.map do |dir| "-I#{dir}" end)
    arguments.concat(@environment.library_directories.map do |dir| "-L#{dir}" end)
    arguments.concat(@definition.library_directories.map do |dir| "-L#{dir}" end)
    arguments.concat(@environment.sources)
    arguments.concat(@definition.source_files)
    arguments.push("-o#{@definition.executable}")
    arguments.concat(@environment.libraries.map do |library| "-l#{library}" end)
    arguments.concat(@definition.libraries.map do |library| "-l#{library}" end)

    stdin, stdout, stderr, wait_thr = Open3.popen3(@environment.compiler, *arguments)

    stdin.close

    stdout.each do |line|
      if /error/ =~ line
        if @environment.verbose
          puts line
        end
        return_value = true
      end
      if /warning/ =~ line
        if @environment.verbose
          puts line
        end
        return_value = true
      end
    end
    stdout.close

    stderr.each do |line|
      if /error/ =~ line
        if @environment.verbose
          puts line
        end
        return_value = true
      end
      if /warning/ =~ line
        if @environment.verbose
          puts line
        end
        return_value = true
      end
    end
    stderr.close

    return return_value
  end

  # Run test executable provide input through stdin and compare output on stdout, stderr and possible files
  def runTest()
    # open program
    stdin, stdout, stderr, wait_thr = Open3.popen3(@definition.executable, *@definition.arguments)

    # put input template to program
    if File.file?(@definition.stdin)
      expect_stdin = IO.readlines(@definition.stdin)
    else
      expect_stdin = [ ]
    end

    expect_stdin.each do |x| stdin.puts x end
    stdin.close

    # store program output
    result_stdout = stdout.readlines
    result_stderr = stderr.readlines
    stdout.close
    stderr.close

    # retreive exit status
    result_status = wait_thr.value.exitstatus

    # compare program stdout
    if File.file?(@definition.stdout)
      expect_stdout = IO.readlines(@definition.stdout)
    else
      expect_stdout = [ ]
    end
    mismatch_stdout = compare(expect_stdout, result_stdout)

    # compare program stderr
    if File.file?(@definition.stderr)
      expect_stderr = IO.readlines(@definition.stderr)
    else
      expect_stderr = [ ]
    end
    mismatch_stderr = compare(expect_stderr, result_stderr)

    # compare program output files
    mismatch_files = false
    @definition.files.each do |output_file, file|
      if File.file?(file)
        expect_file = IO.readlines(file)
      else
        expect_file = [ ]
      end

      if File.file?(output_file)
        result_file = IO.readlines(output_file)
      else
        result_file = [ ]
      end
      mismatch_files |= compare(expect_file, result_file)
    end

    # compare program status
    mismatch_status = @definition.status != result_status

    # debugging output
    if @environment.verbose
      if mismatch_stdout
        puts "mismatch in stdout"
      end
      if mismatch_stderr
        puts "mismatch in stderr"
      end
      if mismatch_files
        puts "mismatch in files"
      end
      if mismatch_status
        puts "mismatch in status"
      end
    end

    # check the test results
    if !mismatch_stdout && !mismatch_stderr && !mismatch_files && ! mismatch_status
      return_value = false
    else
      return_value = true
    end

    return return_value
  end

  # Cleanup after test run
  def cleanup()
    @definition.temporary_files.each do |file|
      if File.exists?(file) && File.writable?(file)
        File.delete(file)
      end
    end
    if @environment.verbose
      puts "========== END #{@definition.name} =========="
    end
  end
end
