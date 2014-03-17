<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

echo "<A href='base.php?id=tools'>назад</a><br>";

processAllEvents();

//=====================

function processAllEvents() {

	echo "<pre>";

	$rows = MotlDB::queryRows( "SELECT id,date,title,havePlaces,haveScores FROM db_events "
							."WHERE id=562 and pid is null AND type='event' "
							."ORDER BY date DESC" );
							
	foreach( $rows as $row ) {
		if( $row->havePlaces || $row->haveScores ) {
			processEvent( $row );
		}
	}
  
}

//=====================

function processEvent( $eventRow ) {

	echo "<p><big><big><b>$eventRow->date $eventRow->title</b></big></big><p>\n";

	$sql = "SELECT id,pid,title,type,groupCode,ratingLevel,havePlaces,haveScores FROM db_events "
		."WHERE id=1455 and pid=".MRES($eventRow->id)." AND type='competition' AND (regType is null OR regType='couple') "
		."ORDER BY position,id";

	$rows = MotlDB::queryRows( $sql );
                     
	foreach( $rows as $row ) {
		if( $row->havePlaces || $row->haveScores ) {
			processComp( $row );
		}
	}
}

//=====================

function processComp( $compRow ) {

	echo "<br><b>$compRow->title</b><br>\n";
	print_r($compRow);
	$isRating = checkRating( $compRow->groupCode ) ? 1 : 0;

	$rows = loadCouples( $compRow->id, false );
	
	$points = null;
	
	if( $isRating ) {
		switch( $compRow->ratingLevel ) {
			case 'A': $points = calcPointsRatingA( $rows ); break;
			case 'B': $points = calcPointsRatingB( $rows ); break;
			default:  $points = calcPointsRatingC( $rows ); break;
		}
	}
	else {
		$points = calcPointsClassification( $rows ); break;
	}
	
	print_r( $points );
}

//=====================

function calcPointsRatingA( $rows ) {
}

//=====================

function calcPointsRatingB( $rows ) {
}

//=====================

function calcPointsRatingC( $rows ) {
}

//=====================

function calcPointsClassification( $rows ) {
}

//=====================

function checkRating( $groupCode ) {
	$part = substr($groupCode,0,2);
	$ratingParts = array('ju','yo','am','se');
	return in_array( $part, $ratingParts );
}

//=====================

function loadCouples( $idComp, $loadInfo ) {

	$idComp = (int)$idComp;

	$selFields = "ce.id,idCouple,place,number";
	$selTables = "db_couple_event AS ce";
	
	if( $loadInfo ) {
		$selFields .= ",CONCAT_WS(' ',man.surname,man.name,'-',"
						."lady.surname,lady.name) AS couple";
		
		$selTables .= " INNER JOIN db_couples as cpl ON ce.idCouple=cpl.id "
					." INNER JOIN db_person as man ON cpl.idMan=man.id "
					." INNER JOIN db_person as lady ON cpl.idLady=lady.id ";
	}

	// COD $idComp
	$sql = "SELECT $selFields FROM $selTables "
			."WHERE idEvent=$idComp "
			."ORDER BY cast(ce.place as unsigned)";
//	echo "$sql<br>";
	$rows = MotlDB::queryRows( $sql );
	return $rows;
}

//=====================
?>
