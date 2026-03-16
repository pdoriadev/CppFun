#pragma once

#include <format>                                                                         // formatter, format(), format_to()
#include <iomanip>                                                                        // quoted
#include <iostream>                                                                       // istream, ostream, cin, cout, ios, ws
#include <iterator>                                                                       // ostreambuf_iterator
#include <string>                                                                         // string
#include <utility>                                                                        // move(), pair






class FoodItem
{
  friend std::istream & operator>>( std::istream & theStream,       FoodItem & item );
  friend std::ostream & operator<<( std::ostream & theStream, const FoodItem & item );

  public:
    using WeightRange = std::pair<unsigned, unsigned>;

    // Constructors and assignment operators
    FoodItem( std::string description = "Unspecified Food Item", unsigned weight_ounces = 0, double calories = 0.0 )
    : _calories     (            calories        ),
      _weight_ounces(            weight_ounces   ),
      _description  ( std::move( description   ) )
    {}



    // Accessors
    std::string const & description()  const noexcept    { return _description;   }
    unsigned            weight     ()  const noexcept    { return _weight_ounces; }
    double              calories   ()  const noexcept    { return _calories;      }



    // Queries
    bool is_valid() const noexcept { return !_description.empty() && _weight_ounces > 0 && _calories >= 0.0; }



    // Relational operators
    auto operator<=>( FoodItem const & ) const noexcept = default;



  private:
    // Place the most likely and fastest to check attributes first.
    double      _calories;
    unsigned    _weight_ounces;
    std::string _description;
};






// Specialize the formatter to format FoodItem typed objects
template<>
struct std::formatter<FoodItem> : std::formatter<std::string>
{
  auto format( const FoodItem & item, auto & ctx ) const
  {
    // I didn't use the foarmat_to version here because I really did want to keep the client's ability to use string's format
    // specifiers, especially the field width.
    const std::string str = std::format( "{:?}, {}, {:.2f}", item.description(), item.weight(), item.calories() );
    return std::formatter<std::string>::format( str, ctx );
  }
};






// Extraction operator
inline std::istream & operator>>( std::istream & theStream, FoodItem & item )
{
  FoodItem temp;
  char     delim = '\0';

  if(    theStream >> std::ws >> std::quoted( temp._description )
      && theStream >> std::ws >> delim && delim == ','
      && theStream >> std::ws >> temp._weight_ounces
      && theStream >> std::ws >> delim && delim == ','
      && theStream >> std::ws >> temp._calories )
  {
    //temp.validate();
    item = std::move( temp );
  }
  else  theStream.setstate( std::ios::failbit );

  return theStream;
}






// Insertion operator
inline std::ostream & operator<<( std::ostream & theStream, const FoodItem & item )
{
  // This is easier to read and understand, but it can be optimized a little bit by avoiding the temporary string object created by
  // std::format() and instead writing directly to the stream using std::format_to() and an ostreambuf_iterator
  //   return theStream << std::format( "{}", item );

  std::format_to( std::ostreambuf_iterator<std::ostream::char_type>( theStream ), "{}", item );
  return theStream;
}
