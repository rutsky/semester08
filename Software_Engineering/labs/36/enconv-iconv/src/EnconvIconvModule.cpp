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

#include <iostream>

#include "nsIGenericFactory.h"
#include "EnconvIconv.h"

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(EnconvIconv, init)

static 
NS_METHOD 
EnconvIconvRegistrationProc( nsIComponentManager *aCompMgr,
                             nsIFile *aPath,
                             char const *registryLocation,
                             char const *componentType,
                             nsModuleComponentInfo const *info )
{
  //std::cout << "EnconvIconvRegistrationProc()" << std::endl;
  return NS_OK;
}

static 
NS_METHOD 
EnconvIconvUnregistrationProc( nsIComponentManager *aCompMgr,
                               nsIFile *aPath,
                               char const *registryLocation,
                               nsModuleComponentInfo const *info )
{
  // Never show. Why?
  //std::cout << "EnconvIconvUnregistrationProc()" << std::endl;
  return NS_OK;
}

static nsModuleComponentInfo components[] =
{
  {
    ENCONV_ICONV_CLASSNAME,
    ENCONV_ICONV_CID,
    ENCONV_ICONV_CONTRACTID,
    EnconvIconvConstructor,
    EnconvIconvRegistrationProc,
    EnconvIconvUnregistrationProc,
  }
};

NS_IMPL_NSGETMODULE(EnconvIconvModule, components)
