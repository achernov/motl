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

$sql = makeBadRegSelector( "idCouple" );

$ids = MotlDB::queryValues( $sql );
$sids = implode( ",", $ids );

//$sql = "SELECT id, WS_CONCAT(' ',)"
$sql = "SELECT couple.id, "
			."CONCAT_WS(' ',man.surname,man.name, '-', lady.surname,lady.name) as title, "
			."couple.club "
		."FROM db_couples AS couple "
		."INNER JOIN db_person AS man ON couple.idMan=man.id "
		."INNER JOIN db_person AS lady ON couple.idLady=lady.id "
		."WHERE couple.id In($sids)";

$coupleRows = MotlDB::queryRows( $sql );

echo "<A href='base.php?id=tools'>назад</a><br>";
	
//print_r( $coupleRows );
$colClub = new BaseColumnClub();
foreach( $coupleRows as $cRow ) {
	$clubIds = explode( ",", $cRow->club );
	$cRow->textClub = $colClub->sidsToSvalues( $cRow->club, ", " );
	$cRow->htmlClub = $colClub->makeListImploded( $clubIds, ", " );
}

sortObjArrayByProps( $coupleRows, array("textClub","title") );

$prevClubs = "";
foreach( $coupleRows as $cRow ) {
	if( $cRow->club != $prevClubs ) {
		echo "<p style='margin:0;padding-top:20px' ><big>$cRow->htmlClub</big><br>";
		$prevClubs = $cRow->club;
	}
	echo "<p style='margin:0;padding-top:3px' ><A href=db.php?id=c$cRow->id>$cRow->title</a></p>";
	$links = makeRegLinks( $cRow->id );
	echo "<p style='margin:0; font-family:arial; font-size:12'>$links</p>";
}

//=====================



function makeBadRegSelector( $what, $cond="1" ) {

	$ratingCond = "(dbe.groupCode Like 'am%' Or dbe.groupCode Like 'ju%' "
					."Or dbe.groupCode Like 'se%' Or dbe.groupCode Like 'yo%')";
			
	$sql = "SELECT $what "
			."FROM db_couple_event AS dbce INNER JOIN db_events AS dbe ON dbce.idEvent=dbe.id "
			." INNER JOIN db_events AS event ON dbe.pid=event.id "
			."WHERE $ratingCond AND event.date>'2012-09-01' AND (dbce.class Is Null OR dbce.class='' OR 0=LOCATE(dbce.class,'MSABCDEN')) AND $cond";
	//die($sql);
	return $sql;
}

//=====================

function makeRegLinks( $idCouple ) {
	$sql = makeBadRegSelector( "dbe.id, dbe.pid, dbe.ratingLevel, dbce.id as title", "idcouple=$idCouple" );

	$rows = MotlDB::queryRows( $sql );
	$items = array();
	foreach( $rows as $row ) {
		$color = ( $row->ratingLevel=='A' || $row->ratingLevel=='B' ) ? '#a0a0ff' : 'black';
		$items[] = "<A style='color:$color' href='db.php?id=e$row->pid,r$row->id&hc=$idCouple'>$row->title</a>";
	}
	$sitems = implode( ", ", $items );
	return $sitems;
}

//=====================
function sortObjArrayByProps( &$array, $propNamesArray ) {

	$order = array();

	foreach( $array as $key=>$obj ) {
		$v = "";
		foreach( $propNamesArray as $pn ) {
			$v .= $obj->$pn . " ";
		}
		
		$order[$key] = $v;
	}
	
	asort( $order );
	
	$tmp = array();
	foreach( $order as $key=>$value ) {
		//echo "
		//$order[$key] = $obj->$propName;
		$tmp[] = $array[$key];
	}
	
	for( $k=0; $k<count($tmp); $k++ ) {
		$array[$k] = $tmp[$k];
	}
}

//=====================
?>
