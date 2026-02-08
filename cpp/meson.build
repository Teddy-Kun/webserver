project(
  'webserver_cpp',
  'cpp',
  version: '0.1',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    # 'werror=true',
  ],
)

sources = ['src/socket_linux.cpp', 'src/error.cpp', 'src/html_files.cpp', 'src/main.cpp']

# Define your executable
executable('webserver_cpp', sources, install: true)
