//-----------------------------------------------------------------------------------

function openPopupDiv( div ) {

  div.style.display = 'block';
  
  div.style.left = document.body.scrollLeft + (document.body.clientWidth-div.clientWidth)/2;
  div.style.top  = document.body.scrollTop  + (document.body.clientHeight-div.clientHeight)/2;
}

//-----------------------------------------------------------------------------------

function closePopupDiv( div ) {
  div.style.display = 'none';
}

//-----------------------------------------------------------------------------------

function openUserGalleryUpload() {
  //alert(1)
  openPopupDiv( uploadPopupDiv );
}

//-----------------------------------------------------------------------------------

function checkUpload( form ) {

  var ctlFile = form.elements.userfile
  if( ctlFile.value == "" ) {
    alert( "Сначала нужно выбрать файл для загрузки" )
    return false;
  }

  var ctlName = form.elements.albumName
  if( ctlName.style.display != 'none' ) {
    if( ctlName.value == "" ) {
      alert( "Для нового альбома нужно указать название" )
      return false;
    }
  }
  
  return true;
}

//-----------------------------------------------------------------------------------
