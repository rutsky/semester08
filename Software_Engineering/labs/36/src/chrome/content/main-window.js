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

// Note: don't use unique namespace since this JS is loaded only in 
// enconv's window.

var enconvIconv = null;
var enconvCharDet = null;

var encodings = [];
var singleEncNameToIdx = {};

var inputText        = null;
var resultText       = null;
var fromEncodingList = null;
var toEncodingList   = null;

// TODO: Better implementation.
function assert(expr)
{
  if (!expr)
    throw "Assertion failed.";
}

function updateConvertedText()
{
  dump("main-window.js::updateConvertedText()\n"); // DEBUG
  
  var inputStr = inputText.value;
  var inputEnc = encodings[fromEncodingList.selectedIndex].split("/")[0];
  //dump("toEncodingList:" + toEncodingList + "\n");
  var resultEnc = encodings[toEncodingList.selectedIndex].split("/")[0];

  var resultStr = "";
  try
  {
    resultStr = enconvIconv.iconv(resultEnc, inputEnc, inputStr);
  }
  catch (e)
  {
    resultStr = "Conversion not available.";
  }

  resultText.value = resultStr;
}

function onLoadFreqTable()
{
  dump("main-window.js::updateConvertedText()\n"); // DEBUG

  nsIFilePicker = Components.interfaces.nsIFilePicker;
  var fp = Components.classes["@mozilla.org/filepicker;1"]
    .createInstance(Components.interfaces.nsIFilePicker);
  
  fp.init(window, "Load Frequencies Table", nsIFilePicker.modeOpen);
  fp.appendFilters(nsIFilePicker.filterAll | nsIFilePicker.filterText);

  var rv = fp.show();
  if (rv == nsIFilePicker.returnOK || rv == nsIFilePicker.returnReplace)
  {
    /*
    var file = fp.file;
    // Get the path as string. Note that you usually won't
    // need to work with the string paths.
    var path = fp.file.path;
    // work with returned nsILocalFile...
    alert(path);*/

    try
    {
      enconvCharDet.loadFreqTable(fp.file);
    }
    catch (e)
    {
      alert("Encoding Converter Error: Failed to load frequency table.");
    }
  }
}

function onUseResultAsInput()
{
  dump("main-window.js::onUseResultAsInput()\n"); // DEBUG

  inputText.value = resultText.value;
  updateConvertedText();
}

function onDetectConversion()
{
  dump("main-window.js::onDetectConversion()\n"); // DEBUG

  var toEnc = {};
  var fromEnc = {};
  enconvCharDet.guessConversion(inputText.value, toEnc, fromEnc);
  if (toEnc.value && fromEnc.value)
  {
    dump("To: " + toEnc.value + "\n");
    dump("From: " + fromEnc.value + "\n");
    var toEncIdx = singleEncNameToIdx[toEnc.value];
    dump("Select to #" + toEncIdx + "\n");
    toEncodingList.ensureIndexIsVisible(toEncIdx);
    toEncodingList.selectedIndex = toEncIdx;

    var fromEncIdx = singleEncNameToIdx[fromEnc.value];
    dump("Select from #" + fromEncIdx + "\n");
    fromEncodingList.ensureIndexIsVisible(fromEncIdx);
    fromEncodingList.selectedIndex = fromEncIdx;

    //updateConvertedText(); // Will update after select events.
  }
  else
  {
    alert("Encoding Converter: Not found any suitable conversion.");
  }
}

function onLoad()
{
  dump("main-window.js::onLoad()\n"); // DEBUG
  try
  {
    inputText        = document.getElementById("input-text");
    resultText       = document.getElementById("result-text");
    fromEncodingList = document.getElementById("from-encoding-list");
    toEncodingList   = document.getElementById("to-encoding-list");
    
    inputText.value = window.arguments[0];

    // Obtain IEnconvIconv.
    enconvIconv = Components.classes["@enconv.sourceforge.net/enconv/iconv"]
      .getService(Components.interfaces.IEnconvIconv);

    // Obtain IEnconvCharDetector.
    enconvCharDet = Components.classes["@enconv.sourceforge.net/enconv/chardet"]
      .getService(Components.interfaces.IEnconvCharDetector);

    // Get supported encoding list.
    var encodingsGroups = enconvIconv.listEncodings().split("\n\n")
      .filter(function (x) { return x.length != 0; });
    encodings = [];
    for (var i = 0; i < encodingsGroups.length; i++)
    {
      encodings.push(encodingsGroups[i].split("\n").join("/"));
    }
    encodings.sort();
    for (var i = 0; i < encodings.length; i++)
    {
      var splitted = encodings[i].split("/");
      for (var j = 0; j < splitted.length; j++)
        singleEncNameToIdx[splitted[j]] = i;
    }

    var utf8Idx = encodings.indexOf("UTF-8");
    assert(utf8Idx >= 0);
    
    // Converter "From" encodings list.
    while (fromEncodingList.itemCount > 0)
      fromEncodingList.removeItemAt(0);
    for (var i = 0; i < encodings.length; i++)
    {
      fromEncodingList.appendItem(encodings[i], encodings[i]);
    }
    // NOTE: First scroll and then select! TODO: May be this is bug?
    // NOTE: Scroll not further that physically scroller can be! Hard to be
    // precise in that, so using "ensureIndexIsVisible()".
    //fromEncodingList.scrollToIndex(utf8Idx);
    fromEncodingList.ensureIndexIsVisible(utf8Idx);
    fromEncodingList.selectedIndex = utf8Idx;

    // Converter "To" encodings list.
    while (toEncodingList.itemCount > 0)
      toEncodingList.removeItemAt(0);
    for (var i = 0; i < encodings.length; i++)
    {
      toEncodingList.appendItem(encodings[i], encodings[i]);
    }
    toEncodingList.ensureIndexIsVisible(utf8Idx);
    toEncodingList.selectedIndex = utf8Idx;
    
    // Install event listeners.
    fromEncodingList.addEventListener("select",   updateConvertedText, false);
    toEncodingList  .addEventListener("select",   updateConvertedText, false);
    inputText       .addEventListener("input",  updateConvertedText, false);

    updateConvertedText();
  }
  catch (e)
  {
    // TODO
    alert("Encoding Converter Failed: " + e);
    window.close();
    throw e;
  }
}

window.addEventListener("load", function () { return onLoad(); }, false);
