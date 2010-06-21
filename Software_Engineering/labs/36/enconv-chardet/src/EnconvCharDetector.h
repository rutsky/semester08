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

#ifndef ENCONV_CHARDETECTOR_H
#define ENCONV_CHARDETECTOR_H

// TODO
#include <map>

#include "IEnconvCharDetector.h"

#define ENCONV_CHARDETECTOR_CONTRACTID "@enconv.sourceforge.net/enconv/chardet"
#define ENCONV_CHARDETECTOR_CLASSNAME "Enconv character encoding detector"

// 8d836da5-f26b-4233-a28f-a9946b7db8e0
#define ENCONV_CHARDETECTOR_CID { 0x8d836da5, 0xf26b, 0x4233, \
                                 { 0xa2, 0x8f, 0xa9, 0x94, 0x6b, 0x7d, 0xb8, 0xe0 } }

// TODO: Get addon ID from outside. E.g. create config.h.in.
#define ENCONV_ADDON_ID "enconv@enconv.sourceforge.net"

class EnconvCharDetector : public IEnconvCharDetector
{
public:
  EnconvCharDetector();
  
  nsresult init();

  NS_DECL_ISUPPORTS
  NS_DECL_IENCONVCHARDETECTOR
  
private:
  ~EnconvCharDetector();

private:
  typedef std::map<PRUnichar, double> freq_table_t;
  
  freq_table_t freqTable_;
};

#endif // ENCONV_CHARDETECTOR_H
