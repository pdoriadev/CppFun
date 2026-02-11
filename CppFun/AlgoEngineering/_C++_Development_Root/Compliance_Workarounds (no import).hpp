#pragma once
// Version:  "25.06.28"

// need to include something from the standard library so _LIBCPP_VERSION gets defined, so don't move
// these #includes under that conditional compile
#include <version>

// C++23 Transition Workarounds
//    _LIBCPP_VERSION is set if using LLVM's libc++ library
//    __GLIBCXX__     is set if using GNU's libstdc++ library (set to date of release)
//           (__GLIBC__ * 1'000 + __GLIBC_MINOR__)  gives libstdc++ version
//    __GNUC__        is defined if using GCC, but also sometimes when using Clang
//          (__GNUC__ * 1'000'000 + __GNUC_MINOR__ * 1'000 + __GNUC_PATCHLEVEL__) give gcc version


#if 0    // keep LLVM's libc++ library Only example around for reference
  #if defined( _LIBCPP_VERSION )
    namespace std
    {
      #if _LIBCPP_VERSION < 17'000'0
        template <typename T>
        inline strong_ordering compare3way_helper( const T & lhs, const T & rhs )
        {
          auto lhsCurrent = cbegin( lhs ),  rhsCurrent = cbegin( rhs );
          auto lhsEnd     = cend  ( lhs ),  rhsEnd     = cend  ( rhs );

          while( lhsCurrent != lhsEnd  &&  rhsCurrent != rhsEnd )
          {
            auto result = compare_weak_order_fallback( *lhsCurrent++, *rhsCurrent++ );
            if( result < 0 ) return strong_ordering::less;
            if( result > 0 ) return strong_ordering::greater;
          }

          if( lhsCurrent == lhsEnd  &&  rhsCurrent == rhsEnd ) return strong_ordering::equivalent;
          if( lhsCurrent == lhsEnd                           ) return strong_ordering::less;
          else                                                 return strong_ordering::greater;
        }


        template< typename T>
        inline strong_ordering operator<=>( const std::vector<T> & lhs, const std::vector<T> & rhs ) noexcept
        {
          return compare3way_helper( lhs, rhs );
        }


        template<typename T>
        inline strong_ordering operator<=>( const std::list<T> & lhs, const std::list<T> & rhs ) noexcept
        {
          return compare3way_helper( lhs, rhs );
        }

        template<typename T>
        inline strong_ordering operator<=>( const std::forward_list<T> & lhs, const std::forward_list<T> & rhs ) noexcept
        {
          return compare3way_helper( lhs, rhs );
        }

        template<typename T, size_t N, size_t M>
        inline strong_ordering operator<=>( const std::array<T, N> & lhs, const std::array<T, M> & rhs ) noexcept
        {
          return compare3way_helper( lhs, rhs );
        }
      #else
        // Fixed in clang version 17.0.3
        // #pragma message ("A potentially obsolete C++20 workaround is present.  Either remove the workaround if no longer needed, or update the version number requiring it")
      #endif


    } // namespace std
  #endif // defined( _LIBCPP_VERSION )
#endif   // keep example around for reference


#if 0    // keep LLVM's libc++ or libstdc++ library example around for reference
  #if( ( defined( _LIBCPP_VERSION ) && (_LIBCPP_VERSION                      <   17'000'0 )) || \
       ( defined( __GLIBCXX__     ) && ( __GLIBC__ * 1'000 + __GLIBC_MINOR__ <=   2'036   )) )      // ldd 2.37 and gcc 13.0.1 (run "ldd --version" and "gcc --version" at the command line)
    namespace std::chrono                                                                           // Check lib version and not gcc version, clang may use gnu's libstdc++ library
    {
      template<class Duration>
      inline std::ostream & operator<<( std::ostream & os, const std::chrono::hh_mm_ss<Duration> & t )
      {
        return os << t.hours     ().count() << ':'
                  << t.minutes   ().count() << ':'
                  << t.seconds   ().count() << "."
                  << t.subseconds().count();
      }
    }    // namespace std::chrono
  #else
    // gcc: fixed in ldd 2.37, but not ldd 2.35.  Don't know about 2.36
    // Fixed in clang version 17.0.3
  #endif    // ( _LIBCPP_VERSION || __GLIBCXX__ )
#endif    // keep example around for reference



// GNU's libstdc++ library
#if defined( __GLIBCXX__ )                                                                      // If GNU's libstdc++ library (GCC or Clang)
  #if ( __GLIBC__ * 1'000 + __GLIBC_MINOR__ <= 2'039 )                                          // ldd 2.39 and gcc 15.1.0  (run "ldd --version" and "gcc --version" at the command line)
    #include <format>
    #include <queue>
    #include <stack>

    namespace std                                                                               // Check lib version and not gcc version, clang may use gnu's libstdc++ library
    {
      // Generic formatter for any adaptor exposing a protected container `c`
      template<typename Adaptor, typename Container, typename T>
      requires std::formattable<T, char>
      struct adaptor_formatter : std::range_formatter<T>
      {
        auto format( const Adaptor & adapter, auto & ctx ) const
        {
          struct Accessor : Adaptor                                                             // Derive a class so we can access the protected container member
          {
            static const Container & get( const Adaptor & a )                                   // returns a reference to the underlying container
            { return a.*&Accessor::c; }                                                         // c is a protected member of the adapter, accessible to derived classes
          };
          return std::range_formatter<T>::format( Accessor::get( adapter ), ctx );
        }
      };

      template<typename T, typename Container>
      struct formatter<stack<T, Container>> : adaptor_formatter<stack<T, Container>, Container, T>
      {};

      template<typename T, typename Container>
      struct formatter<queue<T, Container>> : adaptor_formatter<queue<T, Container>, Container, T>
      {};

      template<typename T, typename Container, typename Compare>
      struct formatter<priority_queue<T, Container, Compare>> : adaptor_formatter<priority_queue<T, Container, Compare>, Container, T>
      {};
    }    // namespace std
  #else
  // gcc: fixed in ldd 2.41 (g++ version 15.1.1), don't know about 2.40
  // #pragma message ("A potentially obsolete C++23 workaround is present.  Either remove the workaround if no longer needed, or update the version number requiring it")
  #endif
#endif    // GNU's libstdc++ library
