/***********************************************************************************************************************************
** Class TimerType - A class of objects that keeps track of CPU time used since the object was created or last reset
**                   and reports that time in Resolution units.  It implicitly converts to Resolution units so it can
**                   be used like this:
**
**      Timer t;                                       // begin timing some operation, or
**      Timer t("The consumed time is:  ")             // begin timing some operation and provide results message, or
**      Timer t("The consumed time is:  ", std::clog)  // begin timing some operation, provide results message, and provide where to write message
**      ...
**      std::cout << t;                                // print out how much CPU time has elapsed in Resolution units
**
**  If a results message was provided at construction, that message and time duration is emitted at destruction
**
***********************************************************************************************************************************/
#pragma once

#include <chrono>     // steady_clock, duration_cast<>(), hh_mm_ss(), now(), time_point
#include <iostream>
#include <string>
#include <utility>    // move()
#include <ratio>      // nano, micro, milli





namespace Utilities
{
  template<typename Resolution, typename Clock = std::chrono::high_resolution_clock>
  class TimerType
  {
  public:
    // Note:  Constructing the message parameter object and then moving its contents to the _message attribute happens before the
    //        clock starts and is not included in the timed results.  Unfortunately, the message parameter object's destructor is
    //        called after the clock starts and is included in the time results.
    TimerType( std::string message, std::ostream& stream = std::cout )
      : _message{ std::move(message) }, _stream{ &stream }
    {}


    // Note:  This constructor avoids the overhead of destroying the message parameter
    TimerType( std::ostream & stream = std::cout )
      : _stream{ &stream }
    {}


    ~TimerType() noexcept
    {
      std::chrono::nanoseconds duration = Clock::now() - _start;
      if( !_message.empty() )
      {
        using namespace std::chrono_literals;

        ( *_stream ) << _message;

        if     ( duration < 1us )  ( *_stream ) <<                                                                        (duration).count() << " nanoseconds (ns)\n";
        else if( duration < 1ms )  ( *_stream ) << std::chrono::duration_cast< std::chrono::duration<double, std::micro> >(duration).count() << " microseconds (us)\n";
        else if( duration < 1s  )  ( *_stream ) << std::chrono::duration_cast< std::chrono::duration<double, std::milli> >(duration).count() << " milliseconds (ms)\n";
        else if( duration < 100s)  ( *_stream ) << std::chrono::duration_cast< std::chrono::duration<double            > >(duration).count() << " seconds\n";
        else                       ( *_stream ) << std::chrono::hh_mm_ss<std::chrono::nanoseconds>{duration}                                 << " (hh:mm:ss.ss)\n";
      }
    }

    // Implicit casting operator
    operator typename Resolution::rep() const
    { return std::chrono::duration_cast<Resolution>( Clock::now() - _start ).count(); }


    void reset()
    { _start = Clock::now(); }



    private:
      std::string                _message;
      std::ostream*              _stream   = nullptr;             // storing the stream as a pointer instead of a reference allows
                                                                  // the compiler to synthesize the copy and copy assignment functions.
      typename Clock::time_point _start    = Clock::now();        // physically the last attributes to ensure the clock starts after everything else
  };




  // Let's create a couple default timer types
  using TimerNS  = TimerType< std::chrono::nanoseconds  >;
  using TimerUS  = TimerType< std::chrono::microseconds >;
  using TimerMS  = TimerType< std::chrono::milliseconds >;
  using Timer    = TimerType< std::chrono::seconds      >;
  using TimerMin = TimerType< std::chrono::minutes      >;


}  // namespace Utilities












/***********************************************************************************************************************************
** (C) Copyright 2021 by Thomas Bettens. All Rights Reserved.
**
** DISCLAIMER: The participating authors at California State University's Computer Science Department have used their best efforts
** in preparing this code. These efforts include the development, research, and testing of the theories and programs to determine
** their effectiveness. The authors make no warranty of any kind, expressed or implied, with regard to these programs or to the
** documentation contained within. The authors shall not be liable in any event for incidental or consequential damages in
** connection with, or arising out of, the furnishing, performance, or use of these libraries and programs.  Distribution without
** written consent from the authors is prohibited.
***********************************************************************************************************************************/

/**************************************************
** Last modified:  23-June-2021  (Added nano seconds, optimized when clock starts and stops)
** Last Verified:  23-June-2021
** Verified with:  MS Visual Studio 2019 Version 16.10.2 (C++20)
**                 GCC version 11.1.1 20210510 (-std=c++20 ),
**                 Clang version 12.0.0 (-std=c++20 -stdlib=libc++)
***************************************************/
