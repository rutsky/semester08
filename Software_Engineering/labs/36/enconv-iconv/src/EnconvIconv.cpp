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
#include "nsIXULRuntime.h"
#include "nsCRT.h"
#include "nsMemory.h"
#include "prlink.h"

NS_IMPL_ISUPPORTS1(EnconvIconv, IEnconvIconv)

EnconvIconv::EnconvIconv()
  : iconvlist_(nsnull)
  , iconv_open_(nsnull)
  , iconv_(nsnull)
  , iconv_close_(nsnull)
  , iconvLib_(nsnull)
{
  std::cout << "EnconvIconv::EnconvIconv()" << std::endl; // DEBUG
}


typedef nsCString * iconvlist_callback_data_t;

static // TODO: Static?
int iconvlistCallback( unsigned int namescount, const char * const *names,
                       void *data )
{
  nsCString &encodingsList = *((iconvlist_callback_data_t)data);

  nsresult rv;
  
  for (unsigned int i = 0; i < namescount; ++i)
  {
    rv = NS_CStringAppendData(encodingsList, names[i],
      (PRUint32)strlen(names[i])); // TODO: Conversion.
    NS_ENSURE_SUCCESS(rv, -1);

    rv = NS_CStringAppendData(encodingsList, "\n",
      (PRUint32)strlen("\n")); // TODO: Conversion.
    NS_ENSURE_SUCCESS(rv, -1);
  }

  rv = NS_CStringAppendData(encodingsList, "\n",
    (PRUint32)strlen("\n")); // TODO: Conversion.
  NS_ENSURE_SUCCESS(rv, -1);

  return 0;
}

nsresult EnconvIconv::init()
{
  // TODO: Return value corresponds to internal error. Maybe only return error
  // should be "Failed to initialize"?
  // TODO: Return appropriate error state.
  
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
    NS_ENSURE_SUCCESS(rv, rv);

    std::cout << cpath.get() << std::endl;
  }
  
  if (1)
  {
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
    
    // Construct platform dependent subpath to libraries.
    nsCAutoString platformName;
    {
      // Note: Using not frozen interface!
      nsCOMPtr<nsIXULRuntime> XULRuntime;
      rv = svcMgr->GetServiceByContractID("@mozilla.org/xre/app-info;1", 
        NS_GET_IID(nsIXULRuntime), getter_AddRefs(XULRuntime));
      NS_ENSURE_SUCCESS(rv, rv);
      
      nsCAutoString targetOS;
      rv = XULRuntime->GetOS(targetOS);
      NS_ENSURE_SUCCESS(rv, rv);
      
      nsCAutoString targetABI;
      rv = XULRuntime->GetXPCOMABI(targetABI);
      NS_ENSURE_SUCCESS(rv, rv);
      
      // OS string should not be empty.
      NS_ENSURE_TRUE((!targetOS.IsEmpty()), NS_ERROR_FAILURE);
      rv = NS_CStringCopy(platformName, targetOS);
      NS_ENSURE_SUCCESS(rv, rv);

      if (!targetABI.IsEmpty())
      {
        rv = NS_CStringAppendData(platformName, "_", 1);
        NS_ENSURE_SUCCESS(rv, rv);
        
        rv = NS_CStringAppendData(platformName, targetABI.get(), targetABI.Length());
        NS_ENSURE_SUCCESS(rv, rv);
      }
    }
    
    // Construct path to addon libraries on current platform.
    nsCOMPtr<nsIFile> path;
    {
      rv = instPath->Clone(getter_AddRefs(path));
      NS_ENSURE_SUCCESS(rv, rv);
      
      nsEmbedCString platformDirName("platform");
      nsEmbedCString librariesDirName("libraries");
      
      path->AppendNative(platformDirName);
      path->AppendNative(platformName);
      path->AppendNative(librariesDirName);
    }
    
    // Check existence of libraries directory.
    {
      PRBool exists;
      rv = path->Exists(&exists);
      NS_ENSURE_SUCCESS(rv, rv);
      
      // Test that libraries directory exists.
      NS_ENSURE_TRUE(exists, NS_ERROR_FAILURE);
    }

    // Convert Unicode path wide string to signle byte string.
    nsCAutoString cpath;
    {
      nsAutoString uniStr;
      rv = path->GetPath(uniStr);
      NS_ENSURE_SUCCESS(rv, rv);
      
      rv = NS_UTF16ToCString(uniStr, NS_CSTRING_ENCODING_UTF8, cpath);
      NS_ENSURE_SUCCESS(rv, rv);
    }

    // DEBUG
    if (1)
    {
      std::cout << cpath.get() << std::endl;
    }

    // Load iconv library.
    {
      // Construct full library path.
      char *libName = PR_GetLibraryName(cpath.get(), "iconv");
      std::cout << "Loading: " << libName << std::endl; // DEBUG

      // Load library.
      iconvLib_ = PR_LoadLibrary(libName);
      PR_FreeLibraryName(libName);

      // Test that loading succeded.
      NS_ENSURE_TRUE(iconvLib_, NS_ERROR_FAILURE);
    }

    // Set up iconv library functions entry points.
    {
      iconvlist_   = (iconvlist_func_t)  PR_FindSymbol(iconvLib_, "libiconvlist");
      NS_ENSURE_TRUE(iconvlist_,   NS_ERROR_FAILURE);
      
      iconv_open_  = (iconv_open_func_t) PR_FindSymbol(iconvLib_, "libiconv_open");
      NS_ENSURE_TRUE(iconv_open_,  NS_ERROR_FAILURE);

      iconv_       = (iconv_func_t)      PR_FindSymbol(iconvLib_, "libiconv");
      NS_ENSURE_TRUE(iconv_,       NS_ERROR_FAILURE);

      iconv_close_ = (iconv_close_func_t)PR_FindSymbol(iconvLib_, "libiconv_close");
      NS_ENSURE_TRUE(iconv_close_, NS_ERROR_FAILURE);

      iconvctl_    = (iconvctl_func_t)   PR_FindSymbol(iconvLib_, "libiconvctl");
      NS_ENSURE_TRUE(iconvctl_,    NS_ERROR_FAILURE);
    }
  }

  return rv;
}

EnconvIconv::~EnconvIconv()
{
  std::cout << "EnconvIconv::~EnconvIconv()" << std::endl; // DEBUG

  if (iconvLib_)
    PR_UnloadLibrary(iconvLib_);
}

NS_IMETHODIMP
EnconvIconv::ListEncodings( nsACString &encodingsList )
{
  NS_ENSURE_TRUE(iconvlist_, NS_ERROR_FAILURE);

  iconvlist_(iconvlistCallback, (void *)&encodingsList);

  return NS_OK;
}

nsresult
EnconvIconv::iconvImpl( nsACString const &toEncoding, nsACString const &fromEncoding,
                        nsACString const &sourceText, nsACString &resultText )
{
  // TODO: Return appropriate error state.
  NS_ENSURE_TRUE(iconv_open_,  NS_ERROR_FAILURE);
  NS_ENSURE_TRUE(iconv_,       NS_ERROR_FAILURE);
  NS_ENSURE_TRUE(iconv_close_, NS_ERROR_FAILURE);

  /*
  std::cout << "iconv() from '" << fromEncoding.BeginReading() <<
    "' to '" << toEncoding.BeginReading() << "' for '" <<
    sourceText.BeginReading() << "'" <<
    std::endl; // DEBUG.*/

  // Open iconv session.
  iconv_t cd = iconv_open_(toEncoding.BeginReading(), fromEncoding.BeginReading());
  NS_ENSURE_TRUE((cd != (iconv_t)-1), NS_ERROR_FAILURE);

  // Allocate memory for result string.
  // TODO: Calculate size more precisely.
  size_t const resultBufSize = sourceText.Length() * 4 + 16;
  char *resultBuf = (char *)nsMemory::Alloc(resultBufSize);
  if (!resultBuf)
  {
    iconv_close_(cd); // TODO: Leak return status.
    return NS_ERROR_FAILURE;
  }

  // Prepare arguments.
  char const *inBuf = sourceText.BeginReading();
  size_t inBytesLeft = sourceText.Length();
  char *outBuf = resultBuf;
  size_t outBytesLeft = resultBufSize;

  // Run iconv.
  while (inBytesLeft > 0)
  {
    if (iconv_(cd,
      &inBuf, &inBytesLeft,
      &outBuf, &outBytesLeft) == (size_t)-1)
    {
      if (errno == EILSEQ)
      {
        // Invalid sequence. Skip it.
        int one = 1;
        iconvctl_(cd, ICONV_SET_DISCARD_ILSEQ, &one);
      }
      else
      {
        iconv_close_(cd); // TODO: Leak return status.
        nsMemory::Free(resultBuf);
        return NS_ERROR_FAILURE;
      }
    }
  }

  // Close iconv session.
  if (iconv_close_(cd))
  {
    nsMemory::Free(resultBuf);
    return NS_ERROR_FAILURE;
  }

  // DEBUG
  resultBuf[resultBufSize - outBytesLeft] = 0;
  /*std::cout << "Result: '" << resultBuf << "'" << std::endl; // DEBUG */

  // Save result.
  nsresult rv = NS_CStringSetData(resultText, resultBuf,
    (PRUint32)(resultBufSize - outBytesLeft));
  NS_ENSURE_SUCCESS(rv, rv);

  // Free result buffer.
  nsMemory::Free(resultBuf);
  
  return NS_OK;
}

NS_IMETHODIMP
EnconvIconv::Iconv( nsACString const &toEncoding, nsACString const &fromEncoding,
                    nsAString const &sourceText, nsAString &resultText )
{
  // TODO: Null-characters missed.
  
  nsresult rv;

  nsCAutoString csourceText;
  rv = NS_UTF16ToCString(sourceText, NS_CSTRING_ENCODING_UTF8, csourceText);
  NS_ENSURE_SUCCESS(rv, rv);
  
  nsCAutoString systemEnc("UTF-8"); // TODO: LE or BE?
  
  nsCAutoString conv1;
  rv = iconvImpl(fromEncoding, systemEnc, csourceText, conv1);
  NS_ENSURE_SUCCESS(rv, rv);

  //nsCAutoString conv2;
  //rv = iconvImpl(fromEncoding, toEncoding, conv1, conv2);
  //NS_ENSURE_SUCCESS(rv, rv);

  nsCAutoString cresultText;
  rv = iconvImpl(systemEnc, toEncoding, conv1, cresultText);
  NS_ENSURE_SUCCESS(rv, rv);

  rv = NS_CStringToUTF16(cresultText, NS_CSTRING_ENCODING_UTF8, resultText);
  NS_ENSURE_SUCCESS(rv, rv);

  return NS_OK;
}
