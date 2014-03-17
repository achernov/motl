//-----------------------------------------------------------------------------------

function confirmDeleteMessage() {
  return confirm( 'Удалить сообщение?\n(вместо него останется пометка об удаленном сообщении)' );
}

//-----------------------------------------------------------------------------------

function openPictureInserter( uid, ctlText ) {
  btnInsertPic.disabled = true;
  document.all.divPreview.innerHTML = "";
  
  popupDiv.style.display = 'block';
  
  popupDiv.style.left = document.body.scrollLeft + (document.body.clientWidth-popupDiv.clientWidth)/2;
  popupDiv.style.top  = document.body.scrollTop  + (document.body.clientHeight-popupDiv.clientHeight)/2;
}

//-----------------------------------------------------------------------------------

var idImageToZoom = null

function zoomPicture( idImage ) {
  idImageToZoom = idImage
  setTimeout( 'doZoomPicture()', 100 );
}

//--------

function doZoomPicture() {
  divZoom1.innerHTML = "<iframe style='display:none;margin:0;padding:0' width=100% height=100% src='zoom.php?idImage="+idImageToZoom+"'></iframe>";
}

//--------

function setZoomFrame( w, h, url ) {

  var W = document.body.clientWidth;
  var H = document.body.clientHeight;
  
  
  divZoom.style.left = document.body.scrollLeft + (W-w)/2;
  divZoom.style.top  = document.body.scrollTop  + (H-h)/2;

  divZoom.style.display = 'block';
  divZoom1.innerHTML = "<A title='Нажмите, чтобы закрыть' href='' onclick='closeZoom();return false;'><img border=0 src='"+url+"' width='"+w+"' height='"+h+"'></a>";
}

//--------

function closeZoom() {
  divZoom.style.display = 'none';
}

//-----------------------------------------------------------------------------------

function onSelAlbum() {
var sel = document.all.selAlbum;
var ia = sel.options[sel.selectedIndex];
var idAlbum = ia.value*1;
document.all.frmAlbum.src = "forumMedia.php?idAlbum="+idAlbum
}

function onInsert() {
  
  var ta = document.all.taTheText;
  
  var ins = "[gallery|"+idPicture+"]";
  
  if( ta.selectionStart == undefined ) {
    ta.value = ta.value + ins;
  }
  else {
    ta.value = ta.value.substring( 0, ta.selectionStart ) + ins + ta.value.substring( ta.selectionEnd );
  }

  
  document.all.popupDiv.style.display = 'none'
}

function onCancel() {
  //alert( document.all.taTheText.selectionStart + ", " + document.all.taTheText.selectionEnd )
//prop(document.all.taTheText);
  document.all.popupDiv.style.display = 'none'
}

var idPicture = 0;

function showPicture( idPic, url ) {
  idPicture = idPic;
  document.all.divPreview.innerHTML = "<img src="+url+">"
  btnInsertPic.disabled = false;

  //alert( "showPicture: "+p )
}

//-----------------------------------------------------------------------------------

