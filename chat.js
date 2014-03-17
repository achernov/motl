//--------------------------------------------

var checkInterval = 500; // milliseconds

var lastKnownId = 0;
var lastWho = "";

//--------------------------------------------

function chatSay() {

  var msg = document.all['chatMessage'].value;
  document.all['chatMessage'].value = "";
  if( msg == "" ) return;
 //alert( "chatReceiver.php?message="+escape(msg) ) 
  //document.location.href = "chatReceiver.php?message="+escape(msg) 
  ajaxGET( null, "chatReceiver.php?message="+escape(msg) );
//prompt( 1, escape(msg) )
  //ajaxPOST( null, "chatReceiver.php", "message="+msg );
}

//-----------------------

function doCheck() {
  //document.all['checkFrame'].src = 'chatCheck.php'
  ajaxGET( onChatCheck, "chatCheck.php" );
}

//-----------------------

function onChatCheck( t ) {
//prompt( 1, "'"+t+"'")
  var at = t.split(':');
  
  var idMsg = at[0];
  if( idMsg > lastKnownId ) {
    getMessages( lastKnownId, idMsg );
  }
  lastKnownId = idMsg;
  
  var who = at[1];
  if( who != lastWho ) {
    getPeople( who );
  }
  lastWho = who;
}

//-----------------------

function onMessage( t ) {
  document.all['chatWindow'].innerHTML += t
  setTimeout( 'doScroll()', 100 );
}

function doScroll() {
  document.all['chatScroller'].scrollTop = 10000
}

//-----------------------

function getMessages( oldId, newId ) {
  ajaxGET( onMessage, 'chatGet.php?idFrom='+oldId+'&idTo='+newId );
}

//-----------------------

function getPeople( ids ) {
  ajaxGET( onPeople, 'chatWho.php?ids='+ids );
}

//-----------------------

function onPeople( t ) {
  document.all['chatWho'].innerHTML = t
}

//-----------------------

setInterval( 'doCheck()', checkInterval );

//-----------------------
