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
#include <vector>
#include <cassert>
#include <cmath>

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

// TODO:
#include "../../enconv-iconv-component/IEnconvIconv.h"

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
    freqTableIdxs_.clear();
    freqVec_.resize(freqTable_.size());
    size_t idx(0);
    for (freq_table_t::const_iterator it = freqTable_.begin();
        it != freqTable_.end();
        ++it, ++idx)
    {
      freqTableIdxs_.insert(std::make_pair(it->first, idx));
      freqVec_[idx] = it->second;
    }
  }

  return NS_OK;
}

nsresult
EnconvCharDetector::countFreqs( nsAString const &text, std::vector<double> &freqs )
{
  std::vector<size_t> counts(freqTable_.size(), size_t(0));

  size_t totalCount(0);
  for (PRUnichar const *p = text.BeginReading(); p != text.EndReading(); ++p, ++totalCount)
  {
    freq_table_idxs_t::const_iterator it = freqTableIdxs_.find(*p);
    if (it != freqTableIdxs_.end())
      counts[it->second]++;
  }

  freqs.resize(counts.size());
  for (size_t i = 0; i < counts.size(); ++i)
  {
    if (totalCount)
      freqs[i] = static_cast<double>(counts[i]) / static_cast<double>(totalCount);
  }

  return NS_OK;
}

double
EnconvCharDetector::metric( std::vector<double> const &freqsA,
                            std::vector<double> const &freqsB )
{
  double dist = 0.0;

  assert(freqsA.size() == freqsB.size());
  for (size_t i = 0; i < freqsA.size(); ++i)
    dist += (freqsA[i] - freqsB[i]) * (freqsA[i] - freqsB[i]);

  return sqrt(dist);
}

NS_IMETHODIMP
EnconvCharDetector::GuessConversion( nsAString const &text,
                                     nsACString &toEncoding,
                                     nsACString &fromEncoding )
{
  nsresult rv;

  nsCOMPtr<nsIServiceManager> svcMgr;
  rv = NS_GetServiceManager(getter_AddRefs(svcMgr));
  NS_ENSURE_SUCCESS(rv, rv);

  nsCOMPtr<IEnconvIconv> enconvIconv;
  rv = svcMgr->GetServiceByContractID("@enconv.sourceforge.net/enconv/iconv",
    NS_GET_IID(IEnconvIconv), getter_AddRefs(enconvIconv));
  NS_ENSURE_SUCCESS(rv, rv);

  nsCAutoString encodingGroups;
  rv = enconvIconv->ListEncodings(encodingGroups);
  NS_ENSURE_SUCCESS(rv, rv);

  // Obtain all encodings.
  std::vector<std::string> encodings;
  {
    std::istringstream istr(encodingGroups.BeginReading());

    size_t maxBuf = 64;
    char buf[maxBuf];
    buf[maxBuf - 1] = 0;

    std::string lastStr;
    while (istr.getline(buf, maxBuf - 1))
    {
      std::cout << ">> '" << buf << "'" << std::endl;
      if (buf[0] == 0)
      {
        encodings.push_back(lastStr);
        buf[0] = 0;
      }
      else
      {
        lastStr = std::string(buf);
      }
    }
  }
  std::cout << "Number of different encodings: " << encodings.size() << std::endl;

  size_t bestFromIdx(-1), bestToIdx(-1);
  double bestMetric(0);
  // Iterate through all compinations of convesion.
  for (size_t fromIdx = 0; fromIdx < encodings.size(); ++fromIdx)
    for (size_t toIdx = 0; toIdx < encodings.size(); ++toIdx)
    {
      if (fromIdx == toIdx)
        continue;

      std::cout << "Try: '" << encodings[toIdx] << "' <= '" << encodings[fromIdx] << std::endl;
      
      nsAutoString convertedText;
      nsCAutoString toEnc(encodings[toIdx].c_str(),
                          (PRUint32)encodings[toIdx].length());
      nsCAutoString fromEnc(encodings[fromIdx].c_str(),
                            (PRUint32)encodings[fromIdx].length());
      rv = enconvIconv->Iconv(toEnc, fromEnc, text, convertedText);
      if (NS_FAILED(rv))
      {
        std::cout << "  coversion not available." << std::endl;
        continue;
      }
      else
      {
        std::vector<double> curFreqs;
        countFreqs(convertedText, curFreqs);
        double const curMetric = metric(curFreqs, freqVec_);

        std::cout << "  metric: " << curMetric << " (best: " << bestMetric << ")" << std::endl;

        if (bestFromIdx == (size_t)-1 || bestMetric > curMetric)
        {
          std::cout << "  mark as best." << std::endl;
          bestFromIdx = fromIdx;
          bestToIdx = toIdx;
          bestMetric = curMetric;
        }
      }
    }

  if (bestToIdx != (size_t)-1)
  {
    std::cout << "Best fit conversion from '" << encodings[bestFromIdx] << "' to '" << encodings[bestToIdx] << "'." << std::endl;
    
    toEncoding.Assign(encodings[bestToIdx].c_str(),
                      (PRUint32)encodings[bestToIdx].length());
    fromEncoding.Assign(encodings[bestFromIdx].c_str(),
                        (PRUint32)encodings[bestFromIdx].length());
  }
  
  return NS_OK;
}
