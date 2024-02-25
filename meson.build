project(
  'vms',
  'c',
  default_options : ['c_std=c17', 'warning_level=3'],
)
add_global_arguments('-D_DEFAULT_SOURCE', language : 'c')

include_directories('/usr/local/include')

executable(
  'c_program',
  'src/c_program.c',
)

executable(
  'pipe_test',
  'src/main.c',
)