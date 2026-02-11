##############################################################
# CLANG Configuration and Build Generation
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
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_MODULE_STD ON)

set(CMAKE_AR "/usr/bin/llvm-ar")
set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")

set(cxx_flags
  -pthread
  -stdlib=libc++                              # LLVM's clang standard library (not the default)
)
string(JOIN " " CMAKE_CXX_FLAGS ${cxx_flags})


set(cxx_flags_debug
  -g3 -O0
)
string(JOIN " " CMAKE_CXX_FLAGS_DEBUG ${cxx_flags_debug})


#   -fvisibility=hidden                       # See https://groups.google.com/a/chromium.org/g/chromium-dev/c/9IQH-wKdT_c
#                                             # core problem is that `inline` variables which are defined in header files will be duplicated during component builds
#                                             # Assume shared libraries are not being built and take the optimizations.  But if that assumption changes ...
set(cxx_flags_release
  -g1 -O3 -DNDEBUG                            # g1 or better required to allow stacktrace to provide names vice just address
  -flto=auto
  -fvisibility=hidden
)
string(JOIN " " CMAKE_CXX_FLAGS_RELEASE ${cxx_flags_release})



project(build_with_clang LANGUAGES CXX)




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

    -Weverything
    -Wno-comma
    -Wno-unused-template
    -Wno-sign-conversion
    -Wno-exit-time-destructors
    -Wno-global-constructors
    -Wno-missing-prototypes
    -Wno-weak-vtables
    -Wno-padded
    -Wno-double-promotion
    -Wno-c++98-compat-pedantic
    -Wno-c++11-compat-pedantic
    -Wno-c++14-compat-pedantic
    -Wno-c++17-compat-pedantic
    -Wno-c++20-compat-pedantic
    -Wno-unsafe-buffer-usage
    -Wno-ctad-maybe-unsupported
#   -Wno-unique-object-duplication   # introduced in clang-21
#   -Wno-zero-as-null-pointer-constant
  )

  if(COMPLIANCE_HEADER_PATH)
    target_compile_options(${target} PRIVATE -include "${COMPLIANCE_HEADER_PATH}${COMPLIANCE_HEADER_FILE}")
  endif()
endfunction()





# Define the executable target using globbed sources
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)   # disable at commandline with cmake --compile-no-warning-as-error
set(CMAKE_LINK_WARNING_AS_ERROR    ON)   # disable at commandline with cmake --link-no-warning-as-error

add_executable(build_clang_exe ${SRC_SOURCES})
target_compile_features(build_clang_exe PUBLIC cxx_std_23)
apply_compiler_flags(build_clang_exe)
target_include_directories(build_clang_exe PRIVATE "${Project_Root}")
set_target_properties(build_clang_exe PROPERTIES
    OUTPUT_NAME "${ProjectName}_clangxx_${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${Project_Root}/bin")





##############################################################
# If there are module files, define the library target and its dependencies
##############################################################
  if(MODULE_INTERFACES)
  target_sources(build_clang_exe
    PUBLIC
      FILE_SET   modules
      TYPE       CXX_MODULES
      BASE_DIRS  "${Project_Root}"
      FILES      ${MODULE_INTERFACES}
  )
endif()

add_custom_command(TARGET build_clang_exe POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo ""    # ninja doesn't like escaped characters, so ...
  COMMAND ${CMAKE_COMMAND} -E echo ""
  COMMAND ${CMAKE_COMMAND} -E echo  "✅ Executable \\\"$<TARGET_FILE_NAME:build_clang_exe>\\\" built successfully"
  COMMAND ${CMAKE_COMMAND} -E echo  "   Location:  \\\"$<TARGET_FILE_DIR:build_clang_exe>\\\""
)
