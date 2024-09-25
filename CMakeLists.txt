cmake_minimum_required(VERSION 3.23) # FILE_SET

project(
    c_array_support
    VERSION 1.1.0
    DESCRIPTION "Concepts, traits and utils for handing C arrays. C++20"
    HOMEPAGE_URL "https://github.com/lemuriad/c_array_support"
    LANGUAGES NONE # Header-only library so no need to set languages
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
    BASE_DIRS .
    FILES
      c_array_support.hpp
      util_traits.hpp
      c_array_assign.hpp
      c_array_compare.hpp
      namespace.hpp
      ALLOW_ZERO_SIZE_ARRAY.hpp
)

# ---- Install rules ----

if (NOT CMAKE_SKIP_INSTALL_RULES)
  if (PROJECT_IS_TOP_LEVEL)
    set(
        CMAKE_INSTALL_INCLUDEDIR "c_array_support-${PROJECT_VERSION}"
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



