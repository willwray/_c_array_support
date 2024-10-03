# ---- Get project VERSION major.minor.patch ----
# ----  extracts version from 'git describe' ----

# The git repo is tagged with e.g v1.2 version major.minor numbers only
# with the patch number extracted from 'git describe' which appends the
# number of commits since the tag (and a hash) -NUMCOMMITS-gHEXHASH
project(
  'c_array_support', 'cpp',
  default_options : ['cpp_std=c++20','warning_level=3'],
  meson_version : '>=1.0.0',
  version : run_command(find_program('git'),'-C',meson.project_source_root(),
                        'describe','--tags','--long','--abbrev=10',
                        check: true).stdout().strip()
                                    .substring(1,-12).replace('-','.')
)

# ---- meson_options.txt ----
# --- 'tests' defaults True in top-level project else False in subproject ---
TESTS = get_option('tests').disable_auto_if(meson.is_subproject()).allowed()

headers = files(
  'c_array_support/c_array_support.hpp',
  'c_array_support/util_traits.hpp',
  'c_array_support/c_array_assign.hpp',
  'c_array_support/c_array_compare.hpp',
  'c_array_support/namespace.hpp',
  'c_array_support/ALLOW_ZERO_SIZE_ARRAY.hpp',
)

install_headers(headers, subdir: 'c_array_support')

c_array_support_dep = declare_dependency(
  include_directories : include_directories('c_array_support'),
)

meson.override_dependency('c_array_support', c_array_support_dep)

import('pkgconfig').generate(
  name: 'c_array_support',
  subdirs: 'c_array_support',
  filebase: 'c_array_support',
  description: 'C++20 Concepts, traits and tools for handing C arrays.',
  url: 'https://github.com/Lemuriad/c_array_support',
)

if (TESTS)
  subdir('tests')
endif
