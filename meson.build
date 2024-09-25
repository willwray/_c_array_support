project('c_array_support', 'cpp',
  default_options : ['cpp_std=c++20','warning_level=3'],
  version : 'v1.1'
)

headers = files('c_array_support.hpp', 'util_traits.hpp'
                ,'c_array_assign.hpp', 'c_array_compare.hpp'
                ,'namespace.hpp','ALLOW_ZERO_SIZE_ARRAY.hpp')

install_headers(headers, subdir: 'c_array_support')

c_array_support_dep = declare_dependency(
  include_directories : include_directories('.'),
)

if meson.version().version_compare('>=0.54.0')
  meson.override_dependency('c_array_support', c_array_support_dep)
endif

import('pkgconfig').generate(
  name: 'c_array_support',
  subdirs: 'c_array_support',
  filebase: 'c_array_support',
  description: 'C++20 Concepts, traits and tools for handing C arrays.',
  url: 'https://github.com/Lemuriad/c_array_support',
)

if get_option('tests').disable_auto_if(meson.is_subproject()).allowed()
  subdir('tests')
endif
