#pragma once

#include <algorithm>                                                                    // all_of
#include <cstddef>                                                                      // size_t
#include <initializer_list>
#include <vector>

#include "Disk.hpp"



// Disks is-a std::vector meaning that Disks has all the operations std::vector operations has
struct Disks : std::vector<Disk>
{
  Disks( const std::size_t n )
  : std::vector<Disk>{ 2*n, Disk::Color::DARK }
  {
    for( auto i = 0U; i < size(); i += 2 )    at(i) = Disk::Color::LIGHT;
  }


  Disks( const std::initializer_list<Disk> & initializer )
  : std::vector<Disk>{ initializer }
  {}


  bool is_sorted() const
  {
    const auto n = size() / 2;
    return std::all_of( begin(),     begin() + n, []( const Disk & disk ) { return disk.color() == Disk::Color::LIGHT; } )
        && std::all_of( begin() + n, end(),       []( const Disk & disk ) { return disk.color() == Disk::Color::DARK;  } );
  }
};
