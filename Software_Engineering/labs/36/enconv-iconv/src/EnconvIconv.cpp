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
#include "nsIExtensionManager.h"
#include "prlink.h"

NS_IMPL_ISUPPORTS1(EnconvIconv, IEnconvIconv)

EnconvIconv::EnconvIconv()
{
  std::cout << "EnconvIconv::EnconvIconv()" << std::endl; // DEBUG
}

nsresult EnconvIconv::init()
{
  // TODO: Return value corresponds to internal error. Maybe only return error
  // should be "Failed to initialize"?
  
  std::cout << "EnconvIconv::init()" << std::endl; // DEBUG

  nsresult rv;

  nsCOMPtr<nsIServiceManager> svcMgr;
  rv = NS_GetServiceManager(getter_AddRefs(svcMgr));
  NS_ENSURE_SUCCESS(rv, rv);

  if (0)
  {
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
  }
  
  if (1)
  {
    nsCOMPtr<nsIExtensionManager> extMgr;
    rv = svcMgr->GetServiceByContractID("@mozilla.org/extensions/manager;1", 
      NS_GET_IID(nsIExtensionManager), getter_AddRefs(extMgr));
    NS_ENSURE_SUCCESS(rv, rv);
    
    // Obtain addon location.
    // Done similar to method in 
    // http://github.com/linkinpark342/firefoxnotify/blob/master/src/chrome/content/overlay.js
    // TODO: Not sure that this is correct method.
    
    NS_NAMED_LITERAL_STRING(enconvAddonID, ENCONV_ADDON_ID);
    
    nsCOMPtr<nsIInstallLocation> instLoc;
    rv = extMgr->GetInstallLocation(enconvAddonID, getter_AddRefs(instLoc));
    NS_ENSURE_SUCCESS(rv, rv);
    NS_ENSURE_TRUE(instLoc, NS_ERROR_FAILURE);
    
    nsCOMPtr<nsIFile> instLoc2;
    rv = instLoc->GetItemLocation(enconvAddonID, getter_AddRefs(instLoc2));
    NS_ENSURE_SUCCESS(rv, rv);
    
    nsAutoString uniStr;
    rv = instLoc2->GetPath(uniStr);
    NS_ENSURE_SUCCESS(rv, rv);
    
    nsCAutoString cStr;
    rv = NS_UTF16ToCString(uniStr, NS_CSTRING_ENCODING_UTF8, cStr);
    NS_ENSURE_SUCCESS(rv, rv);

    std::cout << cStr.get() << std::endl;
  }

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
