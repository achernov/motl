<?php
require_once "userGallery.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

//=====================

$idAlbum = Motl::getIntParameter( "idAlbum" );

if( Motl::getParameter( "loadingImage" ) ) {
  acceptUpload();
}

if( Motl::getParameter( "afterUpload" ) ) {
  setUploadedImage();
}
    
if( $idAlbum ) {
  showAlbum( $idAlbum );
}
else {
  showUpload();
}

//=====================

function showUpload() {
  $user = User::getCurrentUser();
  $userGallery = new UserGallery( $user->row );
  
  echo "<link href='motl.css' type='text/css' rel='stylesheet'>";
  echo "<body class=mainText style='padding-left:20;padding-top:20;'>";
  echo $userGallery->makeUpload();
}

//=====================
// http://chernov-pc/motl/forumMedia.php?uid=1&afterUpload=1&idUploadAlbum=1231&idUploadImage=0

function setUploadedImage() {
  $uploadFailed = Motl::getIntParameter( "uploadFailed" );
  $idImage = Motl::getIntParameter( "idUploadImage" );
  
  if( $idImage && !$uploadFailed ) {
    $picture = MediaManager::getPicture($idImage,200,200);
    $url = $picture->getFileUrl();
    echo "<script>parent.showPicture($idImage,'$url')</script>\n";
  }
  else {
    echo "<script>alert('Не удалось загрузить.\\nВозможно, файл слишком большой')</script>\n";
  }
}

//=====================

function acceptUpload() {
  $user = User::getCurrentUser();
  $userGallery = new UserGallery( $user->row );

  $idUser = $user->id;
  //print_r($_REQUEST);die();
  $userGallery->acceptImage( "forumMedia.php?uid=$idUser&afterUpload=1", "forumMedia.php?afterUpload=1&uploadFailed=1" );
}

//=====================

function showAlbum( $idAlbum ) {

  $gallery = new GalleryItemProducer();
  
  $items = $gallery->loadMultipleFromDB( "pid='$idAlbum'", "position,id" );

  echo "<body style='margin:0;padding:0;'>";
  
  echo "<table border=0 cellspacing=0 cellpadding=0><tr>\n";
  for( $k=0; $k<count($items); $k++ ) {
    $item = $items[$k];
    $idPic = $item->row->idPicture;
    $picture = MediaManager::getPicture($idPic,200,200);
    $picHtml = $picture->getHtml();
    $url = $picture->getFileUrl();
    echo "<td onclick=\"parent.showPicture($idPic,'$url')\"><img height='100' style='padding-right:10' src='$url'></td>\n";
  }
  echo "</tr></table>\n";
}

//=====================
?>
