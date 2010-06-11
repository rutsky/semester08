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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <cstddef>

namespace function
{
  typedef double (*func_t)( double x, double y );

  
  //
  // Input functions.
  //
  extern double f1 ( double x, double y );
  extern double f2 ( double x, double y );
  extern double f3 ( double x, double y );
  extern double f4 ( double x, double y );
  extern double f5 ( double x, double y );
  extern double f6 ( double x, double y );
  extern double f7 ( double x, double y );
  extern double f8 ( double x, double y );
  extern double f9 ( double x, double y );
  extern double f10( double x, double y );
  extern double f11( double x, double y );
  extern double f12( double x, double y );
  extern double f13( double x, double y );
  extern double f14( double x, double y );
  extern double f15( double x, double y );
  extern double f16( double x, double y );
  
  extern size_t const nFunctions;
  extern func_t functions[16];
}

#endif // FUNCTION_H
