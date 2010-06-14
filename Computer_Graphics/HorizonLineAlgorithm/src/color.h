/*
 *  This file is part of HLA - Horizon Line Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  HLA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HLA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HLA.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COLOR_H
#define COLOR_H

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>

namespace color
{
  typedef uint32_t color_t;
  
  inline
  color_t make_rgb( int r, int g, int b )
  {
    color_t result;
#if   (BOOST_BYTE_ORDER == 4321)
    result =
      (static_cast<uint8_t>(r) << 24) |
      (static_cast<uint8_t>(g) << 16) |
      (static_cast<uint8_t>(b) << 8);
#elif (BOOST_BYTE_ORDER == 1234)
    result =
      (static_cast<uint8_t>(r) << 0) |
      (static_cast<uint8_t>(g) << 8) |
      (static_cast<uint8_t>(b) << 16);
#else
# error Unsupported byte order in color.h
#endif

    return result;
  }
  
  inline
  color_t make_rgb( int v )
  {
    return make_rgb(v, v, v);
  }
  
  
  inline color_t white() { return make_rgb(255); }
  inline color_t black() { return make_rgb(0); }
  inline color_t red  () { return make_rgb(255,   0,   0); }
  inline color_t green() { return make_rgb(  0, 255,   0); }
  inline color_t blue () { return make_rgb(  0,   0, 255); }
  inline color_t gray () { return make_rgb(127, 127, 127); }
}
  
#endif // COLOR_H
