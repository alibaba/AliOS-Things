var cookie_namespace = 'doxygen'; 
var sidenav,navtree,content,header;

function readCookie(cookie) 
{
  var myCookie = cookie_namespace+"_"+cookie+"=";
  if (document.cookie) 
  {
    var index = document.cookie.indexOf(myCookie);
    if (index != -1) 
    {
      var valStart = index + myCookie.length;
      var valEnd = document.cookie.indexOf(";", valStart);
      if (valEnd == -1) 
      {
        valEnd = document.cookie.length;
      }
      var val = document.cookie.substring(valStart, valEnd);
      return val;
    }
  }
  return 0;
}

function writeCookie(cookie, val, expiration) 
{
  if (val==undefined) return;
  if (expiration == null) 
  {
    var date = new Date();
    date.setTime(date.getTime()+(10*365*24*60*60*1000)); // default expiration is one week
    expiration = date.toGMTString();
  }
  document.cookie = cookie_namespace + "_" + cookie + "=" + val + "; expires=" + expiration+"; path=/";
}
 
function resizeWidth() 
{

}

function restoreWidth(navWidth)
{

}

function resizeHeight() 
{
}

function initResizable()
{
}


