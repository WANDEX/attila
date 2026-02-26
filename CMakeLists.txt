cmake_minimum_required(VERSION 3.29..4.1)

## Disable modules support, this is broken with clang-tidy at the moment
## github.com/cpp-best-practices/cmake_template/blob/main/CMakeLists.txt
cmake_policy(SET CMP0155 OLD)

## https://cmake.org/cmake/help/latest/policy/CMP0156.html
SET(QT_FORCE_CMP0156_TO_VALUE NEW)
cmake_policy(SET CMP0156 NEW)

## ^ POLICIES
project(
  attila
  VERSION       0.1.1
  LANGUAGES     CXX
  DESCRIPTION   "ATTILA - Advanced Task Text Interactive Analyzer"
  HOMEPAGE_URL  "https://github.com/WANDEX/attila"
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

option(ATTILA_DISABLE_TOP_PROJECT OFF)
if(PROJECT_IS_TOP_LEVEL AND NOT ATTILA_DISABLE_TOP_PROJECT)
  set(ATTILA_IS_TOP_PROJECT ON)
else()
  set(ATTILA_IS_TOP_PROJECT OFF)
endif()

## search path for cmake modules to be loaded by: include() find_package().
## NOTE: works properly only if defined at the top-level CMakeLists.txt.
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ## project modules
  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wndx/sane/cmake"
)

macro(targets_and_linking)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wndx/sane")
  include(wndx_sane_create_targets)
  ## library targets & compilation options/flags
  wndx_sane_create_targets(PFX wndx LIB attila
    CXX_STD cxx_std_${CMAKE_CXX_STANDARD}
    PHD "include/wndx/attila"
  )

  ## inherit from wndx::sane targets
  target_link_libraries(attila_core INTERFACE wndx::sane::core)
  target_link_libraries(attila_dev  INTERFACE wndx::sane::dev )
  target_link_libraries(attila_deps    PUBLIC wndx::sane::deps)
  target_link_libraries(attila_src     PUBLIC wndx::sane::src )

  ## link sources with project dependencies from the core library target
  target_link_libraries(attila_src INTERFACE wndx::attila::core)

  ## link sources with project dev interface (for the compilation flags/options)
  target_link_libraries(attila_src PRIVATE wndx::attila::dev)

  ## link sources with project dependencies
  target_link_libraries(attila_src PUBLIC wndx::attila::deps)

  if(ATTILA_BUILD_SRC OR NOT ATTILA_IS_TOP_PROJECT)
    ## find/fetch dependecies:
    find_package(fmt REQUIRED)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    find_package(QT NAMES Qt6) # => populate QT_VERSION variables!
    message(DEBUG "QT_VERSION:${QT_VERSION} OR ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
    if(${QT_VERSION} VERSION_LESS 6.3)
      message(FATAL_ERROR "Min req version of Qt is 6.3!")
    endif()
    qt_standard_project_setup()
    add_subdirectory(src/wndx/attila)
  endif()
endmacro(targets_and_linking)

if(NOT ATTILA_IS_TOP_PROJECT)
  targets_and_linking()
  return() ## finish processing of the file
endif()

## MAIN
option(ATTILA_BUILD_SRC        "whether or not src should be built"          ON)
option(ATTILA_BUILD_TESTS      "whether or not tests should be built"        OFF)
option(ATTILA_BUILD_PACKAGE    "whether or not the package should be built"  ON)
option(ATTILA_COVERAGE_ENABLE  "whether or not to enable the tests coverage" OFF)
option(ATTILA_COVERAGE_CLEAN   "clean coverage data before taking new"       ON)
option(ATTILA_INSTALL_ENABLE   "whether or not to enable the install rule"   ON)
option(ATTILA_MEMCHECK_ENABLE  "detect leaks via memcheck tool"              OFF)

## for the list of supported compilers visit:
## https://cmake.org/cmake/help/latest/prop_tgt/COMPILE_WARNING_AS_ERROR.html
option(CMAKE_COMPILE_WARNING_AS_ERROR "treat compilation warnings as errors" OFF)

## BUG? - does not work if set via option()
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)

## QT_DEPLOY_LIB_DIR defaults to the value of
## ${CMAKE_INSTALL_LIBDIR} (usually lib or lib64),
## which is provided by CMake's GNUInstallDirs module.
## To change the value of QT_DEPLOY_LIB_DIR, ensure that the project sets
## CMAKE_INSTALL_LIBDIR before the Core package is found.

## output dir is correct by default for most build setups.
## however, when building lib as a DLL, it is important to have the DLL in the same dir
## as the executable using it. Thus, we put all executables in a single /bin dir.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
## on macOS, property ignored for the linker import files
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

targets_and_linking()

if(ATTILA_COVERAGE_ENABLE)
  include(wndx_sane_coverage)
  wndx_sane_coverage(TGT_NAME code_coverage
    CLEAN ATTILA_COVERAGE_CLEAN
  )
endif(ATTILA_COVERAGE_ENABLE)

if(ATTILA_BUILD_TESTS)
  add_subdirectory(tests/units)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # target_link_libraries(tests_units PRIVATE debug -fsanitize=thread) # MutExc
    # target_link_libraries(tests_units PRIVATE debug -fsanitize=address)
    # target_link_libraries(tests_units PRIVATE debug -fsanitize=leak)
    # target_link_libraries(tests_units PRIVATE debug -fsanitize=undefined)
  endif()
endif(ATTILA_BUILD_TESTS)

if(ATTILA_BUILD_PACKAGE)
  include(wndx_sane_package)
  wndx_sane_package(SUFFIX src FILES
    cmake/ include/ scripts/ src/ tests/ CMakeLists.txt COPYING.LESSER COPYING README.md
  )
endif(ATTILA_BUILD_PACKAGE)

if(ATTILA_MEMCHECK_ENABLE)
  include(wndx_sane_memcheck)
  wndx_sane_memcheck(TGT_NAME memcheck
    TGT_EXEC tests_units --gtest_brief=1
    # VALGRIND_OPTS --read-var-info=yes --show-leak-kinds=definite --track-origins=yes
    # DRMEMORY_OPTS -brief -no_crash_at_error
    # EXIT_ON_FIRST_ERROR
    # FORCE_DRMEMORY
  )
endif(ATTILA_MEMCHECK_ENABLE)

if(ATTILA_INSTALL_ENABLE)
  ## override install path via CMAKE_INSTALL_PREFIX or
  ## cmake --install . --prefix "install_prefix"
  include(GNUInstallDirs)
  if(FALSE)
    cmake_path(SET lib_path NORMALIZE "${CMAKE_CURRENT_BINARY_DIR}/deploy/lib")
    cmake_path(SET libsys_path NORMALIZE "${lib_path}/libsys")
    message(DEBUG "libsys_path: ${libsys_path}")
    file(MAKE_DIRECTORY "${libsys_path}")
  endif()
  include(wndx_sane_install)
  wndx_sane_install(
    TARGETS attila_app
    PHD "include/wndx/attila"
    PATTERNS "*.hpp"
    NAMESPACE wndx::
    RUNTIME_DEPENDENCIES
  ) # ^--install required set of libs vs v--install max amount of system libs.
  if(DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP AND
         NOT CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP)
    ## BUG? - not respects CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP
    ## for some reason, no matter what, it installs more system libs than used.
    if(${QT_VERSION} VERSION_GREATER_EQUAL 6.3)
      qt_generate_deploy_app_script(
        TARGET attila_app
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
      )
      install(SCRIPT ${deploy_script})
    else()
      message(FATAL_ERROR "DEPLOY min req version of Qt is 6.3!")
    endif()
  endif()
endif(ATTILA_INSTALL_ENABLE)
