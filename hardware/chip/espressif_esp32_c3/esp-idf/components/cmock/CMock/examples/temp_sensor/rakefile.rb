HERE = __dir__ + '/'

require 'rake'
require 'rake/clean'
require 'rake/testtask'
require './rakefile_helper'

include RakefileHelpers

REQUIRED_DIRS = ['./build', './build/mocks'].freeze
REQUIRED_DIRS.each do |v|
  directory v
end

# Load default configuration, for now
DEFAULT_CONFIG_FILE = 'gcc.yml'.freeze
configure_toolchain(DEFAULT_CONFIG_FILE)

task :unit do
  run_tests(unit_test_files)
end

desc 'Generate test summary'
task :summary do
  report_summary
end

desc 'Build and test Unity'
task :all => %i[clean unit summary]
task :default => REQUIRED_DIRS + %i[clobber all]
task :ci => [:default]
task :cruise => [:default]

desc 'Load configuration'
task :config, :config_file do |_t, args|
  configure_toolchain(args[:config_file])
end

desc 'Return error on Failures'
task :strict do
  $return_error_on_failures = true
end
