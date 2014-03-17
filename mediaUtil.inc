<?php
//=====================

class MediaUtil {

  //================

  public function resizePicture( $source, $target, $neww, $newh ) {

    $size = getimagesize( $source );

    $sim = null;
    switch( $size[2] ) {
      case IMAGETYPE_GIF:  $sim=imagecreatefromgif($source);  break;
      case IMAGETYPE_JPEG: $sim=imagecreatefromjpeg($source); break;
      case IMAGETYPE_PNG:  $sim=imagecreatefrompng($source);  break;
    }
    if( ! $sim ) return false;

    $im = imagecreatetruecolor( $neww, $newh );

    imagecopyresampled( $im , $sim, 0,0, 0,0, $neww , $newh , $size[0],$size[1] );

    $ok = 0;
    switch( $size[2] ) {
      case IMAGETYPE_GIF:  $ok=imagegif($im,$target);  break;
      case IMAGETYPE_JPEG: $ok=imagejpeg($im,$target); break;
      case IMAGETYPE_PNG:  $ok=imagepng($im,$target);  break;
    }

    imagedestroy($im);

    return $ok;
  }

  //================
}

//=====================
?>