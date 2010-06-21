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

#define ENCONV_ICONV_CONTRACTID "@enconv.sourceforge.net/enconv/iconv"
#define ENCONV_ICONV_CLASSNAME "Enconv libiconv wrapper"

// 889580bd-eef4-4309-8d7a-b75bc620e25d
#define ENCONV_ICONV_CID { 0x889580bd, 0xeef4, 0x4309, \
                           { 0x8d, 0x7a, 0xb7, 0x5b, 0xc6, 0x20, 0xe2, 0x5d } }

// TODO: Get addon ID from outside. E.g. create config.h.in.
#define ENCONV_ADDON_ID "enconv@enconv.sourceforge.net"

class PRLibrary;

class EnconvIconv : public IEnconvIconv
{
public:
  EnconvIconv();
  
  nsresult init();

  NS_DECL_ISUPPORTS
  NS_DECL_IENCONVICONV
  
private:
  ~EnconvIconv();

private:
  nsresult iconvImpl( nsACString const &fromEncoding,
                      nsACString const &toEncoding,
                      nsACString const &sourceText, nsACString &resultText );

private:
  // Iconv library functions.
  // TODO: Include definitions from original iconv.h
  typedef void* iconv_t;
  
  typedef int (*iconvlist_callback_func_t)( unsigned int namescount,
    const char * const *names, void *data );

  typedef void (*iconvlist_func_t)( iconvlist_callback_func_t do_one,
    void* data );

  typedef iconv_t (*iconv_open_func_t)( const char* tocode,
    const char* fromcode );

  typedef size_t (*iconv_func_t)( iconv_t cd, char const **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft );

  typedef int (*iconv_close_func_t)( iconv_t cd );
  
  iconvlist_func_t   iconvlist_;
  iconv_open_func_t  iconv_open_;
  iconv_func_t       iconv_;
  iconv_close_func_t iconv_close_;

private:
  PRLibrary *iconvLib_;
};

#endif // ENCONV_ICONV_H
