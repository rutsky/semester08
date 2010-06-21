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
#include <errno.h>

#include "EnconvCharDetector.h"

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
#include "nsIXULRuntime.h"
#include "nsCRT.h"
#include "nsMemory.h"
#include "prlink.h"

NS_IMPL_ISUPPORTS1(EnconvCharDetector, IEnconvCharDetector)

EnconvCharDetector::EnconvCharDetector()
{
  std::cout << "EnconvCharDetector::EnconvCharDetector()" << std::endl; // DEBUG
}


nsresult EnconvCharDetector::init()
{
  // TODO: Return value corresponds to internal error. Maybe only return error
  // should be "Failed to initialize"?
  // TODO: Return appropriate error state.
  
  std::cout << "EnconvCharDetector::init()" << std::endl; // DEBUG

  nsresult rv;

  nsCOMPtr<nsIServiceManager> svcMgr;
  rv = NS_GetServiceManager(getter_AddRefs(svcMgr));
  NS_ENSURE_SUCCESS(rv, rv);

  // NOTE: Using not frozen interface!
  nsCOMPtr<nsIExtensionManager> extMgr;
  rv = svcMgr->GetServiceByContractID("@mozilla.org/extensions/manager;1",
    NS_GET_IID(nsIExtensionManager), getter_AddRefs(extMgr));
  NS_ENSURE_SUCCESS(rv, rv);

  // Obtain addon installation directory (as described on
  // https://developer.mozilla.org/en/Code_snippets/File_I%2F%2FO )
  nsCOMPtr<nsIFile> instPath;
  {
    NS_NAMED_LITERAL_STRING(enconvAddonID, ENCONV_ADDON_ID);

    // Get addon installation location.
    nsCOMPtr<nsIInstallLocation> instLoc;
    rv = extMgr->GetInstallLocation(enconvAddonID, getter_AddRefs(instLoc));
    NS_ENSURE_SUCCESS(rv, rv);

    // Test that installation location is known.
    NS_ENSURE_TRUE(instLoc, NS_ERROR_FAILURE);

    // Get addon installation location path.
    rv = instLoc->GetItemLocation(enconvAddonID, getter_AddRefs(instPath));
    NS_ENSURE_SUCCESS(rv, rv);
  }

  // Construct path to default frequencies table file.
  nsCOMPtr<nsIFile> path;
  {
    rv = instPath->Clone(getter_AddRefs(path));
    NS_ENSURE_SUCCESS(rv, rv);

    nsEmbedCString fileName("ru_freqs.txt");

    path->AppendNative(fileName);
  }

  rv = this->LoadFreqTable(path);
  NS_ENSURE_SUCCESS(rv, rv);

  return rv;
}

EnconvCharDetector::~EnconvCharDetector()
{
  std::cout << "EnconvCharDetector::~EnconvCharDetector()" << std::endl; // DEBUG
}

NS_IMETHODIMP
EnconvCharDetector::LoadFreqTable( nsIFile *file )
{
  nsresult rv;
  
  // Check file existance.
  {
    PRBool exists;
    rv = file->Exists(&exists);
    NS_ENSURE_SUCCESS(rv, rv);

    // Test that libraries directory exists.
    NS_ENSURE_TRUE(exists, NS_ERROR_FAILURE);
  }

  // TODO

  return NS_OK;
}

NS_IMETHODIMP
EnconvCharDetector::GuessEncoding( nsAString const &text, nsACString &encoding )
{
  // TODO
  
  return NS_OK;
}
