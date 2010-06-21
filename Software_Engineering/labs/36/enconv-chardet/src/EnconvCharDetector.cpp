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

#include <sstream>
#include <fstream>

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
#include "nsILocalFile.h"
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

    nsEmbedCString dataName("data");
    nsEmbedCString fileName("ru_freqs.txt");

    path->AppendNative(dataName);
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

#if 0
  {
    // Shit load of fuck.

    nsCOMPtr<nsILocalFile> localFile = do_QueryInterface(file);
    if (!localFile)
      return -1;
    
    char *buf = NULL;

    // determine file size:
    PRUint32 fs, numread;
    PRInt64 fileSize;
    rv = file->GetFileSize(&fileSize);
    if (NS_FAILED(rv))
      return -1;

    // Converting 64 bit value to unsigned int
    LL_L2UI(fs, fileSize);

    FILE* openFile;
    rv = localFile->OpenANSIFileDesc("rw", &openFile);
    if (NS_FAILED(rv))
      return -1;

    buf = (char *)malloc((fs+1) * sizeof(char));
    if (!buf)
      return -1;

    numread = fread(buf, sizeof(char), fs, openFile);

    if (numread != fs)
    {
      free(buf);
      return -1;
    }

    std::cout << "buf: '" << buf << "'" << std::endl;

    // Process.
    std::istringstream ()
    wchar_t ch;
    double freq;
    swscanf((wchar_t *)buf, L"%c %lf", &ch, &freq);
    std::wcout << L"ch: '" << ch << L"', freq: '" << freq << L"'\n";
    
    if (buf)
      free(buf);
  }
#endif

#if 0
  {
    nsCOMPtr<nsIServiceManager> svcMgr;
    rv = NS_GetServiceManager(getter_AddRefs(svcMgr));
    NS_ENSURE_SUCCESS(rv, rv);

    nsCOMPtr<nsIFileInputStream> fstream;
    rv = svcMgr->GetServiceByContractID("@mozilla.org/network/file-input-stream;1",
      NS_GET_IID(nsIFileInputStream), getter_AddRefs(fstream));
    NS_ENSURE_SUCCESS(rv, rv);

    fstream.init(file, -1, 0, 0);
#endif

  {
    nsCAutoString path;
    rv = file->GetNativePath(path);
    NS_ENSURE_SUCCESS(rv, rv);

    // Load table from file.
    std::ifstream input(path.BeginReading(), std::ifstream::binary);

    size_t const bufSize = 64;
    char buf[bufSize];
    buf[bufSize - 1] = 0;

    std::cout << "Loading frequency table from: '" << path.BeginReading() << "'... (" << input.good() << ")" << std::endl;

    /*
    std::wstring ws;
    input >> ws;
    std::wcout << "ws: '" << ws << "'" << std::endl;
    */
    
    freq_table_t newFreqTable;
    while (input.getline(buf, bufSize - 1).good())
    {
      std::istringstream istr(buf);
      wchar_t ch;
      double freq;
      istr.read((char *)&ch, 2) >> freq;

      // Test that character not in table.
      //NS_ENSURE_SUCCESS((newFreqTable.find(ch) == newFreqTable.end()),
      //  NS_ERROR_FAILURE);

      //std::cout << "test" << std::endl;
      std::cout << "ch: '" << ch << "', freq: '" << freq << "'" << std::endl;

      newFreqTable.insert(std::make_pair(ch, freq));
    }

    //std::cout << "buf: '" << buf << "'" << std::endl;

    // Test that loaded table not empty.
    NS_ENSURE_TRUE(!newFreqTable.empty(), NS_ERROR_FAILURE);
    
    // Store successfully loaded table.
    freqTable_.swap(newFreqTable);
  }

  return NS_OK;
}

NS_IMETHODIMP
EnconvCharDetector::GuessEncoding( nsAString const &text, nsACString &encoding )
{
  // TODO
  
  return NS_OK;
}
