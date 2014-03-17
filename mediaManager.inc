<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "mediaUtil.inc";
require_once "fileUtil.inc";
//=====================

class MediaManager {

  //===============

  public function getBasePath() {
    return  Motl::getFilesystemScriptsFolder() . "/gallery";
  }

  //---------------

  public function getBaseUrl() {
    return  Motl::getScriptsFolder() . "/gallery";
  }

  //===============

  public function declareFolder( $idFolder, $folder ) {
    ASSERT_subpath( $idFolder );

    $baseFolder = MediaManager::getBasePath();

    //-----

    $oldFolder = MotlDB::queryValue( "SELECT folder FROM mm_folders WHERE idFolder='".MRES($idFolder)."'", null ); 

    if( $oldFolder ) {
      if( ! file_exists( "$baseFolder/$oldFolder" ) )
        mkdir( "$baseFolder/$oldFolder" );
      return $oldFolder;
    }

    //-----

    $newFolder = MediaManager::makeUniqueFileName( $baseFolder, $folder );

    mkdir( "$baseFolder/$newFolder" );

    MotlDB::query( "INSERT INTO mm_folders(idFolder,folder) VALUES('".MRES($idFolder)."','".MRES($newFolder)."')" );  

    return $newFolder;
  }

  //===============

  public function updateFolder( $folder, $files ) {

    MediaManager::registerFolder( $folder );

    for( $k=0; $k<count($files); $k++ ) {

      MediaManager::registerItem( $folder, $files[$k] );

    }

  }

  //===============

  public function acceptFile( $idFolder, $fileInput, $maxW=null, $maxH=null ) {
    //echo "$idFolder<br>";
    //print_r( $fileInput );
    
    $source = $fileInput['tmp_name'];
    
    $baseFolder = MediaManager::getBasePath();
    $folder   = MotlDB::queryValue( "SELECT folder FROM mm_folders WHERE idFolder='".MRES($idFolder)."'", null );
    $newfile = MediaManager::makeUniqueFileName( "$baseFolder/$folder", $fileInput['name'] );
    $target  = "$baseFolder/$folder/$newfile";

    $size = getimagesize( $source );
    $w = (int)$size[0];
    $h = (int)$size[1];
    $scale = 1;
    
    if( $maxW ) {
      $scale = min( $scale, $maxW/$w );
    }
    if( $maxH ) {
      $scale = min( $scale, $maxH/$h );
    }

    $scale = $scale*1.00001;
    
    if( $scale < 1 ) {
    //die("YES");
      $w1 = (int)( $w*$scale );
      $h1 = (int)( $h*$scale );
      
      MediaUtil::resizePicture( $source, $target, $w1, $h1 );
    }
    else {
    //die("no");
      move_uploaded_file( $source, $target );
    }
          
    return MediaManager::registerItem( $folder, $newfile );
  }

  //===============

  public function getItem( $idItem ) {
    $idItem = (int)$idItem;
    $row = MotlDB::queryRow( "SELECT * FROM mm_items WHERE id='$idItem'" );  // COD $idItem

    return new MediaManagerItem( $row );
  }

  //===============
  
  public function makeSimplePicture( $src, $w=null, $h=null ) {
    return new MediaManagerFormalItem( $src, $w, $h );
  }
  
  //===============

  public function getPicture( $idItem, $w, $h ) {
    $item = MediaManager::getItem( $idItem );
    if( ! $item ) return null;

    return $item->getPicture( $w, $h );
  }  

  //===============

  public function getFolderItemIds( $idFolder ) {
  }

  //===============

  public function updateFromDisk() {

    $galdir = MediaManager::getBasePath();

    $folders = FileUtil::getDirSubdirs( $galdir );

    foreach( $folders as $fld ) {
      if( $fld!='_tn' && $fld!='_preview' ) {
        $path = "$galdir/$fld";
        $files = FileUtil::getDirFiles( $path );
        MediaManager::updateFolder( $fld, $files );
      }
    }

  }

  //===============

  public function registerFolder( $folder ) {
//".MRES()."
    $idFolder = MotlDB::queryValue( "SELECT idFolder FROM mm_folders WHERE folder='".MRES($folder)."'", null );

    if( $idFolder ) {
      return $idFolder;
    }
    else {
      return MotlDB::saveRow( "mm_folders", "idFolder",$folder, array('folder'=>$folder) );
    }
  }

  //===============

  public function registerItem( $folder, $file ) {

    $baseFolder = MediaManager::getBasePath();

    $path = "$baseFolder/$folder/$file";

    $type = MediaManager::getMediaType( $path );

    if( $type ) {
      $idFolder = MotlDB::queryValue( "SELECT idFolder FROM mm_folders WHERE folder='".MRES($folder)."'", null );

      if( $idFolder ) {

        $fields = array( "idFolder"=>$idFolder, "file"=>$file, "type"=>$type );

        if( $type == 'image' ) {
          $size = getimagesize( $path );
          $fields['width'] = $size[0];
          $fields['height'] = $size[1];
        }

        $idItem = MotlDB::queryValue( "SELECT id FROM mm_items WHERE idFolder='".MRES($idFolder)."' and file='".MRES($file)."'", null );  

        return MotlDB::saveRow( "mm_items", "id",$idItem, $fields );
      }
    }

    return null;
  }

  //---------------

  private function getMediaType( $path ) {

    $f = fopen( $path, "r" );
    $b = fread( $f, 3 );
    fclose($f);

    switch( $b ) {
      case 'FLV':
        return 'flv';

      case "\xff\xd8\xff":
      case "GIF":
      case "‰PN":
        return 'image';
    }

    return null;
  }

  //---------------

  public function makeUniqueFileName( $baseFolder, $name ) {
    $ext = strrchr( $name, "." );
    $nameOnly = substr( $name, 0, strlen($name)-strlen($ext) );
    $ext = strtolower( $ext );

    $name = $nameOnly . $ext;

    while( file_exists( "$baseFolder/$name" ) ) {
      $name = $nameOnly .".". MediaManager::randLetters() . $ext;
    }

    return $name;
  }

  //---------------

  private function randLetters( $len = 4 ) {
    $randtext = md5( "" . mt_rand() );
    return substr( $randtext, 0, $len );
  }

  //===============
}

//=====================

class MediaManagerItem {

  function __construct( $row = null ) {
    if( $row ) {
      if( isset($row->idTn) ) {
        $this->id   = $row->idItem;
        $this->idTn = $row->idTn;
        $this->idFolder = null;
        $this->folder   = '_tn';
        $this->type   = 'image';
      }
      else {
        $this->id = $row->id;
        $this->idFolder = $row->idFolder;
        
        $this->folder   = MotlDB::queryValue( "SELECT folder FROM mm_folders WHERE idFolder='".MRES($this->idFolder)."'", null );  
        
        $this->type   = $row->type;
        $this->previewFile = $row->previewFile;
      }
      $this->file   = $row->file;
      $this->width  = $row->width;
      $this->height = $row->height;
    }
  }

  //===============

  public function isPicture() {
  }

  //===============
  
  public function isVideo() {
  }

  //===============
  
  public function getFileUrl() {
    $baseUrl = MediaManager::getBaseUrl();
    return "$baseUrl/$this->folder/$this->file";
  }
  
  //===============
  
  public function getHtml( $w=null, $h=null ) {
  
	$width  = $this->width;
	$height = $this->height;
	
	if( $w || $h ) {
		$sx = $w/$width;
		$sy = $h/$height;
		$scale = min( $sx, $sy );
		$width  = (int)( $scale * $width );
		$height = (int)( $scale * $height );
	}

    switch( $this->type ) { 
      case 'image':
        $url = $this->getFileUrl();
        return "<img border=0 width='$width' height='$height' src='$url'>";

      case 'flv':
//        Q($this);
        return makePlayer( "$this->folder/$this->file", 
							"_preview/$this->previewFile",
							$this->width,
							$this->height
						);
    }

    return "";
  }

  //===============

  public function getPreview() {

    $newItem = new MediaManagerItem();

    $previewFile = $this->previewFile ? $this->previewFile : "_video.png";

    $newItem->id       = $this->id;
    $newItem->idFolder = null;
    $newItem->folder   = '_preview';
    $newItem->type     = 'image';
    $newItem->file     = $previewFile;

    if( $this->width && $this->height ) {
      $newItem->width  = $this->width;
      $newItem->height = $this->height;
    }
    else {
      $newItem->width  = 200;
      $newItem->height = 150;
    }

    return $newItem;
  }
  
  //===============

  public function getSized( $maxW, $maxH ) {
    //$this->id = (int)$this->id;
    ASSERT_int( $this->id );

    $scale = 1;
    if( $maxW ) $scale = min( $scale, $maxW/$this->width );
    if( $maxH ) $scale = min( $scale, $maxH/$this->height );

    $w = (int)( $scale * $this->width );
    $h = (int)( $scale * $this->height );

    //-----------

    $epsilon = 0.05;

    $dw = max( 1, (int)($w*$epsilon) );
    $w0 = $w-$dw;
    $w1 = $w+$dw;
    $condW = "((width>=$w0) and (width<=$w1))";

    $dh = max( 1, (int)($h*$epsilon) );
    $h0 = $h-$dh;
    $h1 = $h+$dh;
    $condH = "((height>=$h0) and (height<=$h1))";

    //----------- Is Item suitable ?

    // ASS  $this->id, 
    // COD  $condW, $condH
    $row = MotlDB::queryRow( "SELECT * FROM mm_items WHERE id='$this->id' and $condW and $condH" );  

    if( $row ) {
      return new MediaManagerItem( $row );
    }

    //----- Find suitable TN

    // ASS  $this->id, 
    // COD  $condW, $condH
    $row = MotlDB::queryRow( "SELECT * FROM mm_tn WHERE idItem='$this->id' and $condW and $condH" );  
    if( $row ) {
      return new MediaManagerItem( $row );
    }

    //----- Make new TN

    $baseFolder = MediaManager::getBasePath();

    $folder = "$baseFolder/_tn";
    $newfile = MediaManager::makeUniqueFileName( $folder, $this->file );

    $source  = "$baseFolder/$this->folder/$this->file";
    $target = "$baseFolder/_tn/$newfile";

    MediaUtil::resizePicture( $source, $target, $w, $h );

    // ASS $this->id
    // COD $w, $h
    MotlDB::query( "INSERT INTO mm_tn(idItem,file,width,height) VALUES('$this->id','".MRES($newfile)."','$w','$h')" );  

    //----- Find suitable TN NOW

    // ASS $this->id
    // COD $condW, $condH
    $row = MotlDB::queryRow( "SELECT * FROM mm_tn WHERE idItem='$this->id' and $condW and $condH" );  
    if( $row ) {
      return new MediaManagerItem( $row );
    }

    //----- Something is wrong

    return null;
  }

  //===============
  
  public function getPicture( $w, $h ) {
    if( $this->type != 'image' ) {
      return $this->getPreview();
    }

    if( $w || $h ) {
      return $this->getSized( $w, $h );
    }
    
    return null;
  }
  
  //===============
  
  public function makeUploadForm() {
  }

  //===============
  
  public function acceptUpload() {
  }

  //===============
}

//=====================

class MediaManagerFormalItem {

  function __construct( $src, $width=null, $height=null ) {
    $this->src    = $src;
    $this->width  = $width;
    $this->height = $height;
  }
  
  //===============

  public function isPicture() {
    return true;
  }

  //===============
  
  public function isVideo() {
    return false;
  }

  //===============
  
  public function getFileUrl() {
    return $this->src;
  }
  
  //===============
  
  public function getHtml() {

    $url = $this->getFileUrl();
    $s = "<img border=0";
    if( $this->width  ) $s .= " width='$this->width'";
    if( $this->height ) $s .= " height='$this->height'";
    $s .= " src='$url'>";
    return $s;
  }

  //===============

  public function getPreview() {
    motlASSERT( false, "Function not implemented" );
  }
  
  //===============

  public function getSized( $maxW, $maxH ) {
    motlASSERT( false, "Function not implemented" );
  }

  //===============

  public function makeUploadForm() {
    motlASSERT( false, "Function not implemented" );
  }

  //===============
  
  public function acceptUpload() {
    motlASSERT( false, "Function not implemented" );
  }
  
}

//=====================

class MediaManagerUploader {

  private $ctrlName;
  
  //---------------

  public function __construct( $ctrlName) {
    $this->ctrlName = $ctrlName;
  }

  //---------------

  public function makeUpload() {
  }
  
  //---------------
}

//=====================
?>