<?php
require_once "video.inc";
//=====================

class BbTemplator extends Templator {

  public $linkClass;

  //===========

  function processTag_video( $tagParts ) {
    $file = $tagParts[1];
    return makePlayer( $file );
  }

  //===========

  function processTag_image( $tagParts ) {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );

    $fileName = isset($tagParts[1]) ? $tagParts[1] : "";
    $fileRef  = Motl::getPicturesFolder()."/".$fileName;
    $brokenRef = Motl::getSysPicturesFolder()."/brokenImage.png";
    $filePath = Motl::getFilesystemPicturesFolder()."/".$fileName;

    $more = isset($tagParts[2]) ? " ".$tagParts[2] : "";

    $editmode = Motl::getParameter( "editmode", 0 );

    if( file_exists($filePath) ) {
      return "<img src='$fileRef'$more>";
    }
    else {
      if( ! $editmode )
        return "<img src='$brokenRef' title='Изображения нет на сервере'>";
      else
        return "<A href='uploadImage.php?image=$fileName&script=$script&id=$id'>"
            ."<img src='$brokenRef' border='0' title='Изображения нет на сервере.\nЩелкните здесь мышкой, чтобы загрузить'></a>";
    }
  }

  //===========

  function processTag_url( $tagParts ) {

    $url = $tagParts[1];

    $class = "";
/*
    if( $this->linkClass )
      $class = "class='".$this->linkClass."'";
*/
    if( strchr($url,'@') )
      return "<A $class href='mailto://$url'>$url</a>";
    return "<A $class href='http://$url'>$url</a>";
  }

  //===========

  function processTag_mail( $tagParts ) {

    $who = $tagParts[1];

    $class = "";

//    return "<A $class href=''>Для связи по e-mail</a>";
    return "<A $class href='feedback.php?recipient=$who'>Для связи по e-mail</a>";
  }
 
  //===========
}

//=====================

?>