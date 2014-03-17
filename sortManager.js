var doing = false;

function onmouse( event, index ) {

  switch( event.type ) {

    case 'mousedown': 
//      if( ! htmls )
      doing = true;
      startOrder( index ); 
      break;

    case 'mouseup': 
      doing = false;
      if( htmls )
        finishOrder(); 
      break;

    case 'mousemove': 
      if( htmls ) {
//        if( event.button & 1 )
        if( doing )
          changeOrder( index ); 
      }
      break;
  }

}

//---------

var htmls = null
var sortedIds = null
var toMove = 0
var lastCell = -1;

//---------

function startOrder( cellToMove ) {

  if( ! htmls ) {
    htmls = new Array();
    sortedIds = new Array();

	for( var k=0; k<sortManagerIds.length; k++ ) {
      htmls[sortManagerIds[k]] = document.all['sortCell'+k].innerHTML;
      sortedIds[k] = sortManagerIds[k];
    }
  }
  
  toMove = cellToMove
}

//---------

function finishOrder() {
  for( var k=0; k<sortManagerIds.length; k++ ) {
    sortManagerIds[k] = sortedIds[k];
    
    document.all['sortCell'+k].style.border = 'none';
  }
  //alert( toMove );
}

//---------

function saveOrder() {
  var ids = sortManagerIds.join(',');
  var url = sortManagerScript + "?" 
                + "returl="+sortManagerRetUrl
                + "&what="+sortManagerWhat + "&ids="+ids;
  location.replace( url );
}

//---------

function changeOrder( curCell ) {

  if( curCell == lastCell ) return
  lastCell = curCell

  var N = sortManagerIds.length

  var a = -1;
  var b = -1;

  // set border
  for( var k=0; k<N; k++ ) {
    var c = document.all['sortCell'+k];
    if( k == curCell ) {
      c.style.border = '2px solid red'
    }
    else {
      c.style.border = 'none'
    }
  }

  sortedIds[curCell] = sortManagerIds[toMove]

  for( k=0; k<N-1; k++ ) {
    a++; if( a == curCell ) a++;
    b++; if( b == toMove ) b++;
	sortedIds[a] = sortManagerIds[b]
  }
 
  showOrder();
}

//---------

function showOrder() {
	for( var k=0; k<sortManagerIds.length; k++ ) {
	  document.all['sortCell'+k].innerHTML = htmls[sortedIds[k]];
    }
}

//---------
