//-----------------------------------------------------------------------------------

function setCookie( cookieName, cookieValue ) {
  document.cookie = cookieName+"="+cookieValue
  location.replace(location.href)
}

//-----------------------------------------------------------------------------------

function confirmKill() { 
  return confirm('Вы уверены, что хотите удалить этот объект?')
}

//-----------------------------------------------------------------------------------

function confirmKills() { 
  return confirm('Вы уверены, что хотите удалить эти объекты?')
}

//-----------------------------------------------------------------------------------
function getX( obj ) {
  var v=0;
  for( var p=obj; p!=null; p=p.offsetParent ) {
	v += p.offsetLeft//-p.scrollLeft
  }
  return v
}

function getY( obj ) {
  var v=0;
  for( var p=obj; p!=null; p=p.offsetParent ) {
//  prop(p)
//	alert( p+", "+p.offsetTop+", "+p.scrollTop )
	v += p.offsetTop//-p.scrollTop
  }
//  alert(v)
  return v
}
//=====================

