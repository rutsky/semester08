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

#include <iostream> // DEBUG

#include "EnconvIconv.h"

#include "nsXPCOM.h"
#include "nsIServiceManager.h"
#include "nsStringAPI.h"
#include "nsEmbedString.h"
#include "nsAutoPtr.h"
#include "nsCOMPtr.h"
#include "nsDirectoryServiceDefs.h"
#include "nsIProperties.h"
#include "nsIFile.h"
#include "prlink.h"

NS_IMPL_ISUPPORTS1(EnconvIconv, IEnconvIconv)

EnconvIconv::EnconvIconv()
{
  std::cout << "EnconvIconv::EnconvIconv()" << std::endl; // DEBUG
}

nsresult EnconvIconv::init()
{
  std::cout << "EnconvIconv::init()" << std::endl; // DEBUG

  nsresult rv;

  nsCOMPtr<nsIServiceManager> svcMgr;
  rv = NS_GetServiceManager(getter_AddRefs(svcMgr));
  NS_ENSURE_SUCCESS(rv, rv);

  nsCOMPtr<nsIProperties> directory;
  rv = svcMgr->GetServiceByContractID("@mozilla.org/file/directory_service;1", 
    NS_GET_IID(nsIProperties), getter_AddRefs(directory));
  NS_ENSURE_SUCCESS(rv, rv);

  nsCOMPtr<nsIFile> tmpDir;
  rv = directory->Get(NS_OS_TEMP_DIR, NS_GET_IID(nsIFile), getter_AddRefs(tmpDir));
  NS_ENSURE_SUCCESS(rv, rv);
  
  nsAutoString path;
  rv = tmpDir->GetPath(path);
  NS_ENSURE_SUCCESS(rv, rv);
  
  nsCAutoString cpath;
  rv = NS_UTF16ToCString(path, NS_CSTRING_ENCODING_UTF8, cpath);

  std::cout << cpath.get() << std::endl;

  return rv;
}

EnconvIconv::~EnconvIconv()
{
  std::cout << "EnconvIconv::~EnconvIconv()" << std::endl; // DEBUG
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
