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

#include "EnconvIconv.h"

#include "nsStringAPI.h"
#include "nsAutoPtr.h"

NS_IMPL_ISUPPORTS1(EnconvIconv, IEnconvIconv)

EnconvIconv::EnconvIconv()
{
}

EnconvIconv::~EnconvIconv()
{
}

NS_IMETHODIMP
EnconvIconv::ListEncodings( nsACString &encodingsList )
{
  return NS_OK;
}

NS_IMETHODIMP
EnconvIconv::Iconv( nsACString const &fromEncoding, nsACString const &toEncoding, 
                    nsACString const &sourceText, nsACString &resultText )
{
  return NS_OK;
}