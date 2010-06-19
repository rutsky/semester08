/*
 *  This file is part of enconv---character encoding converter suite.
 *
 *  Copyright (C) 2010  Vladimir Rutsky
 *
 *  enconv is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  enconv is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with enconv.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCONV_ICONV_H
#define ENCONV_ICONV_H

#include "IEnconvIconv.h"

#define ENCONV_ICONV_CONTRACTID "@enconv.sourceforge.net/Enconv/Iconv;1"
#define ENCONV_ICONV_CLASSNAME "Enconv libiconv wrapper"

// 889580bd-eef4-4309-8d7a-b75bc620e25d
#define ENCONV_ICONV_CID { 0x889580bd, 0xeef4, 0x4309, \
                           { 0x8d, 0x7a, 0xb7, 0x5b, 0xc6, 0x20, 0xe2, 0x5d } }

class EnconvIconv : public IEnconvIconv
{
public:
  EnconvIconv();

  NS_DECL_ISUPPORTS
  NS_DECL_IENCONVICONV
  
private:
  ~EnconvIconv();
};

#endif // ENCONV_ICONV_H
