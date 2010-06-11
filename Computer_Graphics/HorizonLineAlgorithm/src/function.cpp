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

#include "function.h"
#include "util.h"

#include <cmath>

namespace
{
  using util::sqr;
  using std::sqrt;
  using std::abs;
  using std::sin;
  using std::cos;
  using std::exp;
  using std::atan;
  using util::pi;
  
  double R2( double x, double y )
  {
    return sqr(x) + sqr(y);
  }
  
  double R( double x, double y )
  {
    return sqrt(R2(x, y));
  }
  
  double ch( double x )
  {
    return (exp(x) + exp(-x)) / 2.0;
  }
  
  double sh( double x )
  {
    return (exp(x) - exp(-x)) / 2.0;
  }
}

namespace function
{
  //
  // Input functions.
  //
  
  double f1( double x, double y )
  {
    return sqrt((1.0 - sqr(R(x, y)) + abs(1.0 - sqr(R(x, y)))) / 2.0);
  }
  
  double f2( double x, double y )
  {
    return 10.0 * sqr(x) + 5.0 * sqr(y);
  }
  
  double f3( double x, double y )
  {
    return sin(x + y) / (1.0 + sqr(x - y));
  }
  
  double f4( double x, double y )
  {
    return cos(x) * cos(y);
  }
  
  double f5( double x, double y )
  {
    return cos((x + 2 * y) / R(x, y));
  }
  
  double f6( double x, double y )
  {
    return -sin(R(x, y)) / R(x, y);
  }
  
  double f7( double x, double y )
  {
    return cos(x - y) / cos(R2(x, y));
  }
  
  double f8( double x, double y )
  {
    return sin(x * y) / R2(x, y);
  }
  
  double f9( double x, double y )
  {
    return sqrt(sqr(cos(x)) + sqr(ch(y)));
  }
  
  double f10( double x, double y )
  {
    return atan(R(x, y));
  }
  
  double f11( double x, double y )
  {
    return cos(x * y / R(x, y));
  }
  
  double f12( double x, double y )
  {
    return sin(x * y / R(x, y));
  }
  
  double f13( double x, double y )
  {
    return cos(3.0 * x * y);
  }
  
  double f14( double x, double y )
  {
    return sqr(x) + cos(12.5 * pi() * y);
  }
  
  double f15( double x, double y )
  {
    return sqrt(sqr(sin(x) + sqr(sh(y))));
  }
  
  double f16( double x, double y )
  {
    return sin(R(x, y));
  }

  size_t const nFunctions = 16;
  
  func_t functions[nFunctions] = {
    f1, f2, f3, f4, f5, f6, f7, f8, 
    f9, f10, f11, f12, f13, f14, f15, f16};
}
