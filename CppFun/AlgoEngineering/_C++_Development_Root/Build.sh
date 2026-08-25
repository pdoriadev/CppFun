#!/usr/bin/env bash
clear
# mkdir -p .build
# exec 1> >(tee .build/build.log)   2>&1

# Script file to configure and compile all C++ source files in or under the
# current directory.  This has been used in the OpenSUSE and Ubuntu
# environments with the GCC and Clang compilers and linkers

versionID="26.01.10"
echo "${0##*/} version ${versionID}"

#  See Parameter Expansion section of Bash man page for "%%"'s' Remove matching suffix pattern
#  behavior (https://linux.die.net/man/1/bash)
#
#  ${parameter,,}         ==> lower case
#  ${parameter^^}         ==> upper case
#  ${parameter%word}      ==> Remove shortest matching suffix pattern
#  ${parameter%%word}     ==> Remove longest  matching suffix pattern
#  ${parameter#word}      ==> Remove shortest matching prefix pattern
#  ${parameter##word}     ==> Remove longest  matching prefix pattern

# A common mistake is to place your source code in the same directory as Build.sh, or to copy/move Build.sh
# into the directory with your source code.  Let's usurp some questions by providing a quick reminder ...
script=$( realpath "${0}" )                                         # I know, "script" is also set the same way (far) below but I decided that's okay (for now)'
if [[ "${script%/*}" = "${PWD}" ]]; then
  echo -e "Are you sure you're in the correct directory?\n"            \
          "Build.sh should not be in your current working directory\n" \
          "You can, however, create a symbolic link to Build.sh in your current working directory if that helps,\n" \
          "or just use the relative path, for example: ../Build.sh"
  exit
fi

CMakeOptions=()
NinjaOptions=()
ToolChains=()
while [[ $# -gt 0 && "${1}" = -* ]]; do  # of course options must not have spaces
  case "${1,,}" in
    -debug)
      CMakeOptions+=( "-DCMAKE_BUILD_TYPE=Debug" )
      ;;
    -release)
      CMakeOptions+=( "-DCMAKE_BUILD_TYPE=Release" )
      ;;
    -warn)
      CMakeOptions+=( "--compile-no-warning-as-error"  "--link-no-warning-as-error" )
      ;;
    -clean)
      NinjaOptions+=( "--target clean" )
      ;;
    -cleanfirst)
      NinjaOptions+=( "--clean-first" )
      ;;
    -v)
      NinjaOptions+=( "-v" )
      ;;
    --clang)
      ToolChains+=( clang )
      ;;
    --gcc)
      ToolChains+=( gcc )
      ;;
    *)
      CMakeOptions+=( "${1}" )
      ;;
  esac
  shift
done
executableFileName="${1:-project}"


### Check GCC and Clang versions, and upgrade if needed - Usually a one-time occurrence
###  The procedure could be removed once your Linux system is configured out-of-the-box with correct versions
CheckVersion()
{
  buffer=( $(g++ --version ) )
  gccVersion="${buffer[3]}"

  # This is pretty fragile, but version is presented in different locations depending on version and OS
  buffer=( $(clang++ --version ) )
  if   [[ ${buffer[1],,} = "version" ]]; then  clangVersion="${buffer[2]}"
  elif [[ ${buffer[2],,} = "version" ]]; then  clangVersion="${buffer[3]}"
  fi

  buffer=( $(cmake --version) )
  cmakeVersion="${buffer[2]}"

  buffer=( $(ninja --version) )
  ninjaVersion="${buffer[0]}"


  RequiredGccVersion=15.2
  RequiredClangVersion=21.0
  RequiredCMakeVersion=4.2.1
  RequiredNinjaVersion=1.13.2



  ## Check minimum tool versions
  if [[ "${gccVersion,,}"   < "${RequiredGccVersion,,}"    \
    ||  "${clangVersion,,}" < "${RequiredClangVersion,,}"  \
    ||  "${cmakeVersion,,}" < "${RequiredCMakeVersion,,}"  \
    ||  "${ninjaVersion,,}" < "${RequiredNinjaVersion,,}"  \
    ||  "${CMakeOptions[*]}" = *---force_update* ]]; then
    ## Minimum tool versions not found, let's see if we can install them
    echo -e "\nBasic tools are required, but either not found or found with a version not meeting the minimum required"
    printf "%-10.10s | %-10.10s | %-10.10s\n%-10.10s | %-10.10s | %-10.10s\n%s\n"  "" "Current" "Required" "Tool" "Version" "Version" "===================================="
    printf "%-10.10s | %-10.10s | %-10.10s\n"      "GCC"    "${gccVersion:-not found}"    "${RequiredGccVersion}"
    printf "%-10.10s | %-10.10s | %-10.10s\n"      "Clang"  "${clangVersion:-not found}"  "${RequiredClangVersion}"
    printf "%-10.10s | %-10.10s | %-10.10s\n"      "CMake"  "${cmakeVersion:-not found}"  "${RequiredCMakeVersion}"
    printf "%-10.10s | %-10.10s | %-10.10s\n\n\n"  "Ninja"  "${ninjaVersion:-not found}"  "${RequiredNinjaVersion}"

    ## Get distribution
    Distribution="$(lsb_release -is)"

    if [[ "${Distribution,,}" = "ubuntu" ]]; then

      Release="$(lsb_release -sr)"

      ## Ubuntu 24.04 support with some basic backward support for Ubuntu 22.04
      if [[ ( "${Release,,}" = "24.04"  ||  "${Release,,}" = "22.04" )  ]]; then

        echo -e "\nWould you like to upgrade now?  This may require a system reboot. (yes or no)"
        read shall_I_upgrade

        if [[ "${shall_I_upgrade,,}x" = "yesx"  ||  "${shall_I_upgrade,,}x" = "yx" ]]; then
          echo -e "\nUpgrading could be a long and extensive process.\n\n ****  Make sure you have backups of all your data!\n\n Are you really sure?"
          read shall_I_upgrade
          if [[ "${shall_I_upgrade,,}x" = "yesx"  ||  "${shall_I_upgrade,,}x" = "yx" ]]; then

            echo -e "Yes.  Okay, attempting to upgrade now.  The upgrade requires super user privileges and you may be prompted for your password.\n"

            if [[ "${EUID:-$(id -u)}" -eq 0 ]]; then SUDO=''; else SUDO='/usr/bin/sudo'; fi
            ${SUDO} /bin/bash -svx -- <<-EOF   # the "-" after the "<<" allows leading tabs (but not spaces), a quoted EOF would mean literal input, i.e., do not substitute parameters
				InstallGccAlternative()
				{
				  update-alternatives  --install /usr/bin/gcc         gcc         "\${2}"/bin/gcc-\${1} \${1}  \
				                       --slave   /usr/bin/g++         g++         "\${2}"/bin/g++-\${1}        \
				                       --slave   /usr/bin/gcc-ar      gcc-ar      "\${2}"/bin/gcc-ar-\${1}     \
				                       --slave   /usr/bin/gcc-nm      gcc-nm      "\${2}"/bin/gcc-nm-\${1}     \
				                       --slave   /usr/bin/gcc-ranlib  gcc-ranlib  "\${2}"/bin/gcc-ranlib-\${1} \
				                       --slave   /usr/bin/gcov        gcov        "\${2}"/bin/gcov-\${1}       \
				                       --slave   /usr/bin/gcov-dump   gcov-dump   "\${2}"/bin/gcov-dump-\${1}  \
				                       --slave   /usr/bin/gcov-tool   gcov-tool   "\${2}"/bin/gcov-tool-\${1}  \
				                       --slave   /usr/bin/lto-dump    lto-dump    "\${2}"/bin/lto-dump-\${1}
				}


				InstallClangAlternative()
				{
				  update-alternatives  --install /usr/bin/clang                           clang                        "\${2}"/bin/clang-\${1} \${1}                \
				                       --slave   /usr/bin/amdgpu-arch                     amdgpu-arch                  "\${2}"/bin/amdgpu-arch-\${1}                \
				                       --slave   /usr/bin/c-index-test                    c-index-test                 "\${2}"/bin/c-index-test-\${1}               \
				                       --slave   /usr/bin/clang++                         clang++                      "\${2}"/bin/clang++-\${1}                    \
				                       --slave   /usr/bin/clang-apply-replacements        clang-apply-replacements     "\${2}"/bin/clang-apply-replacements-\${1}   \
				                       --slave   /usr/bin/clang-change-namespace          clang-change-namespace       "\${2}"/bin/clang-change-namespace-\${1}     \
				                       --slave   /usr/bin/clang-check                     clang-check                  "\${2}"/bin/clang-check-\${1}                \
				                       --slave   /usr/bin/clang-cl                        clang-cl                     "\${2}"/bin/clang-cl-\${1}                   \
				                       --slave   /usr/bin/clang-cpp                       clang-cpp                    "\${2}"/bin/clang-cpp-\${1}                  \
				                       --slave   /usr/bin/clang-extdef-mapping            clang-extdef-mapping         "\${2}"/bin/clang-extdef-mapping-\${1}       \
				                       --slave   /usr/bin/clang-format                    clang-format                 "\${2}"/bin/clang-format-\${1}               \
				                       --slave   /usr/bin/clang-include-cleaner           clang-include-cleaner        "\${2}"/bin/clang-include-cleaner-\${1}      \
				                       --slave   /usr/bin/clang-include-fixer             clang-include-fixer          "\${2}"/bin/clang-include-fixer-\${1}        \
				                       --slave   /usr/bin/clang-installapi                clang-installapi             "\${2}"/bin/clang-installapi-\${1}           \
				                       --slave   /usr/bin/clang-linker-wrapper            clang-linker-wrapper         "\${2}"/bin/clang-linker-wrapper-\${1}       \
				                       --slave   /usr/bin/clang-move                      clang-move                   "\${2}"/bin/clang-move-\${1}                 \
				                       --slave   /usr/bin/clang-nvlink-wrapper            clang-nvlink-wrapper         "\${2}"/bin/clang-nvlink-wrapper-\${1}       \
				                       --slave   /usr/bin/clang-offload-bundler           clang-offload-bundler        "\${2}"/bin/clang-offload-bundler-\${1}      \
				                       --slave   /usr/bin/clang-offload-packager          clang-offload-packager       "\${2}"/bin/clang-offload-packager-\${1}     \
				                       --slave   /usr/bin/clang-query                     clang-query                  "\${2}"/bin/clang-query-\${1}                \
				                       --slave   /usr/bin/clang-refactor                  clang-refactor               "\${2}"/bin/clang-refactor-\${1}             \
				                       --slave   /usr/bin/clang-reorder-fields            clang-reorder-fields         "\${2}"/bin/clang-reorder-fields-\${1}       \
				                       --slave   /usr/bin/clang-repl                      clang-repl                   "\${2}"/bin/clang-repl-\${1}                 \
				                       --slave   /usr/bin/clang-scan-deps                 clang-scan-deps              "\${2}"/bin/clang-scan-deps-\${1}            \
				                       --slave   /usr/bin/clang-sycl-linker               clang-sycl-linker            "\${2}"/bin/clang-sycl-linker-\${1}          \
				                       --slave   /usr/bin/clang-tblgen                    clang-tblgen                 "\${2}"/bin/clang-tblgen-\${1}               \
				                       --slave   /usr/bin/clang-tidy                      clang-tidy                   "\${2}"/bin/clang-tidy-\${1}                 \
				                       --slave   /usr/bin/clang.1.gz                      clang.1.gz                   "\${2}"/share/man/man1/clang-\${1}.1.gz      \
				                       --slave   /usr/bin/clangd                          clangd                       "\${2}"/bin/clangd-\${1}                     \
				                       --slave   /usr/bin/diagtool                        diagtool                     "\${2}"/bin/diagtool-\${1}                   \
				                       --slave   /usr/bin/diagtool.1.gz                   diagtool.1.gz                "\${2}"/share/man/man1/diagtool-\${1}.1.gz   \
				                       --slave   /usr/bin/find-all-symbols                find-all-symbols             "\${2}"/bin/find-all-symbols-\${1}           \
				                       --slave   /usr/bin/modularize                      modularize                   "\${2}"/bin/modularize-\${1}                 \
				                       --slave   /usr/bin/nvptx-arch                      nvptx-arch                   "\${2}"/bin/nvptx-arch-\${1}                 \
				                       --slave   /usr/bin/offload-arch                    offload-arch                 "\${2}"/bin/offload-arch-\${1}               \
				                       --slave   /usr/bin/pp-trace                        pp-trace                     "\${2}"/bin/pp-trace-\${1}                   \
				                                                                                                                                                    \
				                       --slave   /usr/lib/share                           llvm-share                   "\${2}"/lib/llvm-\${1}/share                \
				                                                                                                                                                    \
				                       --slave   /usr/bin/llvm-ar                         llvm-ar                      "\${2}"/bin/llvm-ar-\${1}                    \
				                       --slave   /usr/bin/llvm-nm                         llvm-nm                      "\${2}"/bin/llvm-nm-\${1}                    \
				                       --slave   /usr/bin/llvm-ranlib                     llvm-ranlib                  "\${2}"/bin/llvm-ranlib-\${1}
				}



				# Move gcc 13 and clang ?? to gcc ${RequiredGccVersion} and clang ${RequiredClangVersion} on Ubuntu 24.04 LTS

				# add-apt-repository -y ppa:ubuntu-toolchain-r/test                                            # Augment standard repositories when a gcc version isn't otherwise available
				# add-apt-repository --remove ppa:ubuntu-toolchain-r/test/ppa

				apt-get -y update
				apt-get -y full-upgrade
				apt-get -y autoremove

				apt-get -y install build-essential manpages-dev gdb ddd                                        # A few useful odds and ends in case "Tuffix" is not installed (e.g. WSL)
				apt-get -y install dos2unix tree zip xterm language-pack-en                                    # a few more potentially required by either doing or grading homework

				cleanup_list=()




				################
				## Install Ninja
				# apt-get -y install ninja-build                                                               # version 1.11 or better required but not (yet) available in repositories
				# Identify a temporary directory
				if [[ "${ninjaVersion,,}" < "${RequiredNinjaVersion,,}"  ||  "${CMakeOptions[*]}" =~ ---force_update([_-]ninja)?([^[:alnum:]_-]|$) ]]; then   # \b not supported in regex
				  working_dir=\$(mktemp -d)
				  cleanup_list+=( "\${working_dir}" )
				  trap 'rm -rf -- "\${cleanup_list[@]}"' EXIT                                                  # Clean up the temporary directory upon script exit
				  (  cd "\${working_dir}"                                                                                     && \
				     wget  https://github.com/ninja-build/ninja/releases/download/v${RequiredNinjaVersion}/ninja-linux.zip    && \
				     unzip ninja-linux.zip                                                                                    && \
				     rm -rf /usr/local/bin/ninja                                                                              && \
				     mv ninja /usr/local/bin/                                                                                 && \
				     chmod +x /usr/local/bin/ninja  )
				fi




				################
				## Install CMake
				# apt-get -y install cmake                                                                     # version 4.2.0 or better required but not (yet) available in repositories
				# snap install cmake --classic                                                                 # version 4.2.0 available from Snap, but gradescope's Ubuntu minimal image doesn't provide Snap
				if [[ "${cmakeVersion,,}" < "${RequiredCMakeVersion,,}"  ||  "${CMakeOptions[*]}" =~ ---force_update([_-]cmake)?([^[:alnum:]_-]|$) ]]; then
				  working_dir=\$(mktemp -d)
				  cleanup_list+=( "\${working_dir}" )
				  trap 'rm -rf -- "\${cleanup_list[@]}"' EXIT                                                  # Clean up the temporary directory upon script exit
				  (  cd "\${working_dir}"                                                                                                                   && \
				     wget  https://github.com/Kitware/CMake/releases/download/v${RequiredCMakeVersion}/cmake-${RequiredCMakeVersion}-linux-x86_64.tar.gz    && \
				     tar -xf cmake-${RequiredCMakeVersion}-linux-x86_64.tar.gz                                                                              && \
				     rm -rf "/opt/cmake-${RequiredCMakeVersion}"                                                                                            && \
				     mv cmake-${RequiredCMakeVersion}-linux-x86_64 /opt/cmake-${RequiredCMakeVersion}                                                       && \
				     chmod +x /opt/cmake-${RequiredCMakeVersion}/bin/*                                                                                      && \
				     ln -sf /opt/cmake-${RequiredCMakeVersion}/bin/* /usr/local/bin )
				fi





				################
				## Install Clang
				if [[ "${clangVersion,,}" < "${RequiredClangVersion,,}"  ||  "${CMakeOptions[*]}" =~ ---force_update([_-]clang)?([^[:alnum:]_-]|$) ]]; then
				  wget https://apt.llvm.org/llvm.sh
				  chmod +x llvm.sh
				  ./llvm.sh ${RequiredClangVersion%%.*} all
				  rm llvm.sh

				  update-alternatives   --remove clang $( update-alternatives --list clang | grep -E "clang-${RequiredClangVersion%%.*}$" )
				  InstallClangAlternative "13"                          "/usr"                                 # Might have this one if upgrading versions
				  InstallClangAlternative "${RequiredClangVersion%%.*}" "/usr"
				  update-alternatives --auto clang
				fi




				################
				## Install GCC
				if [[ "${gccVersion,,}" < "${RequiredGccVersion,,}"  ||  "${CMakeOptions[*]}" =~ ---force_update([_-]gcc)?([^[:alnum:]_-]|$) ]]; then
				  # Limited backward support for gcc 15 on Ubuntu 22.04 and 24.04   Delete this workaround once
				  # Gradescope offers 24.04 as a baseline image AND 24.04 offers gcc 15 natively
				  # https://askubuntu.com/questions/1513160/how-to-install-gcc-14-on-ubuntu-22-04-and-24-04
				  #
				  update-alternatives   --remove gcc $( update-alternatives --list gcc | grep -E "gcc-${RequiredGccVersion%%.*}$" )

				  if [[ -n "$(apt-cache search --names-only "^gcc-${RequiredGccVersion%%.*}$")"   ]]; then
				    apt-get -y install gcc-${RequiredGccVersion%%.*} g++-${RequiredGccVersion%%.*}
				    InstallGccAlternative   "${RequiredGccVersion%%.*}"   "/usr"

				  else
				    if [[ ! -f  "${0%/*}/gcc-${RequiredGccVersion}.0.tar.xz" ]]; then                          # look for the gcc-*.tar.xz workaround file in same directory as Build.sh
				        wget -O "${0%/*}/gcc-${RequiredGccVersion}.0.tar.xz"  'https://www.dropbox.com/scl/fi/66qz0ywb3ab7vnset3j6r/gcc-15.2.0.tar.xz?rlkey=qrw3882mjl8miixi39rinhrch&st=i8vel8qm&dl=1'
				    fi

				    # add some prerequisites to build and install gcc
				    apt-get -y install libmpfr-dev libgmp3-dev libmpc-dev

				    # Identify a temporary directory
				    temp_dir="/tmp"                                                                            # I wanted to use \$(mktemp -d) but that would force make install to recompile
				    working_dir="\${temp_dir}/gcc-${RequiredGccVersion}.0"

				    # Remove the directory if it already exists
				    if [[ -d "\${working_dir}" ]]; then
				        rm -rf "\${working_dir}"
				    fi

				  	cleanup_list+=( "\${working_dir}" )
				  	trap 'rm -rf -- "\${cleanup_list[@]}"' EXIT                                                # Clean up the temporary directory upon script exit

				    # install the build from offline precompiled sources
				    tar -xf "${0%/*}/gcc-${RequiredGccVersion}.0.tar.xz"  --delay-directory-restore  -C "\${temp_dir}"

				    # Remove the directory if it already exists
				    if [[ -d "/usr/local/${RequiredGccVersion}.0" ]]; then
				      rm -rf "/usr/local/${RequiredGccVersion}.0"
				    fi

				    (cd "\${working_dir}"  &&  make install)

				    InstallGccAlternative   "${RequiredGccVersion%%.*}"   "/usr/local/gcc-${RequiredGccVersion}.0"
				  fi

				  InstallGccAlternative   "11"   "/usr"                                                        # 22.04 build-essential's default version
				  InstallGccAlternative   "13"   "/usr"                                                        # 24.04 build-essential's default version
				  update-alternatives --auto gcc
				fi




				###################
				# To remove:
				# sudo update-alternatives --set gcc   /usr/bin/gcc-9
				# sudo update-alternatives --set clang /usr/bin/clang-10

				###################
				# To select which version interactively:
				# sudo update-alternatives --config gcc
				# sudo update-alternatives --config clang

				###################
				# To remove a group:
				# sudo update-alternatives --remove-all gcc
				# sudo update-alternatives --remove-all clang

				###################
				# References:
				# https://apt.llvm.org/
				# https://stackoverflow.com/questions/67298443/when-gcc-11-will-appear-in-ubuntu-repositories/67406788#67406788

				EOF

          exit

          fi # upgrade? 2
        fi  # upgrade? 1

      else ## Ubuntu, but not 24.04 or 22.04
        echo -e "These required GCC and/or Clang versions are not available on Ubuntu ${Release}\n"
        echo -e "Please consider upgrading to Ubuntu 24.04.  See https://youtu.be/2Mwo4BfJuvA"
        echo -e "    sudo do-release-upgrade\n"

      fi  ## Ubuntu 24.04 support with some basic backward support for Ubuntu 22.04

    fi

    echo -e "Build process aborted.\nPlease install GCC version ${RequiredGccVersion} or better and Clang version ${RequiredClangVersion} or better for your platform,\nalong with CMAke version ${RequiredCMakeVersion} or better and Ninja version ${RequiredNinjaVersion} or better"
    echo -e "\n\nPlatform Information:\n====================="
    cat  /etc/*-release
    exit
  fi  # gccVersion || clangVersion || RequiredCMakeVersion || RequiredNinjaVersion || force_update
}


CheckVersion


################################################################################
# Now that the environment has been verified, configure, compile, and link the solution using both
# Clang and GCC.  For each toolchain, first generate a build configuration with:
#   1)  cmake -S ./cmake  -B .build/[clang|gcc]  -G Ninja  [-DCMAKE_BUILD_TYPE=[Debug|Release]] -Wno-dev [-DProjectName=xxx] [--compile-no-warning-as-error] [--link-no-warning-as-error]
# then build (compile and link) with:
#   2)  cmake --build .build/[clang|gcc]  [--clean-first] [-v]
#
# Once this Build.sh script completes and the environment is set up, these command may also be executed
# directly from the console command prompt, but be sure to get all the parameters correct.  Header, source, and module interface
# files that make up the solution are detected at configuration, not building. Hence, after the initial configuration, running
# the configuration again is needed only after files have added or removed.
#
# The directory containing this Build.sh script must also contain a subdirectory named "cmake".  In other words, the cmake directory must be
# in the same directory as this file (Build.sh).
#
# Two subdirectories will be created:
#  1) .build - used privately during build process.  You can remove this directory, but do not edit anything
#  2) bin    - your executables will be placed here. You can remove this directory
################################################################################
## "${gccVersion:-not found}"
for toolchain in ${ToolChains[@]:-"clang" "gcc"}; do
  toolVersion="${toolchain,,}Version"   # dereferences to either gccVersion or clangVersion, which are defined above
  echo -e "******************\n** ${toolchain^^}: Configuring, compiling, and linking using ${toolchain^^} version ${!toolVersion}\n******************"
  script=$( realpath "${0}" )    # cmake subdirectory must be next to the canonical Build.sh file, but let's allow a symlink to Build.sh (.vscode subdir, for example)
  cmake -S "${script%/*}/cmake"  -B "${PWD}/.build/${toolchain}"  -G Ninja  -DCMAKE_BUILD_TYPE=Release -Wno-dev -DProjectName="${executableFileName}" "${CMakeOptions[@]}"
  cmake --build "${PWD}/.build/${toolchain}" "${NinjaOptions[@]}"

  # No need to continue if errors have already been found
  if [[ $? -ne 0 ]]; then
    exit 1
  fi

  echo ""
done
