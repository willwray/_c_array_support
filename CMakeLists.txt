cmake_minimum_required(VERSION 3.23...3.30)

# ---- Get project VERSION major.minor.patch ----
# ----  extracts version from 'git describe' ----

# The git repo is tagged with e.g v1.2 version major.minor numbers only
# with the patch number extracted from 'git describe' which appends the
# number of commits since the tag (and a hash) -NUMCOMMITS-gHEXHASH
find_package(Git)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long
    OUTPUT_VARIABLE GIT_DESCRIBE_TAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
string(REGEX REPLACE "v([0-9]*)\.([0-9]*)-([0-9]*)-g[a-f0-9]*$"
       "\\1.\\2.\\3" VERSION_NUMBERS_FROM_GIT ${GIT_DESCRIBE_TAGS})

project(
    c_array_support
    VERSION ${VERSION_NUMBERS_FROM_GIT}
    DESCRIPTION "Concepts, traits and utils for handing C arrays. C++20"
    HOMEPAGE_URL "https://github.com/lemuriad/c_array_support"
    LANGUAGES CXX
)

option(C_ARRAY_SUPPORT_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})

# ---- Declare library ----

add_library(c_array_support INTERFACE)
add_library(c_array::support ALIAS c_array_support)

target_compile_features(c_array_support INTERFACE cxx_std_20)

set_property(
    TARGET c_array_support PROPERTY
    EXPORT_NAME c_array_support
)

target_sources(
  c_array_support
  INTERFACE
    FILE_SET api
    TYPE HEADERS
    BASE_DIRS c_array_support
    FILES
      c_array_support/c_array_support.hpp
      c_array_support/util_traits.hpp
      c_array_support/c_array_assign.hpp
      c_array_support/c_array_compare.hpp
      c_array_support/namespace.hpp
      c_array_support/ALLOW_ZERO_SIZE_ARRAY.hpp
)

# ---- Install rules ----

if (NOT CMAKE_SKIP_INSTALL_RULES)
  if (PROJECT_IS_TOP_LEVEL)
    set(
        CMAKE_INSTALL_INCLUDEDIR "include/c_array_support-${PROJECT_VERSION}"
        CACHE STRING ""
    )
    set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
  endif()
  
  # Project is configured with no languages, so tell GNUInstallDirs the lib dir
  set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")
  
  include(CMakePackageConfigHelpers)
  include(GNUInstallDirs)
  
  # find_package(<package>) call for consumers to find this project
  set(package c_array_support)
  
  install(
      TARGETS c_array_support
      EXPORT c_array_supportTargets
      FILE_SET api
  )

  write_basic_package_version_file(
      "${package}ConfigVersion.cmake"
      COMPATIBILITY SameMajorVersion
      ARCH_INDEPENDENT
  )

endif()

# ---- Developer mode ----

if (C_ARRAY_SUPPORT_TESTS)
include(CTest)
  if (BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()
