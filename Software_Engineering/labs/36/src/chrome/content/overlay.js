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
      try
      {
        var enconvIconv = Components.classes["@enconv.sourceforge.net/enconv/iconv"]
          .getService(Components.interfaces.IEnconvIconv);
      }
      catch (e)
      {
        // TODO
        alert("Encoding Converter initialization failed: " + e);
      }

      alert(enconvIconv.listEncodings());

      var inputStr = "Test!";
      var inputEnc = "UTF-8";
      var resultStr = new String();
      var resultEnc = "UTF-8";

      try
      {
        resultStr = enconvIconv.iconv(resultEnc, inputEnc, inputStr);
        alert(resultStr);
      }
      catch (e)
      {
        alert("Encoding converter: iconv() failed.");
      }
      
      //var enconvIconv2 = Components.classes["@enconv.sourceforge.net/enconv/iconv"]
      //  .getService(Components.interfaces.IEnconvIconv);
      //dump(enconvIconv1);
    },
    
  openConverter:
    function()
    {
      window.openDialog("chrome://enconv/content/main-window.xul", null, 
        "chrome,modal=yes,width=600,height=500",
        content.getSelection().toString());
    },
  
  strbundle:
    function() 
      document.getElementById("enconv-strings")
};

window.addEventListener("load", function(){ net.sourceforge.enconv.onLoad(); }, false);
