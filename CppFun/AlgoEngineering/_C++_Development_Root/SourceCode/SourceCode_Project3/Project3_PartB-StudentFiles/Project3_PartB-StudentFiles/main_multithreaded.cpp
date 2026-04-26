#include <algorithm>      // ranges::sort(), min()
#include <concepts>       // same_as
#include <cstddef>        // ptrdiff_t, size_t
#include <cstdio>         // EOF  (should use traits_type::eof() instead)
#include <cstdlib>        // exit(), size_t
#include <exception>      // exception, what()
#include <fstream>        // ifstream
#include <ios>            // streamsize
#include <iostream>       // cout, cerr, clog, cin, print(), println()
#include <list>           // list
#include <locale>         // locale, isalpha(), isalnum(), tolower()
#include <memory>         // unique_ptr, make_unique()
#include <semaphore>      // counting_semaphore
#include <sstream>        // ostringstream, stringbuf
#include <streambuf>      // streambuf
#include <string>         // string
#include <string_view>    // string_view
#include <thread>         // thread, hardware_concurrency()
#include <typeinfo>       // typeid()::name()
#include <utility>        // move()
#include <vector>         // vector

#include "Algorithm.hpp"
#include "Timer.hpp"







namespace    // anonymous, unnamed namespace (avoids polluting the global namespace)
{
  /*********************************************************************************************************************************
  **  Start Multi Threaded Overhead:
  **    Declarations, Definitions, and Objects for per-thread output capture and redirection.
  **
  *********************************************************************************************************************************/
  // Define a thread-safe stream buffer which forwards output to a thread-local std::stringbuf when set; otherwise forwards to the
  // original buffer.
  enum class RedirectId : unsigned {COUT, CERR, CLOG};

  template<RedirectId Id>
  struct RedirectBuf : std::streambuf
  {
    public:
      explicit RedirectBuf( std::streambuf * orig ) noexcept
      : _orig( orig )
      {}

      static void set( std::stringbuf * b ) noexcept
      { _local_buf = b; }

      static void clear() noexcept
      { _local_buf = nullptr; }


    protected:
      int overflow( int atom = EOF ) override
      {
        // Return value
        //  a) Traits::eof() to indicate failure, or
        //  b) c if the character c was successfully appended, or
        //  c) some value other than Traits::eof() if called with Traits::eof() as the argument
        if( atom == EOF ) return 0;

        std::streambuf * target = (_local_buf == nullptr ? _orig : _local_buf);
        return target->sputc( static_cast<char>( atom ) );
      }


      std::streamsize xsputn( const char * str, std::streamsize size ) override
      {
        std::streambuf * target = ( _local_buf == nullptr ? _orig : _local_buf );
        return target->sputn( str, size );
      }


      int sync() override
      {
        std::streambuf * target = ( _local_buf == nullptr ? _orig : _local_buf );
        return target->pubsync();
      }


    private:
      std::streambuf *                            _orig;
      inline static thread_local std::stringbuf * _local_buf = nullptr;    // a stringbuf "is-a" streambuf
  };



  // Create three redirect buffers for standard output streams. These will be installed in main() before threads are launched so that
  // each thread can set its own thread-local target buffer and have its output captured independently.
  RedirectBuf<RedirectId::COUT> * g_cout_redirect = nullptr;
  RedirectBuf<RedirectId::CERR> * g_cerr_redirect = nullptr;
  RedirectBuf<RedirectId::CLOG> * g_clog_redirect = nullptr;



  // Simple RAII guard to acquire and release a counting semaphore.
  struct SemGuard
  {
    explicit SemGuard( std::counting_semaphore<> & sem ) : _sem( sem ) { _sem.acquire(); }
    ~SemGuard() { _sem.release(); }

    private:
      std::counting_semaphore<> & _sem;
  };
  /*********************************************************************************************************************************
  **  End Multi Threaded Overhead
  *********************************************************************************************************************************/








  // Algorithm Driver function
  using StringVector = std::vector<std::string>;
  using StringList   = std::list  <std::string>;




  std::string sanitize( std::string_view word )
  {
    if( word.empty() ) return {};

    static const std::locale locality;
    std::size_t              startIndex = 0;
    std::size_t              endIndex   = word.size() - 1;

    while( startIndex < word.size()  &&  !std::isalpha( word[startIndex], locality ) )   ++startIndex;
    while( endIndex   > startIndex   &&  !std::isalnum( word[endIndex],   locality ) )   --endIndex;

    std::string result{ word.substr( startIndex, endIndex - startIndex + 1 ) };
    for( auto & c : result ) c = std::tolower( c, locality );

    return result;
  }





  const StringVector baseline_repository{ [] -> StringVector
                                          {
                                            std::ifstream fin( "A Bunch of Words.txt" );
                                            if( !fin )
                                            {
                                              // Don't throw an exception here - there's nothing to catch it!
                                              std::println( std::cerr, "\n\n*** Error, failed to open the file \"A Bunch of Words.txt\" for reading\n"
                                                                       "Make sure that the file is in your current working directory and that the file name is correct.\n\n" );
                                              std::exit( -__LINE__ );
                                            }

                                            StringVector words;
                                            std::string  word;
                                            while( fin >> word )
                                            {
                                              word = sanitize( word );
                                              if( !word.empty() )   words.push_back( std::move( word ) );
                                            }

                                            return words;
                                          }() };




  // Algorithm Driver function
  template <typename T>
  void execute( const Algorithm<T> & algo, std::size_t length = baseline_repository.size() )
  {
    try
    {
      typename Algorithm<T>::Collection_Type words{ baseline_repository.begin(),
                                                    baseline_repository.begin() + std::min( length, baseline_repository.size() ) };
      const typename Algorithm<T>::Collection_Type expected_results{ [&words] -> Algorithm<T>::Collection_Type
                                                                     {
                                                                       auto results = words;
                                                                       if constexpr( std::same_as< decltype(results), StringList > ) results.sort();
                                                                       else  std::ranges::sort( results );
                                                                       return results;
                                                                     }() };



      // Display the initial state
      std::print( std::cout,
                  "{:=>40}\n"
                  "{}\n"
                  "Number of words to sort : {:L}\n\n"
                  "Starting execution\n"
                  "{:.>25}\n",
                  "", algo.description(), words.size(), "" );





      // Execute the algorithm and measure wall clock execution time.
      std::ostringstream     measured_time;
      Utilities::Timer( " ", measured_time ),   algo.run( words );



      std::print( std::cout,
                  "{:.>25}\n"
                  "Execution completed\n\n"
                  "Execution time          :{}"                                         // leading space is part of Timer construction message above
                  "Content properly sorted : {}\n"
                  "{:=>40}\n\n\n",
                  "", measured_time.str(), words==expected_results, "");
    }
    catch (std::exception & ex)
    {
      std::print( std::cerr, "\n\n*** Error, unexpected exception caught\n{}\n{}\n\n", typeid( ex ).name(), ex.what() );
      throw;
    }
  }
}    // end of anonymous namespace







int main()
{
  std::locale locale{ "en_US.UTF-8" };                                    // Select the locale
  std::locale::global( locale );                                          // make this locale be the default for newly constructed objects
  std::cout.imbue( locale );                                              // update streams already constructed
  std::clog.imbue( locale );
  std::cerr.imbue( locale );
  std::cin .imbue( locale );

  Utilities::Timer total_time( "\n\n\nTotal execution time:  " );


  /*********************************************************************************************************************************
  **  Start Multi Threaded Overhead:
  **
  *********************************************************************************************************************************/
  // Install global redirecting streambufs that support per-thread targets.
  auto * orig_cout_buf = std::cout.rdbuf();
  auto * orig_clog_buf = std::clog.rdbuf();
  auto * orig_cerr_buf = std::cerr.rdbuf();

  // Allocate redirect buffers and install them; they forward to thread-local std::stringbuf instances when set from the worker thread.
  RedirectBuf<RedirectId::COUT> cout_redirect( orig_cout_buf );
  RedirectBuf<RedirectId::CERR> cerr_redirect( orig_cerr_buf );
  RedirectBuf<RedirectId::CLOG> clog_redirect( orig_clog_buf );

  g_cout_redirect = &cout_redirect;
  g_clog_redirect = &clog_redirect;
  g_cerr_redirect = &cerr_redirect;

  std::cout.rdbuf( g_cout_redirect );
  std::clog.rdbuf( g_clog_redirect );
  std::cerr.rdbuf( g_cerr_redirect );

  // Schedule all iterations as separate threads.  Each iteration gets its own capture buffers for stdout, log and error.  Threads set
  // their thread-local targets on start so their use of std::cout, std::clog, and std::cerr is captured independently.
  std::vector<std::thread> threads;
  std::vector<std::unique_ptr<std::stringbuf>> out_bufs, log_bufs, err_bufs;

  // Limit concurrency to number of CPUs if number of CPUs is 4 or less, and number of CPUs - 1 if number of CPUs is greater than 4.
  // Note:  At the time this was written/tested, Gradescope provides only 4 CPU cores, so we want to allow full concurrency in that case.
  unsigned hc = std::thread::hardware_concurrency();
  unsigned max_threads = (hc > 4 ? hc - 1u : hc);

  std::counting_semaphore<> sem( static_cast<std::ptrdiff_t>( max_threads ) );
  /*********************************************************************************************************************************
  **  End Multi Threaded Overhead
  *********************************************************************************************************************************/








  // Note - Be patient:  You won't see any output from these scenarios until all threads have completed, at which point the captured output from
  //        each thread will be emitted in scheduling order.  Be patient, especially if you have a large quantity of food items in your
  //        scenarios. For example, with 30 food items, the exhaustive pattern has to evaluate 2^30 - 1 = 1,073,741,823 candidate
  //        solutions, so it will take a while (over an hour by some estimates) to complete.

  const std::size_t interval = baseline_repository.size() / 10;
  unsigned          scenario = 1;

  for( auto quantity = interval;  quantity <= baseline_repository.size();  quantity += interval, ++scenario )
  {
    // Get a capture buffer for this scenario and add it to the vector so we can emit the captured output in scheduling order after all
    // threads have completed.  Note that the buffers are owned by the main thread, but will be written to by the worker threads, so we
    // need to use a thread-safe smart pointer type (unique_ptr) to manage them.
    out_bufs.emplace_back( std::make_unique<std::stringbuf>() );
    log_bufs.emplace_back( std::make_unique<std::stringbuf>() );
    err_bufs.emplace_back( std::make_unique<std::stringbuf>() );

    auto * out_ptr = out_bufs.back().get();
    auto * log_ptr = log_bufs.back().get();
    auto * err_ptr = err_bufs.back().get();

    // Launch the worker thread for this scenario.
    threads.emplace_back( [quantity, scenario, out_ptr, log_ptr, err_ptr, &sem]()
    {
      // This is what actually gets executed in the worker thread, finally!   Phew, that's a lot of MT overhead!!  Notice the Algorithm
      // Driver function above and hence the algorithm gets executed in the spawned worker thread. Acquire a slot to limit concurrent active
      // threads, then install per-thread targets.
      SemGuard guard( sem );
      struct OnExit
      {
        ~OnExit()
        {
          RedirectBuf<RedirectId::COUT>::clear();
          RedirectBuf<RedirectId::CERR>::clear();
          RedirectBuf<RedirectId::CLOG>::clear();
        }
      } on_exit;

      RedirectBuf<RedirectId::COUT>::set( out_ptr );
      RedirectBuf<RedirectId::CERR>::set( err_ptr );
      RedirectBuf<RedirectId::CLOG>::set( log_ptr );

      std::print( std::cout, "\n\n\nScenario {}:  Sorting {:L} words\n\n", scenario, quantity );

      Algorithm_1 selection_sort;
      execute( selection_sort, quantity );

      Algorithm_2 merge_sort_vector;
      execute( merge_sort_vector, quantity );

      Algorithm_3 merge_sort_list;
      execute( merge_sort_list, quantity );

      Algorithm_4 quick_sort;
      execute( quick_sort, quantity );

      std::print( std::cout, "\n\n\n\n\n\n" );
    }                                                                           // Lambda function for thread
    );                                                                          // threads.emplace_back(...)
  }                                                                             // for each scenario





  // All threads scheduled; now collect them.
  for( auto & t : threads ) if ( t.joinable() ) t.join();

  // Restore original global streambufs so we can emit captured output.
  std::cout.rdbuf( orig_cout_buf );
  std::clog.rdbuf( orig_clog_buf );
  std::cerr.rdbuf( orig_cerr_buf );

  // Emit captured outputs in scheduling order.
  for( std::size_t i = 0; i < out_bufs.size(); ++i )
  {
    if ( out_bufs[i] ) std::cout << out_bufs[i]->str();
    if ( log_bufs[i] ) std::clog << log_bufs[i]->str();
    if ( err_bufs[i] ) std::cerr << err_bufs[i]->str();
  }
}
