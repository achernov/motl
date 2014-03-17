<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "forumTemplates.inc";
require_once "forumUtil.inc";
require_once "dataUtil.inc";
require_once "galleryContent.inc";
//=====================

class UserGallery extends User {

  //================
  
  public static function userPicturesEnabled() {
    if( User::getCurrentUID() == 3 ) return 1;
    return User::isAdmin();
  }
  
  //================

  public function produce( $format="full" ) {

    switch( Motl::getParameter( "action" ) ) {
    
      case "upload":
        return $this->produceUploader();
        
      case "initialUpload":
        return $this->produceInitialUploader();
        
      case "acceptImage":
        return $this->acceptImage();
        
      case "manage":
        return $this->produceManager();
        break;
    }
    /*
    if( Motl::getParameter( "loadingImage" ) ) {
      $idUser = $this->id;
      return $this->acceptImage( "userGallery.php?uid=$idUser", "userGallery.php?uid=$idUser&upload=1" );
    }*/

    return "";
  }
  
  //================
  // SELECT * FROM gallery where id>1175 order by id
  
  public function produceSummary() { // Для показа в профиле
    $s = "";

    //--- count
    $nAlbums = 0;
    $nPhotos = 0;

    $idGal = (int)$this->getGallery();
    if( $idGal ) {
      $nAlbums = MotlDB::queryValue( "SELECT count(*) FROM gallery WHERE pid=".$idGal ); // COD $idGal
      
      $nPhotos = MotlDB::queryValue( "SELECT count(*) FROM gallery AS g1 INNER JOIN gallery AS g2 ON g1.pid=g2.id "
                                    ."WHERE g2.pid=".$idGal ); // COD $idGal
    }
    //---

    $id = $this->row->id;
    $title = "Фотоальбомы пользователя";
    if( $nAlbums || $nPhotos || $this->isUserEditable() ) {
      $href = $nAlbums ? "gallery.php?id=$idGal" : "userGallery.php?action=initialUpload&uid=$id";
      $title = "<A href='$href'>$title</a>";
    }
    $s .= "<div style='margin-top:15'>$title</div>";
    
    $s .= "<table>";
    $wAlbums = DataUtil::numForm( $nAlbums, "альбом", "альбома", "альбомов" );
    $s .= "<tr><td> $nAlbums </td><td> $wAlbums </td></tr>";
    $wPhotos = DataUtil::numForm( $nPhotos, "фотография", "фотографии", "фотографий" );
    $s .= "<tr><td> $nPhotos </td><td> $wPhotos </td></tr>";
    $s .= "</table>";
    
    return $s;
  }
  
  //================
  
  private function produceUploader() {
    //$s .= "<h1>Фотоальбомы пользователя <A class='Nick' href='profile.php?id=$id'>$nick</a></h1>";
    $s = "";
    $idAlbum = Motl::getParameter( "idAlbum" );
    $s .= $this->makeUpload( $idAlbum );
    return $s;
  }
  
  //================
  
  private function produceInitialUploader() {
  
    $s = "";
    
    $id = $this->row->id;
    $nick = $this->row->login;
    
    $s .= "<h1>Фотоальбомы пользователя <A class='Nick' href='profile.php?id=$id'>$nick</a></h1>";

    $idGal = $this->getGallery();
    if( $idGal ) {
      Motl::redirect( "gallery.php?id=$idGal" );
    }
    else {
      if( $this->isUserEditable() ) {
        $s .= "Еще ничего не загружено. Не начать ли загружать фотографии ? <p>";
        $s .= $this->makeUploadBordered();
      }
      else {
        $s .= "Еще ничего не загружено.<br>";
      }
    }
    
    return $s;
    
  }
  
  //================
  
  private function produceManager() { // UNSAFE ???
  
    $s = "";
    
    $id = $this->row->id;
    $nick = $this->row->login;
    
    $s .= "<h1>Фотоальбомы пользователя <A class='Nick' href='profile.php?id=$id'>$nick</a></h1>";
    
    $idAlbum = Motl::getIntParameter( "idAlbum" );
    
    $album = new GalleryItemProducer();
    $album->loadFromDB( $idAlbum );

    switch( $album->row->special ) {
    
      case 'UserGallery':
        print_r( $this );
        break;
        
      case 'UserAlbum':
        print_r( $album );
        break;
    }
    
    return $s;
}
  
  //================
  
  public function makeUploadBordered( $idSelect=null ) {
    return "<table border=1 cellspacing=0><tr><td style='font-size:12; padding-left:20;padding-right:20;padding-top:20'>\n"
           . $this->makeUpload($idSelect) .
           "</td></tr></table>\n";
  }
  
  //================

  public function makeUpload( $idSelect=null, $moreButtons=null ) {  // SAFE
    $script = $_SERVER['PHP_SELF'];
    $uid = Motl::getParameter( "uid" );

    //-------
    
    $albumSelector = $this->makeAlbumSelector( true, $idSelect, 
                                               "id='selAlbum' name='idAlbum' onchange='onSelAlbum()' style='width:400'" );
    
    //-------
    
    $s = "";

    $s .= "<script>"
         ."function onSelAlbum() {"
         ."var sel = document.all.selAlbum;"
         ."var ia = sel.options[sel.selectedIndex];"
         ."var disp = ia.value*1 ? 'none' : 'block';"
         ."document.all.spnAlbumName.style.display =  disp;"
         ."document.all.fldAlbumName.style.display =  disp;"
         ."}"
         ."</script>";

    
    $s .= "<form enctype='multipart/form-data' action='$script' method='POST' onsubmit='return checkUpload(this)'>\n";
    $s .= "    <input type='hidden' name='uid'     value='$uid' />\n";
    $s .= "    <input type='hidden' name='action' value='acceptImage' />\n";
    
    $s .= "    <input type='hidden' name='MAX_FILE_SIZE' value='60000000' />\n";
    
    $s .= "    <table style='font-size:12'>\n";
    $s .= "    <tr><td> Выберите файл:</td><td> <input type='file' style='width:400' name='userfile' /> </td><tr>\n";
    $s .= "    <tr><td> Куда загрузить:</td><td> $albumSelector </td><tr>\n";
    $s .= "    <tr><td> <span id='spnAlbumName'>Название альбома:</span></td><td> <input type='text' style='width:400' id='fldAlbumName' name='albumName' /> </td><tr>\n";
    $s .= "    <tr><td colspan=2> <nobr><input type='submit' value='Загрузить!' /> $moreButtons</nobr></td></tr>\n";
    $s .= "    </table>\n";

    $s .= "<script>onSelAlbum();</script>";
    
    $s .= "</form>\n";

    return $s;
  }
  
  //================
  
  public function countAlbums() {
    $idGal = $this->getGallery();
    if( $idGal ) {
      return MotlDB::queryRows( "SELECT count(*) FROM gallery WHERE pid=$idGal" );  // COD $idGal
    }
    return 0;
  }
  
  //================
  
  public function makeAlbumSelector( $includeNew, $idSelect, $selectorProps ) {
    $idGal = $this->getGallery();
    
    $options = "";
    
    if( $idGal ) {
      $rows = MotlDB::queryRows( "SELECT id, title FROM gallery WHERE pid=$idGal" );  // COD $idGal
      
      $kSel = -1;
      for( $k=0; $k<count($rows); $k++ ) {
        if( $rows[$k]->id == $idSelect ) {
          $kSel = $k;
        }
      }
      if( $kSel < 0 ) {
        if( count($rows) ) {
          $kSel = 0;
        }
      }
      
      for( $k=0; $k<count($rows); $k++ ) {
        $aid    = $rows[$k]->id;
        $atitle = $rows[$k]->title;
        
        $select = ( $k == $kSel ) ? "selected" : "";
        
        $options .= "<option value='$aid' $select>$atitle";
      }
    }
    $s = "<select $selectorProps>";
    if( $includeNew ) $s .= "<option value='0'>&lt;&lt;новый альбом&gt;&gt;";
    $s .= "$options</select>";
    return $s;
  }
  
  //================
  
  public function acceptImage( $urlGood=null, $urlBad=null ) {
    $idUser = $this->id;
    ASSERT_int($idUser);

    //----- Provide Album
    $idAlbum = Motl::getParameter( "idAlbum" );
    ASSERT_int($idAlbum);
    
    $albumName = Motl::getParameter( "albumName" );
    
    if( ! $idAlbum ) {
      $idGal = $this->getGallery( true );
      if( ! $idGal ) die("ERROR");
      $idAlbum = $this->createAlbum( $idGal, $albumName );
    }
    //print_r( $_REQUEST );die();
    
    //----- Store File
    $idFolder = $folder = "User_$idUser"; // ASS $idUser
    
    MediaManager::declareFolder( $idFolder, $folder );  // SAFE
    //print_r($_FILES);die();
    $idImage = (int)MediaManager::acceptFile( $idFolder, $_FILES['userfile'], 1024,1024 );  // SAFE
    if( ! $idImage ) {
    //print_r( $_REQUEST );die();
      if( $urlBad ) {
        Motl::redirect( $urlBad );
      }
      else {
        //$action = Motl::getParameter( "action" );
        //Motl::redirect( "userGallery.php?action=initialUpload&uid=$idUser&error=1" );
        Motl::redirect( "gallery.php?id=$idAlbum&uploadError=1" );
      }
    }

    //----- Create Gallery Item
    //$mmi = MediaManager::getItem( $idImage, 0,0 );
    //print_r($mmi);
    
    $item = new GalleryItemProducer();
    $item->row = new stdClass();
    $item->row->pid = $idAlbum;
    $item->row->idPicture = $idImage;
    $item->row->idOwner = $idUser;
    $item->row->title = $_FILES['userfile']['name'];
//    print_r( $item );
    $item->saveToDB();

    //----- Update Album icon
    // ASS $idAlbum
    // COD $idImage
    MotlDB::query( "UPDATE gallery SET idIcon=$idImage WHERE id=$idAlbum" );   // ASS $idAlbum,  COD $idImage
    
    if( $urlGood ) {
      Motl::redirect( $urlGood."&idUploadAlbum=$idAlbum&idUploadImage=$idImage" );
    }
    else {
      Motl::redirect( "gallery.php?id=$idAlbum" );
    }
  }
  
  //================
  
  private function createAlbum( $pid, $albumName ) {
    $idUser = (int)$this->id;
    
    $album = new GalleryItemProducer();
    $album->row = new stdClass();
    $album->row->pid = $pid;
    $album->row->title = $albumName;
    $album->row->special = "UserAlbum";
    $album->row->idOwner = $idUser;
    $album->saveToDB();
    return $album->id;
  }
  
  //================
  
  private function getGallery( $create = false ) {  // returns Safe int
  
    $idUser = (int)$this->id;

    $row = MotlDB::queryRow( "SELECT * FROM gallery WHERE special='UserGallery' AND idOwner=$idUser" ); // COD $idUser
    
    if( (!$row) && $create ) {
      $idAllUsers = MotlDB::queryValue( "SELECT id FROM gallery WHERE special='UserAlbums'", null );
//echo "idAllUsers = $idAllUsers<p>";

      $userGal = new GalleryItemProducer();
      $userGal->row = new stdClass();
      $userGal->row->pid = $idAllUsers;
      $userGal->row->special = "UserGallery";
      $userGal->row->idOwner = $idUser;
      $userGal->row->title = $this->row->login;
      $userGal->saveToDB();
      print_r($userGal);
      $row = $userGal->row;
    }

    return $row ? (int)$row->id : null;
  }
  
  //================
}

//=====================
?>