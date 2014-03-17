<?php
require_once "_dbproducer.inc";
require_once "mediaManager.inc";
require_once "userGallery.inc";
//=====================

class GalleryContent extends Producer {

  public function produce() {

    $id = Motl::getIntParameter( "id", 0 );
	
	if( ! $id ) {
		$id = MotlDB::queryValue( "SELECT id "
					."FROM gallery WHERE pid Is Null ORDER BY id LIMIT 1" );
		if( ! $id ) $id=0;
	}
	//die("==$id==");
    $action = Motl::getParameter( "action" );
//print_r($_REQUEST);die();
    if( $action == "acceptImage" ) {
      $this->acceptImage();
      die( "accept" );
      return;
    }
    
    $s = "";

    $producer = new GalleryItemProducer();
    $producer->loadFromDB( $id ? $id : "0" );

    $s .= $producer->produce( "full" );

    return $s;
  }
  
  //----------------
  
  function acceptImage() {
    $user = User::getCurrentUser();
    $userGallery = new UserGallery( $user->row );
    
    $idAlbum = Motl::getIntParameter( "idAlbum" );
    if( ! $idAlbum ) {
      $idAlbum = $userGallery->getGallery();
    }
    
    $userGallery->acceptImage( "gallery.php?id=$idAlbum&ok", "gallery.php?id=$idAlbum&no" );
  }
  
  //----------------
}

//=====================

class GalleryItemProducer extends DbProducer {

  public $children;

  //----------------
  public function getTableName() {
    return "gallery";
  }

  //----------------

  public function getFields() {
    $afolders = MotlDB::queryValues( "SELECT idFolder FROM mm_folders" );
    $folders = implode( ';', $afolders );
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"pid"	=>	"[text|pid]",
		"position"	=>	"[text|position]",
		"special"	=>	"[text|special]",
		"idOwner"	=>	"[text|idOwner]",
		"title"	=>	"[text|title|style='width:400']",
		"subtitle"	=>	"[textarea|subtitle|style='width:400; height:80']",
		"idIcon"	=>	"[text|idIcon]",
		"idPicture"	=>	"[text|idPicture]",
		"folders"	=>	"[select|folders|$folders|multiple style='width:300' size=10]"
	);
  }

  //----------------
  
  public function getFieldNames() { // Чтобы не запрашивать folders
    return array( "id", "ts_created", "pid", "position", "special", "idOwner", 
                  "folders", "idIcon", "idPicture", "title", "subtitle" );
  }
  
  //----------------

  public function loadFromQuery( $rownumber=null ) {
    parent::loadFromQuery( $rownumber );
  }

  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    switch( $format ) {

      case "full":
        return $this->produceFull();

      case "edit":
        return $this->produceEdit();

      default:
        return $this->produceShort();
    }
  }

  //----------------

  public function produceEdit() {

    $s  = "";

    $s .= "<h1>Галерея</h1>";

    $s .= $this->makePath();
    $s .= $this->makeEditForm();

    return $s;
  }

  //----------------

  public function produceShort() {

    $script = "gallery.php";//$_SERVER['PHP_SELF'];
    $editmode = Motl::getParameter( "editmode", 0 );

    $icon = null;

    $idIcon = $this->row->idIcon;
    $idPicture = $this->row->idPicture;
    $type = null;
	
    $fileLabel = "";

    if( $idPicture ) {
      $pictureItem = MediaManager::getItem( $idPicture, 0,0 );
      $type = $pictureItem->type;
      $fileLabel = $pictureItem->file;
    }
    else {
      $pictureItem = null;
      $type = null;
    }
	
    if( $this->row->special == 'UserGallery' ) {
      $iconItem = MediaManager::makeSimplePicture( "pictures/face1.png" );
    }
    else
    if( $idIcon ) {
      $iconItem = MediaManager::getPicture( $idIcon, 200, 150 );
    }
    else {
      if( $pictureItem ) {
        $iconItem = MediaManager::getPicture( $idPicture, 200, 150 );
      }
    }

	if( isset($iconItem) ) {
		if( $iconItem->width>200 || $iconItem->height>150) {
			$sx = 200/$iconItem->width;
			$sy = 150/$iconItem->height;
			$scale = min( $sx, $sy );
			$iconItem->width  = (int)( $scale * $iconItem->width );
			$iconItem->height = (int)( $scale * $iconItem->height );
		}
	}

	if( isset($iconItem) ) {
		$icon = $iconItem->getHtml();
		$iconH = $iconItem->height;
	}
	else {
		$icon = null;
		$iconH = 0;
	}

    //-------

	$sysfolder = Motl::getSysPicturesFolder();
	
    $url = "$script?id=".$this->id."";

    $title = $this->row->title;

    //-------

	$tmplIcon  = "<A href='$url'>$icon</a>";
	$tmplLabel = "<A href='$url'>[plain|title]</a>";
	
  if( $this->row->idPicture ) {
    if( $type == 'flv' ) { // Video cell
      $videoIconH = 20;
      $videoIconX = 6;
      $videoIconY = 5;
      $top    = $videoIconY-$iconH;
      $bottom = -$top-$videoIconH;
      $vid = "<div style='margin-left:$videoIconX; margin-top:$top; padding-bottom:$bottom;'><img title='This is video' border=0 src='$sysfolder/iconVideo.png'></div>";

      $tmplIcon = $tmplIcon.$vid;
	  }
	  else { // Picture cell
	  }
	}
	else {
    if( $this->haveChildren() ) { // Folder cell
      
      $folderA = "<table style='margin-left:5;margin-right:10' cellpadding=0 cellspacing=0>"
                ."<tr> <td style='padding:10; border:#5A5A5A 1px solid;'>";
  				  
      $folderB = "</td> <td background=$sysfolder/folderBorderR.png><img src=$sysfolder/folderBorderS.png></td>  </tr>"
                ."<tr> <td background=$sysfolder/folderBorderB.png><img src=$sysfolder/folderBorderS.png></td> <td><img src=$sysfolder/folderBorderBR.png></td>  </tr>"
                ."</table>";

  		$tmplIcon = "<center>$folderA$tmplIcon$folderB</center>";
      $tmplLabel = "<b>$tmplLabel</b>";
	  }
	  else { // mistake, try to treat as picture
	  }
	}
	
    //-------

	$template = $tmplIcon
			   ."[editbar|edit,del|&nbsp;<small>$fileLabel</small><br>]"
	           ."<div style='text-align:center;width:200;overflow:hidden;margin-left:0;margin-right:20;'><small>"
			   ."$tmplLabel<br>[plain|subtitle]"
			   ."</small></div>";

    //-------

    $s = "";

    $s .= $this->makeText( $this->row, $template );

    //-------
    if( Motl::getParameter( "userEdit", 0 ) ) {
      $s .= $this->makeChildToolbar();
    }
    //-------
            
    return $s;
  }

  //----------------

  public function produceFull() {

    $script = "gallery.php";//$_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id", 0 );
    $editmode = Motl::getParameter( "editmode", 0 );
	
//print_r($this);

    $s = "";

    $s .= "<h1>Галерея</h1>";

    $s .= $this->makePath();

    if( $editmode ) {
      $spacer = "&nbsp;&nbsp;&nbsp;&nbsp;";

      $addLink    = $this->makeAddLink( $id );

      $editLink    = $this->makeEditLink();

      $afolders = $this->collectFolders();
      $folders  = implode( ';', $afolders );

      $addItemsUrl = $folders ? "mmExplorer.php?folders=$folders&addToGallery=$this->id"
                              : "javascript:alert(\"No folders specified\")";
      $addItemsLink = "<A class='EditLink' href='$addItemsUrl'>Add Items</a>";

      $sortLink = $this->makeSortLink();

      $s .= "<nobr>$editLink $spacer $addLink $spacer $addItemsLink $spacer $sortLink</nobr><p>";
    }
    
    $s .= $this->makePrevNext();
    
    if( $this->row->idPicture ) {
      $s .= $this->producePicture();
    }
    else {
      
      $s .= $this->produceChildren();
      
      if( $this->row->special == 'UserGallery' || $this->row->special == 'UserAlbum' ) {
        $s .= $this->makeUserAlbumToolbar();
      }
    }

    return $s;
  }

  //----------------
  
  function makeUserAlbumToolbar() {
    
    $idUser  = User::getCurrentUID();
    $idAlbum = (int)$this->row->id;
    $idOwner = (int)$this->row->idOwner;
    
    $s = "";
    if( $idUser && ( $idUser==$idOwner || User::isAdmin() ) ) {
      $s .= "<script src='gallery.js'></script>";
      
      $s .= $this->makeUploadPopupDiv();
      $s .= $this->makeManagerPopupDiv();
      
      $s .= "<div id='userAlbumToolbar' style='width:400; padding:10; border:1px solid #c0c0c0'>";
    
      $special = $this->row->special;
      
//      $s .= "<A href='userGallery.php?action=upload&uid=$idOwner&idAlbum=$idAlbum'>"
      $s .= "<A href='' onclick='openPopupDiv(uploadPopupDiv);return false;'>"
            ."Добавить фотографии / создать новый альбом</a><br>";
            
      $word = ($special=="UserGallery") ? "альбомами" : "фотографиями";
      
//      $s .= "<A href='userGallery.php?action=manage&uid=$idOwner&idAlbum=$idAlbum'>"
//      $s .= "<A  href='' onclick='openPopupDiv(managerPopupDiv);return false;'>"

      $userEdit = Motl::getParameter( "userEdit", 0 );
      
      if( $userEdit ) {
        $s .= "<A  href='' onclick='setCookie(\"userEdit\",0);return false;'>"
            ."Скрыть средства редактирования </a><br>";
      }
      else {
        $s .= "<A  href='' onclick='setCookie(\"userEdit\",1);return false;'>"
            ."Другие действия с $word (редактирование)</a><br>";
      }
      
      $s .= "</div>";
    }
    return $s;
  }
  
  //----------------

  function makeChildToolbar() {
    if( ! $this->row->idOwner ) return;
    $idUser = User::getCurrentUID();
    if( $this->row->idOwner != $idUser ) return;
    if( $this->row->special == 'UserGalery' ) return;
    
    $s = "";
//    $s .= "<center><div style='width:200; padding:10; background-color:#d0d0f0; border:1px solid #808080'>";
    $s .= "<center><div style='width:200; margin-top:3;padding:3; padding-left:10; background-color:#d0d0f0;text-align:left;font-size:12'>";
    $s .= "<A href='' style='padding-right:10' onclick=\"confirm('Удалить альбом ?')\">удалить</a>";
    $s .= "<A href='' onclick=\"prompt('Введите новое название для ')\">переименовать</a>";
    $s .= "</div></center>";
    return $s;
  }
  
  //----------------
  
  function makeUploadPopupDiv() {
    $user = User::getCurrentUser();
    $userGallery = new UserGallery( $user->row );
    
    $idAlbum = (int)$this->row->id;
  
    $s = "<div id='uploadPopupDiv' style='display:none;position:absolute;padding:20;border:3px solid #0000c0; background-color:white; left:300; top:300; width:600; height:100;'>";
    $moreButtons = "<input type='button' value='Отменить' onclick='closePopupDiv(uploadPopupDiv)' />";
    $s .= $userGallery->makeUpload( $idAlbum, $moreButtons ); // SAFE
    $s .= "</div>";
    
    return $s;
  }
  
  //----------------
  
  function makeManagerPopupDiv() {
    $user = User::getCurrentUser();
    $userGallery = new UserGallery( $user->row );
    
    $idAlbum = (int)$this->row->id;
  
    $s = "<div id='managerPopupDiv' style='display:none;position:absolute;padding:20;border:3px solid #0000c0; background-color:white; left:300; top:300; width:600; height:100;'>";
    $s .= "<input type='button' value='Закрыть' onclick='closePopupDiv(managerPopupDiv)' />";
    //$s .= $userGallery->makeUpload( $idAlbum, $moreButtons ); // SAFE
    $s .= "</div>";
    
    return $s;
  }
  
  //----------------

  private function collectFolders() {

    $agids = array();

    ASSERT_int($this->id);
    
    // ASS $this->id
    // FLD $p (pid)
    for( $p=$this->id; $p; $p=MotlDB::queryValue("SELECT pid FROM gallery WHERE id='$p'",null) ) {  
      $agids[] = $p;
    }
    $gids = implode( ',', $agids );

    $afolders = MotlDB::queryValues( "SELECT folders FROM gallery WHERE id In($gids)" );  // ASS,FLD $gids
    $folders  = implode( ';', $afolders );
    $afolders = explode( ';', $folders );

    $outfolders = array();
    foreach( $afolders as $f )
      if( $f )
        $outfolders[] = $f;

    return $outfolders;
  }

  //----------------

  public function producePicture() {
    $s = "";

    $idPicture = $this->row->idPicture;

    if( ! $idPicture ) {
      return "NO PICTURE";
    }

//    $item = MediaManager::getPicture( $idPicture, 800, 500 );
//    $item = MediaManager::getPicture( $idPicture, 960, 720 );
    
    $item = MediaManager::getItem( $idPicture );

    if( ! $item ) {
      return "NO PICTURE";
    }

    if( $item->type != 'image' ) {
      $pictureHtml = $item->getHtml();
    }
    else {
      $urlFull = $item->getFileUrl();
      $sized = $item->getSized( 960, 720 );
      $sizedPicture = $sized->getHtml();
      $pictureHtml = "<A href='$urlFull'>$sizedPicture</a>";
    }

    $label = "<small><b>". $this->row->title ."</b></small>";

    $s = "";
//    $s .= $this->makePrevNext();
    $s .= "$pictureHtml<br>$label";

    return $s;
  }

  //----------------

  public function makePrevNext() {
    $id  = $this->row->id;
    $pid = $this->row->pid;
    ASSERT_int($pid);
    
    // ASS $pid
    $sql = "SELECT id FROM gallery WHERE pid=$pid ORDER BY position,id";
  
    $siblings = MotlDB::queryValues( $sql );  
  	
  	$pos = array_search( $id, $siblings );
  	
  	if( is_int($pos) ) {
  	  $prev = "&lt;назад";
  	  $next = "вперед&gt;";
  	  $prev = (isset($siblings[$pos-1])) ? $this->makePrevNextArrow($prev,$siblings[$pos-1]) : $this->makePrevNextArrow($prev,null);
  	  $next = (isset($siblings[$pos+1])) ? $this->makePrevNextArrow($next,$siblings[$pos+1]) : $this->makePrevNextArrow($next,null);
  	  return "$prev&nbsp;&nbsp;&nbsp;$next<br>";
	}
	
	return null;
  }

  //----------------

  private function makePrevNextArrow( $text, $id ) {
    $script = "gallery.php";//$_SERVER['PHP_SELF'];
    if( $id ) {
	  return "<A href='$script?id=$id'>$text</a>";
	}
	else {
	  return "<font color='gray'>$text</font>";
	}
  }

  //----------------
  
  public function produceItems( $ids ) {
  
    $sids = implode( ",", $ids );
    ASSERT_ints($sids);
    
    $rows = $this->loadMultipleFromDB( "id In($sids)", "pid,position,id" ); // ASS $sids
    
    return $this->produceTable( $rows );
  }
  
  //----------------
  
  public function produceChildren() {
    
    $rows = $this->loadChildren();
    
    return $this->produceTable( $rows );
  }
  
  //----------------

  public function produceTable( $rows ) {
    $nRows = count($rows);

    $s = "";

    $nCellsPerRow = 4;

    $nTableRows = ceil( $nRows / $nCellsPerRow );
	
	if( $this->checkSorting() ) {
	  $s .= SortManager::beginSortForm();
	}

    $s .= "<table border=0 idth=100%>\n";

    for( $r=0; $r<$nTableRows; $r++ ) {
      $s .= "<tr valign='top' '---'>";
        for( $c=0; $c<$nCellsPerRow; $c++ ) {
          $p = $c + $r*$nCellsPerRow;
          $child = ( $p<$nRows ) ? $rows[$p] : 0;

          $s .= "<td style='width:200;padding-bottom:20'>";
          if( $child ) {
            $sChild = $child->produce("short");
        
            if( $this->checkSorting() ) {
              $sChild = SortManager::processChild( $child, $sChild);
            }
            
            $s .= $sChild;
          }
          else {
            $s .= "&nbsp;";
          }
          $s .= "</td>";
        }
      $s .= "</tr>\n";
    }

    $s .= "</table>\n";

	if( $this->checkSorting() ) {
	  $s .= SortManager::endSortForm();
	}

    return $s;
  }

  //----------------

  public function haveChildren() {
    $pid = $this->row->id;
    ASSERT_int($pid);
    return MotlDB::queryValue( "SELECT 1 FROM gallery WHERE pid='$pid' LIMIT 1", "0" );  // ASS $pid
  }
  
  //----------------

  public function loadChildren() {

    $pid = $this->row->id;
    ASSERT_int($pid);

    if( User::isAdmin() ) {
      $specCond = "true";
    }
    else {
      $specCond = "(special is null or special<>'UserAlbums')";
    }

    // ASS $pid
    // COD $specCond
    $rows = $this->loadMultipleFromDB( "pid='$pid' And $specCond", "position,id" ); 

    return $rows;
  }

  //----------------

  public function makePath( ) {
    return parent::makePath( null, "gallery", "gallery.php", "title" );
  }

  //----------------

}

//=====================
?>