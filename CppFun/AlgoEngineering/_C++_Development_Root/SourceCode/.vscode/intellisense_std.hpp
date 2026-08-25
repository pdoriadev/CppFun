// Workaround for incomplete intellisense support
#pragma once

// - Visual Studio &VSCode(cpptools):
//     The IntelliSense parser defines __INTELLISENSE__ during its analysis pass.That way you can detect “this code is being parsed by IntelliSense,  not compiled.”
// - Clang/LLVM or GCC compilers:
//     They do not define __INTELLISENSE__.  If you run clang++ or g++ directly, the macro won’t exist. It’s strictly an editor‑side macro.
// - Purpose:
//     Lets you gate code so IntelliSense sees extra includes or simplified constructs, while the real compiler ignores them.
#ifdef __INTELLISENSE__



// C++ standard headers (C++20/23)

// Language support
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>

// STL containers
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <queue>

// Algorithms & utilities
#include <algorithm>
#include <execution>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>
#include <tuple>
#include <compare>
#include <optional>
#include <variant>
#include <any>
#include <bitset>
#include <initializer_list>

// Strings & I/O
#include <string>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>

// Localization
#include <locale>

// Regular expressions
#include <regex>

// Smart pointers & memory
#include <memory>
#include <scoped_allocator>
#include <new>

// Concurrency & threading
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <stop_token>
#include <barrier>
#include <latch>
#include <semaphore>

// Time, random, filesystem
#include <chrono>
#include <random>
#include <filesystem>

// Math & numerics
#include <complex>
#include <valarray>
#include <limits>

// Exceptions & RTTI
#include <exception>
#include <stdexcept>
#include <system_error>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

// C compatibility headers (C++ versions of C headers)
#include <ccomplex>
#include <ctgmath>

// Newer C++20/23 additions
#include <span>
#include <ranges>
#include <concepts>
#include <coroutine>
#include <syncstream>
#include <source_location>
#include <bit>
#include <version>

#endif // __INTELLISENSE__
