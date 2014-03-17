//-----------------------------------------------------------------------------------

function prop( obj ) {

  var s = "";

  for( p in obj ) {
    s += p+"="+obj[p]+"<br>";
//    s += p+"=...<br>";
  }

showInWindow(s)

  //alert(s);
//  prompt("prop",s);
//ta.value=s
}

//-----------------------------------------------------------------------------------

function showInWindow(s)
{
  w = window.open( "about:blank", "debug", "top=100,left=100,width=500,height=300" )
  w.document.open();
  w.document.write(s)
  w.document.close();
}

//-----------------------------------------------------------------------------------

