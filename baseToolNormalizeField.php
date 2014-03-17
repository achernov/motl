<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseColumn.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='debug.js'></script>\n";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================

define( "NORM_DONE_OK", 1 );
define( "NORM_DONE_NOTHING", 2 );
define( "NORM_DONE_AMBIGUOUS", 3 );
define( "NORM_NOT_DONE", 4 );

//=====================
//echo "<pre>"; print_r( $_REQUEST ); die();
//$old1 = $_REQUEST["old_1"];
//die( "'$old1'" );
//=====================

if( isset($_REQUEST['saveForReplace']) ) {
  saveForReplace( Motl::getParameter("fieldName"), 
				Motl::getParameter("textIn"), Motl::getParameter("textOut") );
}

if( isset($_REQUEST['normalizeFieldWrite']) ) {
  normalizeFieldWrite( Motl::getParameter( "fieldName" ) );
}
else
if( isset($_REQUEST['normalizeFieldValue']) ) {
  normalizeFieldValue( Motl::getParameter("fieldName"), Motl::getParameter("fieldValue") );
}
else {
  normalizeFieldPrimary( Motl::getParameter("fieldName") );
}


//=====================

function distinct( $array ) {
  $out = array();
  for( $k=0; $k<count($array); $k++ ) {
    if( ! in_array( $array[$k], $out ) ) {
      $out[] = $array[$k];
    }
  }
  return $out;
}

//=====================

function normalizeFieldPrimary( $fname ) {

	ASSERT_name( $fname );

	$column = BaseColumn::createColumn( $fname );
	$fieldName = $column->getName();
	$rawFieldName = $column->getRawFieldName();

	echo "<A href='base.php?id=tools'>назад</a><br>";
	echo "<p>\n\n";

	//--- Load data

	$sql = "SELECT GROUP_CONCAT(id) AS idsCE, $rawFieldName as names, $fieldName as ids FROM db_couple_event "
		."WHERE $rawFieldName<>'' "
	//."and id=145650 "
		."GROUP BY $rawFieldName, $fieldName "
		."ORDER BY $rawFieldName";
//die($sql);
	$rows = MotlDB::queryRows( $sql );
//print_r($rows);die();
echo "<pre>";	

	$arrDoneOk    = array();
	$arrDoneAmbig = array();
	$arrNotDone = array();

	foreach( $rows as $row ) {
		$names = $row->names;
		$ids = $row->ids;
		$url = "baseToolLinkHierarchy.php?show=list&ids=$row->idsCE";
		$linkNames = "<A href='$url'>$names</a>";
		
		if( $names != "-" ) {
			$res = autoSetField( $column, $names, $ids );
			switch( $res ) {
				case NORM_DONE_OK:        $arrDoneOk[] = $names; break;
				case NORM_DONE_AMBIGUOUS: $arrDoneAmbig[] = $linkNames; break;
				case NORM_NOT_DONE:       $arrNotDone[] = $row; break;
			}
		}
		
	}
	
	if( $arrDoneAmbig ) {
		echo "<b>Обработаны автоматически, <font color=red>возможна неоднозначность</font>:</b><p>";
		echo implode( "<br>", $arrDoneAmbig )."<p>";
	}
	
	if( $arrDoneOk ) {
		echo "<b>Обработаны автоматически, <font color=green>OK:</font></b><p>";
		echo implode( "<br>", $arrDoneOk )."<p>";
	}
  
  //---------------------------------------------------------------
  //---------------------------------------------------------------
  //  MANUAL PROCESSING
	if( count($arrNotDone) ) {
		echo "<b>Требуют ручной обработки:</b><p>";
	}
	else {
		if( !count($arrDoneOk) && !count($arrDoneAmbig) ) {
			echo "<b><font color=lightgreen>Обработка не требуется</font></b><p>";
		}
	}

  //--- Make Form
  
  $script = $_SERVER['PHP_SELF'];
  echo "<form action='$script' method='POST'>\n";
  echo "<input type=hidden name='normalizeFieldWrite' value='1'>\n";
  echo "<input type=hidden name='fieldName' value='$fieldName'>\n";
  echo "<table>\n";
  $nValues = count($arrNotDone);
  echo "<input name='nValues' type=hidden value='$nValues'>\n";
  for( $k=0; $k<$nValues; $k++ ) {
	//print_r($arrNotDone[$k]);
	echo makePrimaryNormCell( $fieldName, $k, $arrNotDone[$k]->names );
  }
  echo "</table>\n";
  echo "<input type='submit' value='Сохранить'>\n";
  echo "<form>\n";
  
}
  
//=====================

function autoSetField( $column, $snames, $sids ) {
//	$snames = "Авангард, Алмаз ";

	$fieldName = $column->getName();
	$rawFieldName = $column->getRawFieldName();
		
//	echo "<b>==$snames========</b><p>";

	$names = splitValue( $snames );
	$newSNames = implode( ", ", $names );
	$ids = strlen($sids) ? explode( ",", $sids ) : array();
	
	//-----  Build new ids from names
	
	$newIds = array(); // In case we build it from scratch
	$newIdsUsable = true; // Until spoiled
	$newIdsAmbiguous = false;
	
	foreach( $names as $name ) {
		$items = $column->getItemsBy( "name", $name );
		
		if( count($items) >= 1 ) { // Can use, but admbiguity possible
			$newIds[] = $items[0]->id;
		}
		else {
			$newIdsUsable = false;
		}
		
		if( count($items) > 1 ) { // Is ambiguous
			$newIdsAmbiguous = true;
		}
	}
	
	//-----  Build names from current ids
	
	$logNames = array(); // 'Logical' names
	foreach( $ids as $id ) {
		$item = $column->getItemBy( "id", $id );
		$logNames[] = $item->name;
	}
	
	//-----  autoset ids, if possible

	$autoOk = false;
	
	//print_r($names);print_r($ids);print_r($logNames);
	
	if( array_equal( $names, $logNames ) ) { // If current ids match the text

		if( $newSNames != $snames ) { // just adjust separators etc.
			$set = "$rawFieldName='".MRES($newSNames)."'";
			$sql = "UPDATE db_couple_event SET $set WHERE $rawFieldName='".MRES($snames)."'";
			echo "$sql<p>";
			MotlDB::query( $sql );
			return NORM_DONE_OK;
		}
		else {
			return NORM_DONE_NOTHING;
		}
		
		$autoOk = true; 
	}
	else {
		if( $newIdsUsable && ($sids=='') ) {
//		if( $newIdsUsable ) {
			$newSids = implode( ",", $newIds );
			
			$set = "$fieldName='".MRES($newSids)."'";
			if( $newSNames != $snames ) {
				$set .= ", $rawFieldName='".MRES($newSNames)."'";
			}
			
			$sql = "UPDATE db_couple_event SET $set WHERE $rawFieldName='".MRES($snames)."'";
			echo "$sql<p>";
			MotlDB::query( $sql );
			
			return $newIdsAmbiguous ? NORM_DONE_AMBIGUOUS : NORM_DONE_OK;
		}
		else {
		//print_r($names);print_r($logNames);
			//die("$snames, $sids");
			return NORM_NOT_DONE;
		}
	}
	
	//-----  
}

//=====================

function array_equal( $ar1, $ar2 ) {
	foreach( $ar1 as $k=>$v ) {
		if( (!isset($ar2[$k])) || ($ar1[$k]!=$ar2[$k]) ) return false;
	}
	foreach( $ar2 as $k=>$v ) {
		if( (!isset($ar1[$k])) || ($ar1[$k]!=$ar2[$k]) ) return false;
	}
	return true;
}

//=====================

function splitValue( $value ) {

	$value = trim( $value );
	
	$value = str_replace( ",",   "\n", $value );
	//$value = str_replace( " - ", "\n",  $value );
	
	$parts = explode( "\n", $value );
	
	foreach( $parts as $key=>$part ) {
		$parts[$key] = trim($part);
	}
	
	return $parts;
}

//=====================

function makePrimaryNormCell( $fieldName, $k, $value ) {
	$script = $_SERVER['PHP_SELF'];
    $escapedValue = urlencode($value);
	$htmlEscapedValue = Motl::htmlEscape( $value );
    $jsEscapedValue = escapeJsString($value);
    $valLink = "$script?normalizeFieldValue=1&fieldName=$fieldName&fieldValue=$escapedValue";

	$column = new BaseColumnReplace( $fieldName );
//$value = 'Беловы Александр и Марина';
	$items = $column->getSimilarItems( $value );
//print_r($items);//die();
//$score = $items[0]->score;
	$selector = "<select name='sel$k' id='sel$k' style='width:350px'>";
	for( $i=0; $i<count($items); $i++ ) {
		$id  = $items[$i]->id;
		$val = $items[$i]->title;
		$selector .= "<option value=$id>$val";
	}
	$selector .= "</select>";

    
	$s = "";
    $s .= "<input name='old_$k' type=hidden value='[$htmlEscapedValue]'>";
    $s .= "<tr>";
    $s .= "<td>=&gt;<A id='txt_$k' style='color:red' href='$valLink'>$value</a>&lt;=</td> ";
    $s .= "<td><nobr>";
    $s .= "<input type=button value='=&gt;' onclick=\"textIntoField('txt_$k','new_$k')\"> ";
    $s .= "<input size=50 id='new_$k' name='new_$k' type=text>";
    $s .= "<input type=button value='&lt;-Use' onclick=\"useReplace('sel$k','new_$k')\"> ";
    $s .= "<input type=button value='Save-&gt;' onclick=\"saveForReplace('$fieldName','$jsEscapedValue',new_$k.value)\"> ";
//	$s .= "$score $selector";
	$s .= "$selector";
    $s .= "</nobr></td>";
    $s .= "</tr>\n";
	return $s;
}

//=====================

function goodParts( $value, $separator, $goodValues ) {

  $parts = explode( $separator, $value );
  
  if( count($parts) <= 1 ) {
    $good = false;
  }
  else {
    $good = true;
    foreach( $parts as $part ) {
      if( ! in_array( $part, $goodValues ) ) {
        $good = false;
      }
    }
  }
  return $good;
}

//=====================

function normalizeFieldValue( $fieldName, $fieldValue ) {

  $script = $_SERVER['PHP_SELF'];
  echo "<A href='$script?normalizeField=1&fieldName=$fieldName'>назад</a><p>";
  
  $a = explodeFieldValue( $fieldValue );
  
  echo "<table><tr>";
  $nCells = count($a);
  for( $k=0; $k<$nCells; $k++ ) {
    $cell = makeNormCell( $k, $fieldName, $a[$k] );
    echo "<td style='padding-right:30'>$cell</td>";
  }
  $fieldValueEscaped = escapeJsString($fieldValue);
  echo "<td><input type='button' value='Submit' onclick=normCellSubmit(\"[$fieldValueEscaped]\",$nCells)></td>";
  echo "</tr></table>";

  echo "<form name='F' action='$script' method='POST'>\n";
  echo "<input name='fieldName' type=hidden value='$fieldName'>\n";
  echo "<input name='nValues' type=hidden value='1'>\n";
  echo "<input name='old_0' type=hidden >";
  echo "<input name='new_0' type=hidden >";
  echo "<input type='hidden' name='normalizeFieldWrite' value='Сохранить'>\n";
  echo "</form>\n";
  
  makeValueRegs( $fieldName, $fieldValue );

}

//=====================

function makeValueRegs( $fieldName, $fieldValue ) {
  ASSERT_name( $fieldName );

  $sql = "SELECT ce.id, ce.idEvent AS idComp, e.pid AS idEvent, idCouple, idSolo "
        ."FROM db_couple_event AS ce INNER JOIN db_events AS e ON ce.idEvent=e.id "
        ."WHERE binary raw_$fieldName='".MRES($fieldValue)."'";
        
  $rows = MotlDB::queryRows( $sql );
  
  $ids = array();
  foreach( $rows as $row ) {
    $hc = $row->idCouple ? $row->idCouple : $row->idSolo;
	$ids[] = $row->id;
    echo "<A href='db.php?id=e$row->idEvent,c$row->idComp&hc=$hc'>$row->id</a><br>";
    //print_r($row);
  }
  
  $sids = implode( ",", $ids );
  
  echo "<p>SELECT * FROM db_couple_event where id in($sids)";
}

//=====================

function explodeFieldValue( $s ) {
  $s = str_replace( " - ", ",", $s );
  $s = str_replace( "/", ",", $s );
  $s = str_replace( ", ", ",", $s );
  return explode( ",", $s );
}

//=====================

function makeNormCell( $index, $fieldName, $fieldValue ) {
  //--- Load
  
  $column = BaseColumn::createColumn( $fieldName );
  $items = $column->getSimilarItems( $fieldValue );

  $selector = "<select name='sel$index' id='sel$index'>";
  for( $k=0; $k<count($items); $k++ ) {
    $id  = $items[$k]->id;
    $val = $items[$k]->title;
    $selector .= "<option value=$id>$val";
  }
  $selector .= "</select>";
  
  $returl = Motl::makeRetUrl();
  
  $idCode = $column->getIdCode();
  $producerClass = $column->getProducerClass();
  
  //print_r( $column);
  $createUrl = escapeJsString( "editObject.php?returl=$returl&what=$producerClass&fromString=$fieldValue" );
  $create = "<input type='button' value='Create' onclick='location.href=\"$createUrl\"'>";

  $editUrlBase = escapeJsString( "editObject.php?returl=$returl&what=$producerClass&dbf_id=" );
  $edit = "<input type='button' value='Edit' onclick=normCellGo(\"$editUrlBase\",$index)>";

  $viewUrlBase = escapeJsString( "db.php?id=$idCode" );
  $view = "<input type='button' value='View' onclick=normCellGo(\"$viewUrlBase\",$index)>";

  return "<font size=2 color=red>$fieldValue</font>$create<br>"."$selector<br>$edit&nbsp;$view";
}

//=====================

function normalizeFieldWrite( $fname ) {
	ASSERT_name( $fname );
	
	$column = BaseColumn::createColumn( $fname );
	$fieldName = $column->getName();
	$rawFieldName = $column->getRawFieldName();
	
  //print_r($_REQUEST);die();

  $nValues = Motl::getParameter( "nValues", 0 );
  //die("$nValues");
  for( $k=0; $k<$nValues; $k++ ) {
    $oldValue = Motl::getParameter( "old_$k" );
    $oldValue = substr( $oldValue, 1, -1 );
    $newValue = Motl::getParameter( "new_$k" );
    if( strlen(trim($newValue)) ) {
      
      $sql = "UPDATE db_couple_event SET $rawFieldName='".MRES($newValue)."' WHERE $rawFieldName='".MRES($oldValue)."'";
	  echo "$sql<p>";
      MotlDB::query( $sql );
    }
  }
  //die('q');
  normalizeFieldPrimary( $fname );
}

//=====================

function saveForReplace( $fieldName, $textIn, $textOut ) {
	$textIn  = unescape( $textIn );
	$textOut = unescape( $textOut );
//echo "saveForReplace( $fieldName, $textIn, $textOut )<p>";

	$sql = "INSERT IGNORE INTO db_replace(fieldName,textIn,textOut) "
			."VALUES( '".MRES($fieldName)."', '".MRES($textIn)."', '".MRES($textOut)."' )";
			
	echo "$sql<p>";
	MotlDB::query( $sql );
	
	//die($sql);
}
				
//=====================
?>
