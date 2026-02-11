# 0 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
# 1 "/mnt/c/users/peter/dev/CppFun/CppFun/AlgoEngineering/_C++_Development_Root/SourceCode/.build/gcc//"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
# 24 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
module;

# 1 "/usr/local/gcc-15.2.0/include/c++/15.2.0/stdbit.h" 1 3
# 27 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc" 2
# 1 "/usr/local/gcc-15.2.0/include/c++/15.2.0/stdckdint.h" 1 3
# 28 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc" 2

export module std.compat;
export import std;
# 89 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
export
{
  using std::isalnum;
  using std::isalpha;



  using std::iscntrl;
  using std::isdigit;
  using std::isgraph;
  using std::islower;
  using std::isprint;
  using std::ispunct;
  using std::isspace;
  using std::isupper;
  using std::isxdigit;
  using std::tolower;
  using std::toupper;
}





export
{
# 130 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
}





export
{
# 149 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
}





export
{
  using std::lconv;
  using std::localeconv;
  using std::setlocale;

}


export
{
  using std::abs;
  using std::acos;
  using std::acosf;
  using std::acosh;
  using std::acoshf;
  using std::acoshl;
  using std::acosl;
  using std::asin;
  using std::asinf;
  using std::asinh;
  using std::asinhf;
  using std::asinhl;
  using std::asinl;
  using std::atan;
  using std::atan2;
  using std::atan2f;
  using std::atan2l;
  using std::atanf;
  using std::atanh;
  using std::atanhf;
  using std::atanhl;
  using std::atanl;
  using std::cbrt;
  using std::cbrtf;
  using std::cbrtl;
  using std::ceil;
  using std::ceilf;
  using std::ceill;
  using std::copysign;
  using std::copysignf;
  using std::copysignl;
  using std::cos;
  using std::cosf;
  using std::cosh;
  using std::coshf;
  using std::coshl;
  using std::cosl;
  using std::double_t;
  using std::erf;
  using std::erfc;
  using std::erfcf;
  using std::erfcl;
  using std::erff;
  using std::erfl;
  using std::exp;
  using std::exp2;
  using std::exp2f;
  using std::exp2l;
  using std::expf;
  using std::expl;
  using std::expm1;
  using std::expm1f;
  using std::expm1l;
  using std::fabs;
  using std::fabsf;
  using std::fabsl;
  using std::fdim;
  using std::fdimf;
  using std::fdiml;
  using std::float_t;
  using std::floor;
  using std::floorf;
  using std::floorl;
  using std::fma;
  using std::fmaf;
  using std::fmal;
  using std::fmax;
  using std::fmaxf;
  using std::fmaxl;
  using std::fmin;
  using std::fminf;
  using std::fminl;
  using std::fmod;
  using std::fmodf;
  using std::fmodl;
  using std::fpclassify;
  using std::frexp;
  using std::frexpf;
  using std::frexpl;
  using std::hypot;
  using std::hypotf;
  using std::hypotl;
  using std::ilogb;
  using std::ilogbf;
  using std::ilogbl;
  using std::isfinite;
  using std::isgreater;
  using std::isgreaterequal;
  using std::isinf;
  using std::isless;
  using std::islessequal;
  using std::islessgreater;
  using std::isnan;
  using std::isnormal;
  using std::isunordered;
  using std::ldexp;
  using std::ldexpf;
  using std::ldexpl;



  using std::lgamma;
  using std::lgammaf;
  using std::lgammal;
  using std::llrint;
  using std::llrintf;
  using std::llrintl;
  using std::llround;
  using std::llroundf;
  using std::llroundl;
  using std::log;
  using std::log10;
  using std::log10f;
  using std::log10l;
  using std::log1p;
  using std::log1pf;
  using std::log1pl;
  using std::log2;
  using std::log2f;
  using std::log2l;
  using std::logb;
  using std::logbf;
  using std::logbl;
  using std::logf;
  using std::logl;
  using std::lrint;
  using std::lrintf;
  using std::lrintl;
  using std::lround;
  using std::lroundf;
  using std::lroundl;
  using std::modf;
  using std::modff;
  using std::modfl;
  using std::nan;
  using std::nanf;
  using std::nanl;
  using std::nearbyint;
  using std::nearbyintf;
  using std::nearbyintl;
  using std::nextafter;
  using std::nextafterf;
  using std::nextafterl;
  using std::nexttoward;
  using std::nexttowardf;
  using std::nexttowardl;
  using std::pow;
  using std::powf;
  using std::powl;
  using std::remainder;
  using std::remainderf;
  using std::remainderl;
  using std::remquo;
  using std::remquof;
  using std::remquol;
  using std::rint;
  using std::rintf;
  using std::rintl;
  using std::round;
  using std::roundf;
  using std::roundl;
  using std::scalbln;
  using std::scalblnf;
  using std::scalblnl;
  using std::scalbn;
  using std::scalbnf;
  using std::scalbnl;
  using std::signbit;
  using std::sin;
  using std::sinf;
  using std::sinh;
  using std::sinhf;
  using std::sinhl;
  using std::sinl;
  using std::sqrt;
  using std::sqrtf;
  using std::sqrtl;
  using std::tan;
  using std::tanf;
  using std::tanh;
  using std::tanhf;
  using std::tanhl;
  using std::tanl;
  using std::tgamma;
  using std::tgammaf;
  using std::tgammal;
  using std::trunc;
  using std::truncf;
  using std::truncl;
# 421 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
}


export
{
  using std::jmp_buf;
  using std::longjmp;

}


export
{
  using std::raise;
  using std::sig_atomic_t;
  using std::signal;

}


export
{
  using std::va_list;

}


export
{
  using std::max_align_t;
  using std::nullptr_t;
  using std::ptrdiff_t;
  using std::size_t;
# 470 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
}


export
{
  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::int_fast16_t;
  using std::int_fast32_t;
  using std::int_fast64_t;
  using std::int_fast8_t;
  using std::int_least16_t;
  using std::int_least32_t;
  using std::int_least64_t;
  using std::int_least8_t;
  using std::intmax_t;
  using std::intptr_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;
  using std::uint_fast16_t;
  using std::uint_fast32_t;
  using std::uint_fast64_t;
  using std::uint_fast8_t;
  using std::uint_least16_t;
  using std::uint_least32_t;
  using std::uint_least64_t;
  using std::uint_least8_t;
  using std::uintmax_t;
  using std::uintptr_t;
}


export
{
  using std::clearerr;
  using std::fclose;
  using std::feof;
  using std::ferror;
  using std::fflush;
  using std::fgetc;
  using std::fgetpos;
  using std::fgets;
  using std::FILE;
  using std::fopen;
  using std::fpos_t;
  using std::fprintf;
  using std::fputc;
  using std::fputs;
  using std::fread;
  using std::freopen;
  using std::fscanf;
  using std::fseek;
  using std::fsetpos;
  using std::ftell;
  using std::fwrite;
  using std::getc;
  using std::getchar;
  using std::perror;
  using std::printf;
  using std::putc;
  using std::putchar;
  using std::puts;
  using std::remove;
  using std::rename;
  using std::rewind;
  using std::scanf;
  using std::setbuf;
  using std::setvbuf;
  using std::size_t;
  using std::snprintf;
  using std::sprintf;
  using std::sscanf;
  using std::tmpfile;



  using std::ungetc;
  using std::vfprintf;
  using std::vfscanf;
  using std::vprintf;
  using std::vscanf;
  using std::vsnprintf;
  using std::vsprintf;
  using std::vsscanf;
}


export
{
  using std::_Exit;
  using std::abort;
  using std::abs;






  using std::atexit;
  using std::atof;
  using std::atoi;
  using std::atol;
  using std::atoll;
  using std::bsearch;
  using std::calloc;
  using std::div;
  using std::div_t;
  using std::exit;
  using std::free;
  using std::getenv;
  using std::labs;
  using std::ldiv;
  using std::ldiv_t;
  using std::llabs;
  using std::lldiv;
  using std::lldiv_t;
  using std::malloc;





  using std::qsort;



  using std::rand;
  using std::realloc;
  using std::size_t;
  using std::srand;
  using std::strtod;
  using std::strtof;
  using std::strtol;
  using std::strtold;
  using std::strtoll;
  using std::strtoul;
  using std::strtoull;
  using std::system;




}


export
{
  using std::memchr;
  using std::memcmp;
  using std::memcpy;
  using std::memmove;
  using std::memset;
  using std::size_t;
  using std::strcat;
  using std::strchr;
  using std::strcmp;
  using std::strcoll;
  using std::strcpy;
  using std::strcspn;
  using std::strerror;
  using std::strlen;
  using std::strncat;
  using std::strncmp;
  using std::strncpy;
  using std::strpbrk;
  using std::strrchr;
  using std::strspn;
  using std::strstr;
  using std::strtok;
  using std::strxfrm;
}


export
{
  using std::asctime;
  using std::clock;
  using std::clock_t;
  using std::ctime;
  using std::difftime;
  using std::gmtime;
  using std::localtime;
  using std::mktime;
  using std::size_t;
  using std::strftime;
  using std::time;
  using std::time_t;
  using std::tm;




}


export
{
# 681 "/usr/local/gcc-15.2.0/include/c++/15.2.0/bits/std.compat.cc"
}
