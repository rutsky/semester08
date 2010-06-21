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

/*
if (!net) var net = {};
if (!net.sourceforge) net.sourceforge = {};
if (!net.sourceforge.enconv) net.sourceforge.enconv = {};

net.sourceforge.enconv = {
  onLoad:
    function()
    {
      var contextMenuItem = document.getElementById("enconv-context-menu");
      contextMenuItem.addEventListener("command", this.openConverter, false);
    },
  
  onStatusBarPanelClick:
    function()
    {
      alert(this.strbundle().getString("msg.alert_hello"));
    },
    
  openConverter:
    function()
    {
      //window.open("chrome://enconv/content/main-window.xul", null, "chrome,width=600,height=500");
      window.openDialog("chrome://enconv/content/main-window.xul", null, "chrome,modal=yes,width=600,height=500");
    },
  
  strbundle:
    function() 
      document.getElementById("enconv-strings")
};

window.addEventListener("load", function(){ net.sourceforge.enconv.onLoad(); }, false);
*/

var enconvIconv = null;
var encodings = [];

var inputText        = null;
var resultText       = null;
var fromEncodingList = null;
var toEncodingList   = null;
//var viewEncodingList = null;

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
  if (rv == nsIFilePicker.returnOK || rv == nsIFilePicker.returnReplace) {
    var file = fp.file;
    // Get the path as string. Note that you usually won't
    // need to work with the string paths.
    var path = fp.file.path;
    // work with returned nsILocalFile...
    alert(path);
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

    // Get supported encoding list.
    var encodingsGroups = enconvIconv.listEncodings().split("\n\n")
      .filter(function (x) { return x.length != 0; });
    encodings = [];
    for (var i = 0; i < encodingsGroups.length; i++)
    {
      encodings.push(encodingsGroups[i].split("\n").join("/"));
    }
    encodings.sort();

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
    alert("Encoding Converter failed: " + e);
    throw e;
  }
}

window.addEventListener("load", function () { return onLoad(); }, false);
