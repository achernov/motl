<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "mediaManager.inc";
require_once "_dbproducer.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

$folders = Motl::getParameter( "folders" );
$addToGallery = Motl::getIntParameter( "addToGallery" );

MediaManager::updateFromDisk();

//=====================

class MmeTemplator extends DbProducerTemplator {
  //-----------

  function processTag_mmitem( $tagParts ) {

    $idItem = $this->getFValue($tagParts);

    $more = $this->getMore($tagParts);
    
    $w = $h = null;
    if( $more ) {
      $size = explode( 'x', $more );
      if( count($size) == 2 ) {
        list($w,$h) = $size;
      }
    }
	
    $pic = MediaManager::getPicture( $idItem, $w, $h );

    return $pic ? $pic->getHtml($w,$h) : null;
  }

  //-----------

  function processTag_makepreview( $tagParts ) {

    $idItem = $this->getFValue($tagParts);

    $item   = MediaManager::getItem( $idItem );
  $folders = Motl::getParameter("folders");
  $addToGallery = Motl::getIntParameter("addToGallery");

    if( $item->type=='flv' ) {
//    if( $item->type=='flv' && !$item->previewFile ) {
      $script = $_SERVER['PHP_SELF'];
      $url = "$script?makePreview=$idItem&script=$script&folders=$folders&addToGallery=$addToGallery";
      return "<A class='EditLink' href='$url'>Make&nbsp;preview</a>";
    }
  }

  //-----------
};

//=====================

function makeMatrix( $dataRows, $nCols, $template ) {

  $dbp = new DbProducer();

  $nCells = count( $dataRows );

  if( $nCols > $nCells ) $nCols = $nCells;

  $s = "<table border=1>\n";

  for( $r=0; $r*$nCols<$nCells; $r++ ) {
    $s .= "<tr>";
    for( $c=0; $c<$nCols; $c++ ) {

      $k = $nCols*$r + $c;

      $s .= "<td>";
      $s .= ( $k<$nCells ) ? $dbp->makeText( $dataRows[$k], $template, new MmeTemplator(), $k."_" ) : "&nbsp;";
      $s .= "</td>";
    }
    $s .= "</tr>\n";

    if( $k >= $nCells ) break;
  }

  $s .= "</table>\n";
  return $s;
}

//=====================

function produceFolderList() {

  $rows = MotlDB::queryRows( "SELECT * FROM mm_folders" );

  $s = "<table cellspacing=0 border=1>";
  $s .= "<tr> <td colspan=2 bgcolor=#d0d0d0><b>Folders</b></td> </tr>";
  for( $k=0; $k<count($rows); $k++ ) {
    $s .= $dbp->makeText( $rows[$k], "<tr> <td><input type=checkbox></td> <td>[plain|idFolder]</td> </tr>" );
  }
  $s .= "</table>";
  return $s;
}

//=====================

function foldersToSql( $folders ) {
  $afolders = explode( ';', $folders );

  $bfolders = array();
  foreach( $afolders as $f ) {
    $bfolders[] = MRES($f);
  }
  return "'" . implode( "','", $bfolders ) . "'";
}

//=====================

function produceAddtoGallery( $folders, $addToGallery ) {

	ASSERT_int( $addToGallery );

	$lfolders = foldersToSql( $folders );

	$filter = Motl::getParameter("filter","unused");
	switch( $filter ) {

	case 'unused':
		$filterCond = "not exists (SELECT * FROM gallery WHERE idPicture=mm_items.id)";
		break;

	case 'nohere':
		$filterCond = "not exists (SELECT * FROM gallery WHERE idPicture=mm_items.id AND gallery.pid='$addToGallery')";
		break;

	case 'noprev':
		$filterCond = "( (type='flv') AND ((previewFile Is Null) OR (previewFile='')) )";
		break;

	default:
	case 'none':
		$filterCond = "true";
		break;
	}

	// COD $lfolders, $filterCond
	$sql = "SELECT * FROM mm_items "
			."WHERE idFolder In($lfolders) "
			."and $filterCond"
			." ORDER BY idFolder, file";
	//die($sql);
	$rows = MotlDB::queryRows( $sql );  
	$nItems = count($rows);

	$cellTemplate = "<div style='width:200;padding:10'>[hidden|id]<center>"
					."<A href='' onclick='toggleOneSel(\"dbf_[prefix]sel\");return false;'>[mmitem|id|200x150]</a><br>"
					."[check|sel][plain|file] [makepreview|id]"
					."&nbsp;</center></div>";

	$spacer = "&nbsp;&nbsp;&nbsp;&nbsp;";
				 
	$s = "";

	$s .= "<script src='db.js'></script>";

	$s .= makeFilterSwitch($filter);

	$s .= "<form action='mmAction.php' method='POST'>";

	$goBack = "<A href='gallery.php?id=$addToGallery'>назад</a>";
	$s .= "<p>$goBack$spacer<input type=submit value='addToGallery'><p>";
	
	$toggle = "<A href='' onclick='toggleSels(\"dbf_\",\"_sel\",0);return false;'>"
			.	"выбрать/очистить все</a>";
	
	$autoPreview = "AutoPreview at <input id='autoTime' style='width:30px'> sec. "
					."<input type='button' value='Go!' onclick='autoPreview(\"$folders\",$addToGallery)'>";
	
	$s .= "$toggle$spacer$spacer$spacer$autoPreview<p>";

	$s .= "<input type=hidden name=nItems value='$nItems'>";
	$s .= "<input type=hidden name=addToGallery value='$addToGallery'>";

	$s .= makeMatrix( $rows, 4, $cellTemplate );

	$s .= "</form>";
	
	$s = str_replace( "640", "200", $s );
	$s = str_replace( "360", "112", $s );
	return $s;
}

//---------------------

function makeFilterSwitch($filter) {

  $script = $_SERVER['PHP_SELF'];
  //$filter = Motl::getParameter("filter");
  $folders = Motl::getParameter("folders");
  $addToGallery = Motl::getParameter("addToGallery");

  $url = "$script?folders=$folders&addToGallery=$addToGallery";

  $s = "";
  $s .= "Show images: ";
  $s .= makeSwitchLink( ($filter!='unused' && $filter!='nohere' && $filter!='noprev'), "$url&filter=none", "Any" );
  $s .= "&nbsp;";
  $s .= makeSwitchLink( ($filter=='unused'), "$url&filter=unused", "Unused" );
  $s .= "&nbsp;";
  $s .= makeSwitchLink( ($filter=='nohere'), "$url&filter=nohere", "Not here" );
  $s .= "&nbsp;";
  $s .= makeSwitchLink( ($filter=='noprev'), "$url&filter=noprev", "Without preview" );

  return $s;
}

//---------------------

function makeSwitchLink( $selected, $url, $label ) {
  return $selected ? "<b>$label</b>" : "<A href='$url'>$label</a>";
}

//=====================

function produceMakePreview( $idItem ) {
	$script = $_SERVER['PHP_SELF'];
	$folders = Motl::getParameter( "folders" );
	$addToGallery  = Motl::getParameter( "addToGallery" );
	$autoTime  = Motl::getParameter( "autoTime" );

	$autoParam = "";
	if( $idItem == 'auto' ) {
		$idItem = getAutoItemId( $folders );
		if( ! $idItem ) {
			$url = "mmExplorer.php?folders=$folders&addToGallery=$addToGallery";
			Motl::redirect( $url );
			die( "No more items" );
		}
		$autoTime = $autoTime ? $autoTime : 10;
		$autoParam = "&auto=$autoTime";
	}

	$item = MediaManager::getItem( $idItem );

	$path = "$item->folder/$item->file";

	$onDone = "mmSetItemPreview.php?dbf_id=$idItem&script=$script"
				."&folders=$folders&addToGallery=$addToGallery".$autoParam;

	$s = "";
	$s .= "<A href='mmExplorer.php?folders=$folders&addToGallery=$addToGallery'>назад</a><p>";
	$s.= makeVideoTool( $path, $onDone, $autoTime );
	return $s;
}

//=====================

function getAutoItemId( $folders ) {

	$filterCond = "( (type='flv') AND ((previewFile Is Null) OR (previewFile='')) )";
	$lfolders = foldersToSql( $folders );

	// COD $lfolders, $filterCond
	$sql = "SELECT id FROM mm_items "
			."WHERE idFolder In($lfolders) "
			."and $filterCond "
			."LIMIT 1";
	$id = MotlDB::queryValue( $sql );
	return $id;
}

//=====================

//echo "<link href='motl.css' type='text/css' rel='stylesheet'>";

$makePreview = Motl::getParameter( "makePreview" );

if( $makePreview ) {
  echo produceMakePreview( $makePreview );
}
else
if( $folders && $addToGallery ) {
  echo produceAddtoGallery( $folders, $addToGallery );
}

//=====================
?>
<script>

function autoPreview( folders, idGallery ) {
	var t = document.all['autoTime'].value
	var url = "mmExplorer.php?makePreview=auto"
							+"&script=mmExplorer.php"
							+"&folders="+folders
							+"&addToGallery="+idGallery+""
							+"&autoTime="+t
	//alert(url)
	location.href = url;
}

</script>
