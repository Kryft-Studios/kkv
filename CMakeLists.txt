
cmake_minimum_required(VERSION 3.21)

# Load the shared project configuration before selecting a toolchain and
# declaring the project version.
set(CONFIGENV "${CMAKE_CURRENT_SOURCE_DIR}/project-config.env")
if(EXISTS "${CONFIGENV}")
  file(STRINGS "${CONFIGENV}" CEF)
  foreach(LINE IN LISTS CEF)
    if(LINE MATCHES "^([^#=]+)=(.*)$")
      string(STRIP "${CMAKE_MATCH_1}" KEY)
      string(STRIP "${CMAKE_MATCH_2}" VALUE)
      set("${KEY}" "${VALUE}")
    endif()
  endforeach()
endif()

if(NOT DEFINED kkv_ver OR "${kkv_ver}" STREQUAL "")
  message(FATAL_ERROR
    "The project version is not set. Define kkv_ver in project-config.env."
  )
endif()

# ============================================================
# Conan toolchain selection
# ============================================================

if(CMAKE_TOOLCHAIN_FILE)
  get_filename_component(
    _KKV_SELECTED_TOOLCHAIN
    "${CMAKE_TOOLCHAIN_FILE}"
    ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(NOT EXISTS "${_KKV_SELECTED_TOOLCHAIN}")
    message(FATAL_ERROR
      "The selected CMAKE_TOOLCHAIN_FILE does not exist: "
      "${_KKV_SELECTED_TOOLCHAIN}"
    )
  endif()
endif()

if(DEFINED KKV_CONAN_OUTPUT_DIR AND NOT "${KKV_CONAN_OUTPUT_DIR}" STREQUAL "")
  set(_KKV_CONAN_DIR "${KKV_CONAN_OUTPUT_DIR}")
  set(_KKV_USE_CONAN_TOOLCHAIN ON)
elseif(CMAKE_TOOLCHAIN_FILE)
  get_filename_component(_KKV_SELECTED_TOOLCHAIN_NAME
    "${_KKV_SELECTED_TOOLCHAIN}" NAME
  )
  if(_KKV_SELECTED_TOOLCHAIN_NAME STREQUAL "conan_toolchain.cmake")
    set(_KKV_USE_CONAN_TOOLCHAIN ON)
    set(_KKV_EXPECTED_CONAN_TOOLCHAIN "${_KKV_SELECTED_TOOLCHAIN}")
    get_filename_component(_KKV_CONAN_GENERATORS_DIR
      "${_KKV_SELECTED_TOOLCHAIN}" DIRECTORY
    )
  else()
    set(_KKV_USE_CONAN_TOOLCHAIN OFF)
  endif()
else()
  set(_KKV_USE_CONAN_TOOLCHAIN ON)
  if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(_KKV_CONAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build-wasm/conan")
  else()
    set(_KKV_CONAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/conan")
  endif()
endif()

if(_KKV_USE_CONAN_TOOLCHAIN AND NOT _KKV_EXPECTED_CONAN_TOOLCHAIN)
  file(GLOB _KKV_CONAN_LAYOUT_TOOLCHAINS LIST_DIRECTORIES FALSE
    "${_KKV_CONAN_DIR}/build/*/generators/conan_toolchain.cmake"
  )
  list(LENGTH _KKV_CONAN_LAYOUT_TOOLCHAINS _KKV_CONAN_LAYOUT_TOOLCHAIN_COUNT)
  if(CMAKE_BUILD_TYPE)
    foreach(_KKV_CANDIDATE IN LISTS _KKV_CONAN_LAYOUT_TOOLCHAINS)
      get_filename_component(_KKV_CANDIDATE_GENERATORS_DIR
        "${_KKV_CANDIDATE}" DIRECTORY
      )
      get_filename_component(_KKV_CANDIDATE_CONFIG_DIR
        "${_KKV_CANDIDATE_GENERATORS_DIR}" DIRECTORY
      )
      get_filename_component(_KKV_CANDIDATE_BUILD_TYPE
        "${_KKV_CANDIDATE_CONFIG_DIR}" NAME
      )
      if("${_KKV_CANDIDATE_BUILD_TYPE}" STREQUAL "${CMAKE_BUILD_TYPE}")
        set(_KKV_EXPECTED_CONAN_TOOLCHAIN "${_KKV_CANDIDATE}")
        set(_KKV_CONAN_GENERATORS_DIR "${_KKV_CANDIDATE_GENERATORS_DIR}")
      endif()
    endforeach()
  endif()

  if(NOT _KKV_EXPECTED_CONAN_TOOLCHAIN
     AND _KKV_CONAN_LAYOUT_TOOLCHAIN_COUNT EQUAL 1)
    list(GET _KKV_CONAN_LAYOUT_TOOLCHAINS 0 _KKV_EXPECTED_CONAN_TOOLCHAIN)
    get_filename_component(
      _KKV_CONAN_GENERATORS_DIR
      "${_KKV_EXPECTED_CONAN_TOOLCHAIN}"
      DIRECTORY
    )
  elseif(NOT _KKV_EXPECTED_CONAN_TOOLCHAIN
         AND _KKV_CONAN_LAYOUT_TOOLCHAIN_COUNT GREATER 1)
    message(FATAL_ERROR
      "Multiple Conan toolchains were found under '${_KKV_CONAN_DIR}/build', "
      "but none matches CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'. "
      "Use a dedicated Conan output directory and matching Conan build type."
    )
  elseif(NOT _KKV_EXPECTED_CONAN_TOOLCHAIN)
    set(_KKV_CONAN_GENERATORS_DIR "${_KKV_CONAN_DIR}")
    set(_KKV_EXPECTED_CONAN_TOOLCHAIN
      "${_KKV_CONAN_GENERATORS_DIR}/conan_toolchain.cmake"
    )
  endif()

  get_filename_component(
    _KKV_EXPECTED_CONAN_TOOLCHAIN
    "${_KKV_EXPECTED_CONAN_TOOLCHAIN}"
    ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
  )
endif()

if(_KKV_USE_CONAN_TOOLCHAIN)
  if(CMAKE_TOOLCHAIN_FILE
     AND NOT "${_KKV_SELECTED_TOOLCHAIN}" STREQUAL "${_KKV_EXPECTED_CONAN_TOOLCHAIN}")
    message(FATAL_ERROR
      "CMAKE_TOOLCHAIN_FILE selects '${_KKV_SELECTED_TOOLCHAIN}', but "
      "KKV_CONAN_OUTPUT_DIR selects '${_KKV_EXPECTED_CONAN_TOOLCHAIN}'. "
      "Use a fresh build directory when changing toolchains."
    )
  elseif(NOT CMAKE_TOOLCHAIN_FILE
         AND NOT EXISTS "${_KKV_EXPECTED_CONAN_TOOLCHAIN}")
    message(FATAL_ERROR
      "The Conan toolchain was not found at '${_KKV_EXPECTED_CONAN_TOOLCHAIN}'. "
      "Run Conan install for this build directory before configuring CMake."
    )
  else()
    set(CMAKE_TOOLCHAIN_FILE
      "${_KKV_EXPECTED_CONAN_TOOLCHAIN}"
      CACHE FILEPATH "Conan toolchain file"
    )
  endif()
endif()

project(kkv VERSION "${kkv_ver}" LANGUAGES CXX)

include(GNUInstallDirs)
include(CTest)
include(CMakePackageConfigHelpers)

if(_KKV_USE_CONAN_TOOLCHAIN)
  list(PREPEND CMAKE_PREFIX_PATH "${_KKV_CONAN_GENERATORS_DIR}")
  set(rang_DIR "${_KKV_CONAN_GENERATORS_DIR}")
endif()

# ============================================================
# Project configuration
# ============================================================

set(PROJECT_DEPENDENCIES)
set(PROJECT_DEPENDENCY_CONFIG_LINES)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
  "src/*.cpp"
  "src/*.cc"
  "src/*.cxx"
)
option(KKV_BUILD_DOCUMENTATION "Build documentation" ON)
option(KKV_BUILD_FORMAT_TARGET "Enable formatting target" ON)

# ============================================================
# Conan dependencies 
# ============================================================

find_package(rang CONFIG REQUIRED)


# ============================================================
# Native C++ library
# ============================================================

add_library(kkv STATIC ${PROJECT_SOURCES})

target_include_directories(kkv
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

target_compile_features(kkv PUBLIC cxx_std_20)

# Link rang privately if KKV implementation files use it.
# Verify the target name exported by the Conan recipe.
target_link_libraries(kkv PRIVATE rang::rang)

if(MSVC)
  target_compile_options(kkv PRIVATE
    /W4
    /WX
  )
else()
  target_compile_options(kkv PRIVATE
    -Wall
    -Wextra
    -Wwarn
    -O2
  )

  # Sanitizers are for native builds, not WebAssembly.
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_compile_options(kkv PRIVATE
      -fsanitize=address
      -fsanitize=undefined
    )
  endif()
endif()

# ============================================================
# Installation and CMake package export
# ============================================================

install(TARGETS kkv EXPORT kkvTargets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT kkvTargets
  NAMESPACE kkv::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kkv"
)

string(JOIN "\n" PROJECT_DEPENDENCY_CONFIG
  ${PROJECT_DEPENDENCY_CONFIG_LINES}
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/toolchain/cmake/kkvConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/kkvConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/toolchain/cmake/kkv"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/kkvConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/kkvConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/kkvConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/kkv"
)

# ============================================================
# Tests
# ============================================================

if(BUILD_TESTING AND NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  include(FetchContent)

  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
    GIT_SHALLOW TRUE
  )

  FetchContent_MakeAvailable(doctest)

  add_executable(kkv_tests
    "tests/test.cpp"
  )

  target_link_libraries(kkv_tests PRIVATE
    kkv
    doctest::doctest
  )

  if(NOT MSVC)
    target_link_options(kkv_tests PRIVATE
      -fsanitize=address
      -fsanitize=undefined
    )
  endif()

  add_test(
    NAME kkv_tests
    COMMAND kkv_tests
  )

  add_custom_target(check-test
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    DEPENDS kkv_tests
    USES_TERMINAL
  )

  add_custom_target(sanitizer-tests
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    DEPENDS kkv_tests
    USES_TERMINAL
  )
endif()

# ============================================================
# Examples (native builds only)
# ============================================================

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  file(GLOB_RECURSE PROJECT_EXAMPLES CONFIGURE_DEPENDS
    "examples/*.cpp"
  )

  foreach(example IN LISTS PROJECT_EXAMPLES)
    get_filename_component(example_name "${example}" NAME_WE)

    add_executable("${example_name}" "${example}")
    target_link_libraries("${example_name}" PRIVATE kkv)
  endforeach()
endif()

# ============================================================
# Documentation
# ============================================================
if (KKV_BUILD_DOCUMENTATION)
find_package(Doxygen REQUIRED)

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_GENERATE_XML NO)
set(DOXYGEN_GENERATE_MAN NO)
set(DOXYGEN_GENERATE_RTF NO)
set(DOXYGEN_GENERATE_DOCBOOK NO)
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")

doxygen_add_docs(documentation
  "include"
  "src"
)
endif()
# ============================================================
# Formatting
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(KKV_BUILD_FORMAT_TARGET)
find_program(CLANG_FORMAT_EXECUTABLE clang-format REQUIRED)

file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
  "src/*.cpp"
  "src/*.cc"
  "src/*.cxx"
  "include/*.hpp"
  "include/*.h"
  "emscripten/*.cpp"
)

if(FORMAT_SOURCES)
  add_custom_target(format
    COMMAND "${CLANG_FORMAT_EXECUTABLE}" -i ${FORMAT_SOURCES}
    VERBATIM
  )
endif()
endif()
# ============================================================
# Python bindings
# ============================================================
option(KKV_BUILD_PYTHON "Build Python bindings" OFF)

if(KKV_BUILD_PYTHON AND CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  message(FATAL_ERROR
    "KKV_BUILD_PYTHON is not supported for Emscripten builds."
  )
endif()

if(KKV_BUILD_PYTHON)
  find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE _KKV_PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _KKV_PYBIND11_RESULT
    ERROR_VARIABLE _KKV_PYBIND11_ERROR
  )

  if(NOT "${_KKV_PYBIND11_RESULT}" STREQUAL "0"
     OR NOT IS_DIRECTORY "${_KKV_PYBIND11_CMAKE_DIR}")
    message(FATAL_ERROR
      "pybind11 is not installed for ${Python_EXECUTABLE}. "
      "Install it with: \"${Python_EXECUTABLE}\" -m pip install pybind11. "
      "CMake package lookup returned: '${_KKV_PYBIND11_CMAKE_DIR}'. "
      "${_KKV_PYBIND11_ERROR}"
    )
  endif()

  set(pybind11_DIR "${_KKV_PYBIND11_CMAKE_DIR}")
  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(_kkv
    "python/bindings.cpp"
  )

  target_link_libraries(_kkv PRIVATE kkv)
  target_compile_features(_kkv PRIVATE cxx_std_20)

  install(TARGETS _kkv
    LIBRARY DESTINATION "."
    RUNTIME DESTINATION "."
  )
endif()
# ============================================================
# Emscripten / WebAssembly
# ============================================================

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  file(GLOB_RECURSE EMSCRIPTEN_SOURCES CONFIGURE_DEPENDS
    "emscripten/*.cpp"
    "emscripten/*.cc"
    "emscripten/*.cxx"
  )

  file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/em")

  add_executable(kkv_wasm
    ${PROJECT_SOURCES}
    ${EMSCRIPTEN_SOURCES}
  )
  if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
target_link_libraries(kkv_wasm PRIVATE rang::rang)
endif()

  target_include_directories(kkv_wasm PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/private"
  )

  target_compile_features(kkv_wasm PRIVATE cxx_std_20)

  target_compile_options(kkv_wasm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
  )

  set_target_properties(kkv_wasm PROPERTIES
    OUTPUT_NAME "kkv"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/em"
  )

  target_link_options(kkv_wasm PRIVATE
    "-lembind"
    "--no-entry"
    "--emit-tsd=${CMAKE_SOURCE_DIR}/em/generated.d.ts"
    "-sMODULARIZE=1"
    "-sEXPORT_ES6=1"
    "-sEXPORT_NAME=KKV"
    "-sENVIRONMENT=web,node"
  )
endif()
