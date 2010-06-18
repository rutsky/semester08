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

function onLoad()
{
  document.getElementById("input-text").value = window.arguments[0];
}

window.addEventListener("load", onLoad, false);
