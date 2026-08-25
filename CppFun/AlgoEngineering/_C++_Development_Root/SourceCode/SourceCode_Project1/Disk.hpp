#pragma once

#include <format>                                                                       // format, formatter
#include <iostream>                                                                     // cin, cout, ios::failbit, ws, istream, ostream
#include <string>                                                                       // string
#include <utility>                                                                      // swap





class Disk
{
  friend void swap( Disk & lhs, Disk & rhs ) noexcept;

  public:
    enum Color {LIGHT=0, DARK};

    // Constructors
    Disk( Color color = LIGHT ) : _color{ color } {}

    // Compiler synthesized copy and move assignment operators, and the destructor are just what we want


    // Accessors
    Color color() const noexcept     { return _color; }

    // Mutators
    Disk & color( Color newColor )   { _color = newColor; return *this; }

    // Relational operators
    auto operator<=>( const Disk & ) const noexcept = default;        // assumes enumerations are listed in ascending order

  private:
    Color _color;
};





inline void swap( Disk & lhs, Disk & rhs ) noexcept
{ std::swap( lhs._color, rhs._color ); }





// Specialize the formatter to accept and process Disk typed objects
template<>
struct std::formatter<Disk> : std::formatter<std::string>
{
  auto format( const Disk & disk, auto & ctx ) const
  {
    const std::string str = disk.color() == Disk::Color::LIGHT ? ( mode == UTF8 ? "\u26AA" : "L" )    // ⚪
                          : disk.color() == Disk::Color::DARK  ? ( mode == UTF8 ? "\u26AB" : "D" )    // ⚫
                          :                                      "?";
    return std::formatter<std::string>::format( str, ctx );
  }

  enum {ASCII, UTF8} inline static mode = UTF8;
};





inline std::istream & operator>>( std::istream & theStream, Disk & disk )
{
  char      chr{ '\0' };

  if (theStream >> std::ws >> chr)
  {
    if     ( chr == 'L' || chr == 'l' ) disk.color( Disk::LIGHT);
    else if( chr == 'D' || chr == 'd' ) disk.color( Disk::DARK );
    else                                theStream.setstate( std::ios::failbit );
  }
  return theStream;
}





inline std::ostream & operator<<( std::ostream & theStream, const Disk & disk )
{
  return theStream << std::format( "{}", disk );
}
