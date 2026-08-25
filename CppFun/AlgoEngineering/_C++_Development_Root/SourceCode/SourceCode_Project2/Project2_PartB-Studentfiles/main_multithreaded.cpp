#include <algorithm>                                                                      // ranges:: sort()
#include <cstddef>                                                                        // ptrdiff_t, size_t
#include <cstdio>                                                                         // EOF  (should use traits_type::eof() instead)
#include <exception>                                                                      // exception, what()
#include <ios>                                                                            // streamsize
#include <iostream>                                                                       // print, cout, clog, cerr, ios::unitbuf
#include <limits>                                                                         // numeric_limits
#include <memory>                                                                         // unique_ptr, make_unique()
#include <semaphore>                                                                      // counting_semaphore
#include <sstream>                                                                        // ostringstream, stringbuf
#include <streambuf>                                                                      // streambuf
#include <thread>                                                                         // thread, hardware_concurrency()
#include <tuple>                                                                          // tuple
#include <typeinfo>                                                                       // typeid()::name()
#include <vector>                                                                         // vector

#include "Algorithm.hpp"
#include "FoodItem.hpp"
#include "FoodPantry.hpp"
#include "Timer.hpp"







namespace    // unnamed, anonymous namespace
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
  void run( const Algorithm & algo, unsigned weight_limit )
  {
    try
    {
      // Display the initial state
      std::print( std::cout,
                  "{:=>40}\n"
                  "{}\n"
                  "Starting execution\n"
                  "{:.>25}\n",
                  "", algo.description(), "" );



      // Execute the algorithm and measure wall clock execution time.
      FoodPantry::SelectedFoodItems results;
      std::ostringstream            ostring;

      Utilities::Timer( "  ", ostring ),  results = algo.run( weight_limit );



      // Display results
      //  Sort only the The Exhaustive Pattern results and be sure to leave The Greedy Pattern results alone
      //
      //  After some thought, it makes comparing the results of the two algorithms a lot easier when they're sorted the same way.
      //  So for now ...
      //if( dynamic_cast<Algorithm_2 const *>( &algo ) != nullptr )
      {
        std::ranges::sort( results, []( FoodItem const * lhs, FoodItem const * rhs ) noexcept
                                      { return lhs->weight() > rhs->weight()
                                               || ( lhs->weight() == rhs->weight() && lhs->calories() > rhs->calories() );
                                      } );
      }

      std::print( std::cout,
                  "{:.>25}\n"
                  "Execution completed\n\n"
                  "Execution time          :{}"
                  "Number of selected items:  {} of {}\n"
                  "Total weight            :  {} of {}\n"
                  "Total calories          :  {:.2f}\n"
                  "Selected Food Items     :\n"
                  "{}"
                  "{:=>40}\n\n\n",
                  "", ostring.str(), results.size(), algo.inventory().size(), results.weight(), weight_limit, results.calories(), results, "");
    }
    catch (std::exception & ex)
    {
      std::print( std::cerr, "\n\n*** Error, unexpected exception caught\n{}\n{}\n\n", typeid( ex ).name(), ex.what() );
      throw;
    }
  }
}  // namespace







int main()
{
  Utilities::Timer timer( "Total execution time:  " );
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








  // Each scenario is a combination of an inventory and a weight limit.  Execute each scenario as a separate thread so we can reduce the
  // amount of total elapsed time by running them concurrently.
  unsigned scenario_counter = 0;

  // for each scenario
  unsigned weight_limit = 0;
  unsigned min_weight   = 0;
  unsigned max_weight   = 0;
  unsigned quantity     = 0;

  // Read scenario parameters from standard input until EOF.  The easiest way to do this is use file redirection at the command line. For
  // example, main_multithreaded.exe < scenarios.txt
  //
  // Note - Be patient:  You won't see any output from these scenarios until all threads have completed, at which point the captured output from
  //        each thread will be emitted in scheduling order.  Be patient, especially if you have a large quantity of food items in your
  //        scenarios. For example, with 30 food items, the exhaustive pattern has to evaluate 2^30 - 1 = 1,073,741,823 candidate
  //        solutions, so it will take a while (over an hour by some estimates) to complete.


  std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );    // Ignore the first line of input which is just a header line for the columns of data
  while( std::cin >> weight_limit >> min_weight >> max_weight >> quantity )
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
    threads.emplace_back( [scenario_id = ++scenario_counter, weight_limit, min_weight, max_weight, quantity, out_ptr, log_ptr, err_ptr, &sem]()
    {
      // This is what actually gets executed in the worker thread, finally!   Phew, that's a lot of MT overhead!!  Notice the Algorithm
      // Driver function above and hence the algorythem gets executed in the spawned worker thread. Acquire a slot to limit concurrent active
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

      auto inventory = FoodPantry::make_inventory( { min_weight, max_weight }, quantity );

      Algorithm_1 greedy    { inventory };
      Algorithm_2 exhaustive{ inventory };

      std::print( std::cout,
                  "Scenario             : {}\n"
                  "Per item weight range: {}..{}\n"
                  "Weight limit         : {}\n"
                  "Selection pool size  : {}\n"
                  "Selection Pool       :\n"
                  "{}\n",
                  scenario_id, min_weight, max_weight, weight_limit, quantity, inventory );
      run( greedy,     weight_limit );
      run( exhaustive, weight_limit );

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
