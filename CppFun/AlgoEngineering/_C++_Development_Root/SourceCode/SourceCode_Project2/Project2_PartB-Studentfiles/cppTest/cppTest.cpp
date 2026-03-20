#include <print>
  // std::print
#include <iostream>
  // std::cout
  //
// Notes on std::print and cpp's formatter
/* 
 * https://en.cppreference.com/w/cpp/utility/format/basic_format_string.html
 * 	- {id : format spec} 
 * https://en.cppreference.com/w/cpp/utility/format/spec.html
 *	- format specifications
 *
 * basic_format_string, format_string, wformat_string
 * { arg-id (optional) : format-spec }
 * 	Example: {0:>} 
 * 	- '0' means 0'th index. 
 * 	- ':' separates the index id from the format spec
 * 	- '>' is part of the format spec. It means right justified.
 *
 * *************************************************************************
 *
 * https://en.cppreference.com/w/cpp/io/print.html
 * 
 * std::print example
 * std::print( filestream, format_strings, arguments)
 * std::print(std::cout, "{:<12.5} {:>10.9s}\n", 1.234567, "I want a box of chocolates");
 * 	- {:<12.5}
 *        - No index before :
 *        - '<' means left justified
 *        - '12' means width of 12 spaces.
 *        - '.5' means a precision (or length) of 5
 *      - {:>12.5s} 
 *        - No index before :
 *        - '>' means right justified
 *        - '10' means width of 10 spaces.
 *        - '.9' means a precision (or length) of 9 
 *      - 1.234567
 *        - First argument at index id of 0. double.
 *      - "I want a box of chocolates."
 *        - Second argument at index id of 1. string literal.
 *      - "\n"
 *        - New line after all arguments are output.
 *      - Final output
 *        - 1.234        I want a   
 *
 */ 
int main(void)
{
  double doubNum = 1.23456789;
  std::print(std::cout, "Print 1\n");
  std::print(std::cout, "{:>1}\n", "Print 1");
  std::print(std::cout, "{:>14} {:>8}\n", "zipZop", "2222");
  std::print(std::cout, "{:<13} {:<13.5}\n", doubNum, doubNum);

  std::print(std::cout, "{:<12.5} {:>10.9s}\n", 1.234567, "I want a box of chocolates");
  
  return 0;
}
