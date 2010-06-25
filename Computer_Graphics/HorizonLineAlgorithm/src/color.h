/*
 *  This file is part of FHA - Floating Horizon Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  FHA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FHA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FHA.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COLOR_H
#define COLOR_H

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>

namespace color
{
  typedef boost::uint32_t color_t;
  
  inline
  color_t makeRGB( int r, int g, int b, int a = 0 )
  {
#if   (BOOST_BYTE_ORDER == 4321)
    return
      (static_cast<uint8_t>(r & 0xFF) << 24) |
      (static_cast<uint8_t>(g & 0xFF) << 16) |
      (static_cast<uint8_t>(b & 0xFF) <<  8) |
      (static_cast<uint8_t>(a & 0xFF) <<  0);
#elif (BOOST_BYTE_ORDER == 1234)
    return
      (static_cast<boost::uint8_t>(r & 0xFF) <<  0) |
      (static_cast<boost::uint8_t>(g & 0xFF) <<  8) |
      (static_cast<boost::uint8_t>(b & 0xFF) << 16) |
      (static_cast<boost::uint8_t>(a & 0xFF) << 24);
#else
# error Unsupported byte order in color.h
#endif
  }
  
  template< int n >
  inline 
  int getComponent( color_t c )
  {
    //STATIC_ASSERT(n >= 0 && n <= 3); // TODO: Compile errors.
    
#if   (BOOST_BYTE_ORDER == 4321)
    return (c >> ((3 - n) * 8)) & 0xFF;
#elif (BOOST_BYTE_ORDER == 1234)
    return (c >> (n * 8)) & 0xFF;
#else
# error Unsupported byte order in color.h
#endif
  }
  
  inline int getR( color_t c ) { return getComponent<0>(c); }
  inline int getG( color_t c ) { return getComponent<1>(c); }
  inline int getB( color_t c ) { return getComponent<2>(c); }
  inline int getA( color_t c ) { return getComponent<3>(c); }
  
  inline
  color_t makeRGB( int v )
  {
    return makeRGB(v, v, v);
  }
  
  class Converter
  {
  public:
    Converter()
      : r()
      , g()
      , b()
      , a()
    {
    }
    
    Converter( color_t c )
    {
      load(c);
    }
    
    Converter( int newR, int newG, int newB, int newA )
      : r(newR)
      , g(newG)
      , b(newB)
      , a(newA)
    {
    }
    
    operator color_t () const
    {
      return makeRGB(r, g, b, a);
    }
    
    void load( color_t c )
    {
      r = getR(c);
      g = getG(c);
      b = getB(c);
      a = getA(c);
    }
    
    int r, g, b, a;
  };
  
  inline color_t white         () { return makeRGB(255); }
  inline color_t black         () { return makeRGB(  0); }
  inline color_t red           () { return makeRGB(255,   0,   0); }
  inline color_t green         () { return makeRGB(  0, 255,   0); }
  inline color_t blue          () { return makeRGB(  0,   0, 255); }
  inline color_t grey          () { return makeRGB(127, 127, 127); }
  inline color_t violet        () { return makeRGB(128,   0, 255); }
  inline color_t orange        () { return makeRGB(255, 128,   0); }
  inline color_t darkBrown     () { return makeRGB(101,  67,  33); }
  inline color_t darkOliveGreen() { return makeRGB( 85, 107,  47); }
  inline color_t darkSlateGrey () { return makeRGB( 47,  79,  79); }
  inline color_t bulgarianRose () { return makeRGB( 72,   6,   7); }
  inline color_t crimsonGlory  () { return makeRGB(190,   0,  50); }
  inline color_t coquelicot    () { return makeRGB(255,  56,   0); }
}
  
#endif // COLOR_H
