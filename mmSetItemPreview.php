<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "mediaManager.inc";
//=====================

function hexToString( $hex ) {
  $s = "";

  for( $k=0; $k<strlen($hex); $k+=2 ) {
    $h = $hex[$k];
    $l = $hex[$k+1];
    $v = (int)hexdec("$h$l");
    $s .= chr($v);
  }

  return $s;
}

//=====================

$idItem = Motl::getIntParameter( "dbf_id" );

$imageData = hexToString( $_REQUEST['pixels'] );

$im = imagecreatefromstring( $imageData );

$folderPath = MediaManager::getBasePath() . "/_preview";

$padded = substr( "00000000$idItem", -6 );
$fileName = "preview_$padded.jpg";

$w = imagesx( $im );
$h = imagesy( $im );

imagejpeg( $im, "$folderPath/$fileName" );

MotlDB::connect();

MotlDB::query( "UPDATE mm_items "
			."SET previewFile='".MRES($fileName)."', width=$w, height=$h "
			."WHERE id='$idItem'" );  // ASS $idItem

//=====================

$script  = Motl::getParameter( "script" );
$folders = Motl::getParameter( "folders" );
$addToGallery  = Motl::getParameter( "addToGallery" );
//die('qqq');

$autoTime = Motl::getParameter( "auto" );
if( $autoTime ) {
	/*echo "script = $script<p>";
	echo "folders = $folders<p>";
	echo "url = $script?folders=$folders&addToGallery=$addToGallery<p>";*/
	
	$url = "$script?makePreview=auto&script=mmExplorer.php"
			."&folders=$folders&addToGallery=$addToGallery&autoTime=$autoTime";
			
	Motl::redirect( $url );
}

Motl::redirect( "$script?folders=$folders&addToGallery=$addToGallery" );
//die( "$script?folders=$folders&addToGallery=$addToGallery" );

//=====================
?>
