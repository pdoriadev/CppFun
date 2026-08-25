##############################################################
# GCC Configuration and Build Generation
##############################################################


##############################################################
# Turn on C++23 and C++ Modules with import std support
##############################################################
# Enable C++23 and module support
# https://github.com/Kitware/CMake/blob/v4.0.3/Help/dev/experimental.rst
# Must be set before project(...)
# The compiler, compiler options, and libraries must be consistent between the
# project's files and the BMI created for "import std", which is established when
# the project(...) is defined.
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_MODULE_STD ON)


set(cxx_flags
  -pthread
 # -stdlib=libstdc++                           # GNU's g++ standard library (the default)
)
string(JOIN " " CMAKE_CXX_FLAGS ${cxx_flags})


set(cxx_flags_debug
  -ggdb3 -O0
)
string(JOIN " " CMAKE_CXX_FLAGS_DEBUG ${cxx_flags_debug})


#   -fvisibility=hidden                       # See https://groups.google.com/a/chromium.org/g/chromium-dev/c/9IQH-wKdT_c
#                                             # core problem is that `inline` variables which are defined in header files will be duplicated during component builds
#                                             # Assume shared libraries are not being built and take the optimizations.  But if that assumption changes ...
set(cxx_flags_release
  -g1 -O3 -DNDEBUG                            # g1 or better required to allow stacktrace to provide names vice just address
  -flto=auto
  -fuse-linker-plugin
  -fvisibility=hidden
  -fwhole-program
)
string(JOIN " " CMAKE_CXX_FLAGS_RELEASE ${cxx_flags_release})



project(build_with_gcc LANGUAGES CXX)




##############################################################
# Prescribe compiler flags
#   Note:  Flags for C++ Modules is implicitly defined by turning
#          on C++23 and module support above
##############################################################
function(apply_compiler_flags target)
  target_compile_options(${target} PRIVATE
    $<$<CONFIG:Debug>:   ${cxx_flags_debug}>     # CMAKE_CXX_FLAGS_* are already emitted and these will cause duplicate, but
    $<$<CONFIG:Release>: ${cxx_flags_release}>   # I kept them here because some IDEs look for them, and the duplicates don't "hurt"
    ${cmake_cxx_flags}

    # We don't want the below higher-level warnings in CMAKE_CXX_FLAGS because it causes building the std BMI to emit warning messages
    -DUSING_TOMS_SUGGESTIONS=1
    -D__func__=__PRETTY_FUNCTION__

    -Wall -Wextra -pedantic
    -Wconversion
    -Wdelete-non-virtual-dtor
    -Wduplicated-branches
    -Wduplicated-cond
    -Wextra-semi
    -Wfloat-equal
    -Wformat=2
    -Winit-self
    -Wleading-whitespace=spaces
    -Wlogical-op
    -Wnoexcept
    -Wshadow
    -Wnull-dereference
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wstrict-null-sentinel
    -Wsuggest-override
    -Wswitch-default
    -Wswitch-enum
    -Woverloaded-virtual
    -Wuseless-cast
  )
#  -Wzero-as-null-pointer-constant
#  -fvisibility=hidden                  # See https://groups.google.com/a/chromium.org/g/chromium-dev/c/9IQH-wKdT_c
#                                       # core problem is that `inline` variables which are defined in header files will be duplicated during component builds
#                                       # Assume shared libraries are not being built and take the optimizations.  But if that assumption changes ...

  if(COMPLIANCE_HEADER_PATH)
    target_compile_options(${target} PRIVATE -include "${COMPLIANCE_HEADER_PATH}${COMPLIANCE_HEADER_FILE}")
  endif()
endfunction()





# Define the executable target using globbed sources
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)   # disable at commandline with cmake --compile-no-warning-as-error
set(CMAKE_LINK_WARNING_AS_ERROR    ON)   # disable at commandline with cmake --link-no-warning-as-error

add_executable(build_gcc_exe ${SRC_SOURCES})

target_compile_features(build_gcc_exe PUBLIC cxx_std_23)
apply_compiler_flags(build_gcc_exe)
target_include_directories(build_gcc_exe PRIVATE "${Project_Root}")
set_target_properties(build_gcc_exe PROPERTIES
    OUTPUT_NAME "${ProjectName}_gxx_${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${Project_Root}/bin")

# GCC 14 Release Series Changes, New Features, and Fixes
# https://gcc.gnu.org/gcc-14/changes.html
#    Runtime Library (libstdc++)
#    o  The libstdc++exp.a library now includes all the Filesystem TS symbols from the libstdc++fs.a library.
#       The experimental symbols for the C++23 std::stacktrace class are also in libstdc++exp.a, replacing
#       the libstdc++_libbacktrace.a library that GCC 13 provides. This means that -lstdc++exp is the only
#       library needed for all experimental libstdc++ features.
target_link_libraries(build_gcc_exe PRIVATE stdc++exp)


file(REAL_PATH "${CMAKE_CXX_COMPILER}" GXX_REALPATH)
get_filename_component(GXX_DIR "${GXX_REALPATH}" DIRECTORY)
get_filename_component(GCC_ROOT "${GXX_DIR}" DIRECTORY)
set(GCC_LIB64_PATH "${GCC_ROOT}/lib64")
if(EXISTS "${GCC_LIB64_PATH}")
  set_target_properties(build_gcc_exe PROPERTIES
    INSTALL_RPATH "${GCC_LIB64_PATH}"
    BUILD_RPATH "${GCC_LIB64_PATH}"
  )
else()
    message(WARNING "GCC lib64 path not found: ${GCC_LIB64_PATH}")
endif()




##############################################################
# If there are module files, define the library target and its dependencies
##############################################################
if(MODULE_INTERFACES)
  target_sources(build_gcc_exe
    PUBLIC
      FILE_SET   modules
      TYPE       CXX_MODULES
      BASE_DIRS  "${Project_Root}"
      FILES      ${MODULE_INTERFACES}
  )
endif()

add_custom_command(TARGET build_gcc_exe POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo ""    # ninja doesn't like escaped characters, so ...
  COMMAND ${CMAKE_COMMAND} -E echo ""
  COMMAND ${CMAKE_COMMAND} -E echo  "✅ Executable \\\"$<TARGET_FILE_NAME:build_gcc_exe>\\\" built successfully"
  COMMAND ${CMAKE_COMMAND} -E echo  "   Location:  \\\"$<TARGET_FILE_DIR:build_gcc_exe>\\\""
)
