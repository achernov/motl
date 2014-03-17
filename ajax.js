//---------------------

function newXMLHttpRequest() {
  try { return new XMLHttpRequest(); }
    catch(e) {}
  try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); }
    catch(e) {}
  try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); }
    catch(e) {}
  try { return new ActiveXObject("Msxml2.XMLHTTP"); }
    catch(e) {}
  try { return new ActiveXObject("Microsoft.XMLHTTP"); }
    catch(e) {}
  return null;
}

//---------------------

var ajaxBuffer = Array(10)

//---------------------

function ajaxCreate( callback, data ) {
  for( var k=0; k<ajaxBuffer.length; k++ ) {
    if( ajaxBuffer[k] == null ) {
      ajaxBuffer[k] = Array(3);
      ajaxBuffer[k][0] = newXMLHttpRequest();
      ajaxBuffer[k][1] = callback;
      ajaxBuffer[k][2] = data;
      return ajaxBuffer[k][0];
    }
  }
  return null
}

//---------------------

function ajaxOnReadyStateChange() {
  for( var k=0; k<ajaxBuffer.length; k++ ) {
    if( ajaxBuffer[k] ) {
      var req = ajaxBuffer[k][0];
      var callback = ajaxBuffer[k][1];
      var data = ajaxBuffer[k][2];

      try {
        if( req.readyState == 4 ) {
          ajaxBuffer[k] = null;
          if( req.status == 200 ) {
            callback( req.responseText, req.responseXML, data );
          }
        }
      } catch(e) {}
    }
  }
}

//---------------------

function ajaxGET( callback, url, data ) {
  req = ajaxCreate( callback, data );
  req.open( "GET", url, true );
  req.onreadystatechange = ajaxOnReadyStateChange;
  req.send(null);
}

//---------------------
