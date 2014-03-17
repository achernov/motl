<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseColumn.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

if( Motl::getParameter("action") == "listRegs" ) {
	makeRegList();
}
else
if( Motl::getParameter("action") == "replace" ) {
	doReplace();
}
else {
	makeReport();
}

//=====================

function doReplace() {
	$ids = Motl::getParameter("ids");
	ASSERT_ints($ids);

	$fl  = Motl::getParameter("fl");
	$flv = trim( Motl::getParameter("flv") );
	ASSERT_ints($flv);
	$colLow = BaseColumn::createColumn($fl);
	$fieldLow  = $colLow->getName();
	$fieldRawLow  = $colLow->getRawFieldName();
	$rawLow = $colLow->sidsToSvalues($flv,", ");
	
	$fh = Motl::getParameter("fh");
	$fhv = trim( Motl::getParameter("fhv") );
	ASSERT_ints($fhv);
	$colHigh = BaseColumn::createColumn($fh);
	$fieldHigh = $colHigh->getName();
	$fieldRawHigh = $colHigh->getRawFieldName();
	$rawHigh = $colHigh->sidsToSvalues($fhv,", ");
	
	// UNSAFE
	$sql = "UPDATE db_couple_event "
			."SET $fieldLow='".MRES($flv)."', $fieldHigh='".MRES($fhv)."', "
				."$fieldRawLow='".MRES($rawLow)."', $fieldRawHigh='".MRES($rawHigh)."' "
			."WHERE id In($ids)";
		
	//echo $sql;
	MotlDB::query( $sql );
	
	makeReport();
}

//=====================

function makeRegList() {
	$script = $_SERVER['PHP_SELF'];

	$sids0 = Motl::getParameter("ids");
	ASSERT_ints($sids0);

	$fl  = Motl::getParameter("fl");
	$flv = Motl::getParameter("flv");
	ASSERT_ints($flv);
	$colLow = BaseColumn::createColumn($fl);
	$fieldLow  = $colLow->getName();
	$condLow = strCond( $fieldLow, $flv );
	$titleLow = $colLow->getRusName();
	$labelsLow = $colLow->makeListImploded( explode(",",$flv), ", " );
	
	$fh = Motl::getParameter("fh");
	$fhv = Motl::getParameter("fhv");
	ASSERT_ints($fhv);
	$colHigh = BaseColumn::createColumn($fh);
	$fieldHigh = $colHigh->getName();
	$condHigh = strCond( $fieldHigh, $fhv );
	$titleHigh = $colHigh->getRusName();
	$labelsHigh = $colHigh->makeListImploded( explode(",",$fhv), ", " );
	
	//-----
	
	// COD $condLow, $condHigh
	$sql = "SELECT ce.id, e.id as idComp, e.pid AS idEvent, ce.idCouple, ce.idSolo "
			."FROM db_couple_event AS ce INNER JOIN db_events AS e ON ce.idEvent=e.id";
			
	if( strlen($sids0) ) {
		$sql .= " WHERE id In($sids0)"; 
	}
	else {
		$sql .= " WHERE $condLow AND $condHigh"; 
	}
	//die($sql);
	$rows = MotlDB::queryRows( $sql );
	
	//-----
	
	$items = array();
	$ids = array();
	foreach( $rows as $row ) {
		$hc = max( $row->idCouple, $row->idSolo );
		$items[] = "<A href='db.php?id=e$row->idEvent,c$row->idComp&hc=$hc'>$row->id</a>";
		$ids[] = $row->id;
	}
	$sitems = implode( ", ", $items );
	$sids = implode( ",", $ids );
	
	//-----
	
	echo "<big>Для комбинации:</big>\n";
	echo "<table cellspacing=0 border=1>\n";
	echo "<tr> <td><b>$titleHigh</b></td> <td>$fhv&nbsp;</td> <td>$labelsHigh&nbsp;</td> </tr>\n";
	echo "<tr> <td><b>$titleLow</b></td>  <td>$flv&nbsp;</td> <td>$labelsLow&nbsp;</td> </tr>\n";
	echo "</table>\n";
	echo "<p>\n";
	echo "<big>есть регистрации:</big> $sitems\n";
	//echo "<p style='margin:0;padding:0;'>\n";
	
	$mode = Motl::getParameter("show","diff");
	
	$chooserHigh = makeTwinChooser( "fhv", $colHigh, $fhv );
	$chooserLow  = makeTwinChooser( "flv", $colLow, $flv );

	echo "<div style='margin-top:50;'><big>Заменить для всех:</big></div>";
	echo "<form action=$script style='margin-top:3;'>\n";
	echo "<input type=hidden name='action' value='replace'>\n";
	echo "<input type=hidden name='show' value='$mode'>\n";
	echo "<input type=hidden name='ids' value='$sids'>\n";
	echo "<input type=hidden name='fl' value='$fieldLow'>\n";
	echo "<input type=hidden name='fh' value='$fieldHigh'>\n";
	echo "<table cellpadding=5>\n";
	echo "<tr valign=top> <td><b>$titleHigh</b></td> <td> <input type=text id=fhv name=fhv value='$fhv'></td> <td>$chooserHigh</tr>\n";
	echo "<tr valign=top> <td><b>$titleLow</b></td>  <td> <input type=text id=flv name=flv value='$flv'></td> <td>$chooserLow</td> </tr>\n";
	echo "</table>\n";
	echo "<input type=submit value='replace'>\n";
	echo "</form>\n";
	
//	echo "<tr>$sitems";
	
	//die( $sql );
}

//=====================

function makeTwinChooser( $ctrlName, $column, $fvalue ) {

	$s = "";

	$columnName = $column->getName();
	$varName = "arr$columnName";

	$s .= "\n<script>\n";
	$s .= "var $varName = [$fvalue]\n";
	$s .= "</script>\n";
	
	$ids = explode(",",$fvalue);
	$s .= "<table><tr valign=top>";
	foreach( $ids as $k=>$id ) {
		$s .= "<td>".makeTwinChooserCell( $ctrlName, $varName, $k, $column, $id )."</td>";
	}
	$s .= "</tr></table>";
	return $s;
}

//=====================

function makeTwinChooserCell( $ctrlName, $varName, $cellIndex, $column, $id ) {

	$columnName = $column->getName();
	
	if( $columnName == "teacher" ) { // The only special case
		$table = "db_person";
		$titleExpr = "CONCAT_WS(' ',surname,name)";
	}
	else {
		// COD $columnName
		$table = "db_$columnName";
		$titleExpr = "name";
	}
	

	$sql = "SELECT $titleExpr AS title FROM $table WHERE id=$id";
	$title = MotlDB::queryValue( $sql );
	if( ! $title ) return null;
	
	$sql = "SELECT id FROM $table WHERE $titleExpr='".MRES($title)."'";
	$ids = MotlDB::queryValues( $sql );
	
	$items = $column->getItemListBy( "id", $ids );
	if( count($items) < 2 ) {
//		return "";
	}
	
	$buttons = array();
    foreach( $items as $item ) {
      $disambig = isset($item->disambig) ? "(".$item->disambig.")" : "";
	  $label = "$item->name$disambig";
	  $escapedLabel = Motl::htmlEscape( $label );
//      $buttons[] = "<input type=button value='$escapedLabel' onclick='document.all.$ctrlName.value=\"$item->id\"'>";
      $buttons[] = "<input type=button value='$escapedLabel' "
		."onclick='onButton(\"$ctrlName\",\"$varName\",$cellIndex,$item->id)'>";
	  }
	
	$sbuttons = implode( "<br>", $buttons );
	return $sbuttons;
}

//=====================

function strCond( $field, $value ) { // SAFE
	ASSERT_name($field);
	if( strlen($value) ) {
		return "$field='".MRES($value)."'";
	}
	else {
		return "($field='' Or $field Is Null)";
	}
}

//=====================

function makeModeSelector( $current ) {
	$sep = "&nbsp;&nbsp;&nbsp;";
	$s = "";
	$s .= makeModeSelectorItem( "diff", "конфликты", $current );
	$s .= $sep;
	$s .= makeModeSelectorItem( "twin", "омонимы", $current );
	$s .= $sep;
	$s .= makeModeSelectorItem( "all",  "все", $current );
	return $s;
}

//---------------------

function makeModeSelectorItem( $item, $label, $current ) {
	$script = $_SERVER['PHP_SELF'];
	return ($item==$current) ? "<b>$label</b>"
							: "<A href='$script?show=$item'>$label</a>";
}

//=====================

function makeReport() {
	echo "<A href='base.php?id=tools'>назад</a><p>";

	$mode = Motl::getParameter("show","diff");
	echo "Показывать: " . makeModeSelector( $mode ) . "<p>";
	
	$bcCountry = new BaseColumnCountry();
	$bcCity = new BaseColumnCity();
	$bcClub = new BaseColumnClub();
	$bcTeacher = new BaseColumnTeacher();
	
	echo makeHierarchyPair( $bcCountry, $bcCity,    $mode );
	echo makeHierarchyPair( $bcCity,    $bcClub,    $mode );
	echo makeHierarchyPair( $bcClub,    $bcTeacher, $mode );
	echo makeHierarchyPair( $bcTeacher, $bcClub,    $mode  );
}

//=====================

function getTwinIds( $column ) {
	$columnName = $column->getName();
	
	if( $columnName == "teacher" ) { // The only special case
		$table = "db_person";
		$titleExpr = "CONCAT_WS(' ',surname,name)";
	}
	else {
		// COD $columnName
		$table = "db_$columnName";
		$titleExpr = "name";
	}
	$sql = "SELECT $titleExpr AS title FROM $table GROUP BY title HAVING count(*)>1";
	//echo "$sql<p>";
	
	$values = MotlDB::queryValues( $sql );
	if( ! $values ) return null;
	
	$svalues = "'" . implode( "','", $values ) . "'";
	
	$sql = "SELECT id FROM $table WHERE $titleExpr In($svalues)";
	$ids = MotlDB::queryValues( $sql );
	return $ids;
}

//=====================

function loadTwinVariants( $column ) {

	$twinIds  = getTwinIds( $column );
	if( ! $twinIds ) {
		return null;
	}
	
	$field = $column->getName();
	$sql = "SELECT DISTINCT $field "
			."FROM db_couple_event WHERE  $field is not null"; // COD $field
	
	$variants = MotlDB::queryValues( $sql );
	
	$twinVariants = array();
	foreach( $variants as $variant ) { 
		$avariant = explode(",",$variant);
		
		if( array_intersect($avariant,$twinIds) ) {
			$twinVariants[] = $variant;
		}
	}
	
	if( ! $twinVariants ) {
		return null;
	}
	
	$sTwinVariants = "'" . implode("','",$twinVariants) . "'";
	return $sTwinVariants;
}

//=====================

function makeHierarchyPair( $colHigh, $colLow, $mode ) {
	$script = $_SERVER['PHP_SELF'];

	$fieldHigh = $colHigh->getName();
	$fieldLow  = $colLow->getName();
	
//	$normals = loadNormals( $fieldHigh, $fieldLow );
	
	if( $mode == "diff" ) {
		$sql = "SELECT $fieldLow "
			."FROM (SELECT DISTINCT $fieldHigh,$fieldLow "
					."FROM db_couple_event WHERE  $fieldLow is not null) as q "
			."GROUP BY $fieldLow "
			."HAVING count(*)>1"; // All Safe
	}
	else 
	if( $mode == "list" ) {
		$sids0 = Motl::getParameter("ids");
		ASSERT_ints($sids0);
		
		$sql = "SELECT DISTINCT $fieldLow "
				."FROM db_couple_event WHERE  id In($sids0)"; // All Safe
	}
	else 
	if( $mode == "twin" ) {
		$sVariantsLow  = loadTwinVariants($colLow);
		$sVariantsHigh = loadTwinVariants($colHigh);

		if( !$sVariantsLow && !$sVariantsHigh ) {
			$sql = "SELECT 1 FROM db_couple_event WHERE  false";
		}
		else {
			$conds = array();
			if( $sVariantsLow )  $conds[] = "$fieldLow In($sVariantsLow)";
			if( $sVariantsHigh ) $conds[] = "$fieldHigh In($sVariantsHigh)";
			$sconds = implode( " OR ", $conds );
			$sql = "SELECT DISTINCT $fieldLow "
				."FROM db_couple_event "
				."WHERE $sconds "; // UNSAFE
		}
	}
	else {
		$sql = "SELECT DISTINCT $fieldLow "
				."FROM db_couple_event WHERE  $fieldLow is not null"; // All Safe
	}

	$twinIdsLow  = getTwinIds( $colLow );
	$twinIdsHigh = getTwinIds( $colHigh );
	
	//echo "$sql<p>";
	$variants = MotlDB::queryValues( $sql );

	$s = "";

	$s .= "<font color='#000080' size=5><b><u>$fieldLow - $fieldHigh</u></b></font><p>";

	if( !$twinIdsLow && !$twinIdsHigh ) {
		return;
	}
	
	$tables = array();
	
	foreach( $variants as $variant ) { 
		$avariant = explode(",",$variant);
		$lowTwin = $twinIdsLow && array_intersect($avariant,$twinIdsLow);
		
		$svariant = $colLow->makeListImploded( $avariant, ", " );

		$table = "";

		// COD $fieldHigh, $fieldLow
		$sql = "SELECT DISTINCT $fieldHigh FROM db_couple_event WHERE $fieldLow='".MRES($variant)."' ";
		if( $mode == "list" ) {
			$sids0 = Motl::getParameter("ids");
			ASSERT_ints($sids0);
		
			$sql .= " AND id In($sids0)"; // UNSAFE
		}

		$parents = MotlDB::queryValues( $sql );
		//$table.=$sql;
		$table .= "\n<table cellspacing=0 cellpadding=3 border=1>\n";
		
		$highTwin = false;
		foreach( $parents as $parent ) { 
			$aparent = explode(",",$parent);
			$parentTwin = ($twinIdsHigh && array_intersect($aparent,$twinIdsHigh));
			$highTwin = $highTwin || $parentTwin;
			$sparent = $colHigh->makeListImploded( $aparent, ", " );
			
			$listUrl = "$script?action=listRegs&show=$mode&fl=$fieldLow&fh=$fieldHigh&flv=$variant&fhv=$parent";
			$listLink = "<A href='$listUrl'><i>список</i></a>";
			
			$rowStyle = $parentTwin ? " class='twinHilite'" :"";
			
			$table .= "<tr$rowStyle>";
			$table .= "<td>$parent&nbsp;</td>";
			$table .= "<td>$sparent&nbsp;</td>";
			$table .= "<td>$listLink</td>";
			$table .= "</tr>\n";
		}
		
		$table .= "</table>\n";
		
		$tvariant = strip_tags($svariant);
		
		$captionStyle = $lowTwin ? " class='twinHilite'" :"";

		if( $mode!="twin" || $lowTwin||$highTwin ) {
			$tables[$tvariant] = "<b$captionStyle>$svariant:</b><br>$table<p>";
		}
	}
	
	ksort( $tables );
	//print_r($tables);
	$s .= implode( "", $tables );

	return $s;
}

//=====================
/*
function loadNormals( $fieldHigh, $fieldLow ) {
	echo "loadNormals( $fieldHigh, $fieldLow )<p>";
}
*/
?>
<script>

function onButton(ctrlName,varName,cellIndex,itemId) {
	var ctrl = eval(ctrlName);
	var arr = eval(varName);
	arr[cellIndex] = itemId;
	var s = ""+arr
	ctrl.value = s
}

</script>
