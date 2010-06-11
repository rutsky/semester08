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

#ifndef UTIL_H
#define UTIL_H

namespace util
{
  template< class T >
  inline 
  int sign( T v )
  {
    return (v >= 0) ? (v > 0 ? +1 : 0) : -1; 
  }
  
  template< class T >
  inline
  T & make_max( T &v, T const &x )
  {
    if (v < x)
      v = x;
    return v;
  }
  
  template< class T >
  inline
  T & make_min( T &v, T const &x )
  {
    if (v > x)
      v = x;
    return v;
  }
}

#endif // UTIL_H
