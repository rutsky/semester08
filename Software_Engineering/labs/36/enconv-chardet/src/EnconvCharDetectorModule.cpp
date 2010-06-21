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

#include "nsIGenericFactory.h"
#include "EnconvCharDetector.h"

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(EnconvCharDetector, init)

static nsModuleComponentInfo components[] =
{
  {
    ENCONV_CHARDETECTOR_CLASSNAME,
    ENCONV_CHARDETECTOR_CID,
    ENCONV_CHARDETECTOR_CONTRACTID,
    EnconvCharDetectorConstructor,
    NULL,
    NULL,
  }
};

NS_IMPL_NSGETMODULE(EnconvCharDetectorModule, components)
