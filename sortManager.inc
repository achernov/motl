<?php
//=====================

class SortManager {

  //======= common
  
  public static function setOrder( $prototypeObject, $ids ) {
  
    ASSERT_ints($ids);
  
    $table  = $prototypeObject->getTableName();
    $fields = $prototypeObject->getFieldNames();
    
    $havePid = is_int(array_search("pid",$fields));
    if( $havePid ) {
      $pid = MotlDB::queryValue( "SELECT pid FROM $table WHERE id In($ids)", null );  // COD $table, ASS $ids
      $sql = "SELECT id FROM $table WHERE pid='$pid' ORDER BY position,pid"; // COD $table, ASS $ids
    }
    else {
      $sql = "SELECT id FROM $table ORDER BY position,pid"; // COD $table
    }
    
    $ids = trim($ids);
    $aids = strlen($ids) ? explode(",",$ids) : array();
    $fids = array();
    foreach( $aids as $a ) $fids[$a]=1;
    
    $oldIds = MotlDB::queryValues( $sql );  

    for( $k=0; $k<count($oldIds); $k++ ) {
      if( ! isset( $fids[$oldIds[$k]] ) )
        $aids[] = $oldIds[$k];
    }

    for( $k=0; $k<count($aids); $k++ ) {
      ASSERT( $aids[$k] );
      
      // ASS $aids[$k]
      // COD $table, $k
      MotlDB::query( "UPDATE $table SET position=$k WHERE id=$aids[$k]" );  
    }
    
    //print_r( $aids );
  }
  
  //======= For sorting form
  
  public static function makeSortLink( $sortId ) {
    $returl = Motl::makeRetUrl();
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id", null );
    $url = "$script?returl=$returl&id=$id&resort=$sortId";
	  return "<A class='editLink' href='$url'>Sort children</a>";
  }

  public static function checkSorting( $sortId ) {
	$resort = Motl::getParameter( "resort", null );
	if( is_null($resort) ) return 0;
	return $resort==$sortId;
  }
  
  private static $what = null;
  private static $ids = null;
  
  public static function beginSortForm() {
    SortManager::$ids = array();

    $saveButton   = "<input type=button value='Save order' onclick='saveOrder()'>";

    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id", null );
    $url = "$script?id=$id";
    $cancelButton = "<input type=button value='Cancel' onclick='location.replace(\"$url\")'>";

    return "<div>$saveButton $cancelButton</div>";
  }
  
  public static function processChild( $child, $sChild ) {
    $idChild = $child->id;
	  SortManager::$what = get_class( $child );
	
    $index = count( SortManager::$ids );
    SortManager::$ids[$index] = $idChild;
    $sChild = "<div id='sortCell$index' ondrag='return false'"
	              ." onmousedown='onmouse(event,$index);return false'"
	              ." onmouseup='onmouse(event,$index)'"
	              ." onmousemove='onmouse(event,$index)'"
	              ." onclick='return false'>"
	         .$sChild
			 ."</div>";
    return $sChild;
  }
  
  public static function endSortForm() {

    $returl = Motl::getParameter( "returl" );
    $returlUrlencoded = urlencode($returl);

    $sids = "'" . implode( "','", SortManager::$ids ) . "'";
	  $what = SortManager::$what;
    
	  return "\n<script>\n"
	      ."var sortManagerRetUrl='$returlUrlencoded';\n"
	      ."var sortManagerScript='reorder.php';\n"
	      ."var sortManagerWhat='$what';\n"
	      ."var sortManagerIds = Array($sids);\n"
	      ."</script>\n"
	      ."<script src='sortManager.js'></script>\n";
  }

  //=======
  
}

//=====================
?>