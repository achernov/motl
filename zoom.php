<?php
require_once "userGallery.inc";
//=====================

MotlDB::connect();

//=====================

$idImage = Motl::getIntParameter( "idImage" );

$pWidth   = Motl::getIntParameter( "width" );
if( ! $pWidth ) $pWidth = 800;

$pHeight  = Motl::getIntParameter( "height" );
if( ! $pHeight ) $pHeight = 600;

$item = MediaManager::getItem($idImage);
$width  = min($pWidth,$item->width) ;
$height = min($pHeight,$item->height);

$picture = MediaManager::getPicture( $idImage, $width, $height );
//$picHtml = $picture->getHtml();
$url = $picture->getFileUrl();

$finalW = $picture->width;
$finalH = $picture->height;


//echo "<body style='margin:0'>";
//echo $picHtml;
echo "<script>parent.setZoomFrame($finalW,$finalH,'$url')</script>";
echo "</body>";

//=====================
?>
