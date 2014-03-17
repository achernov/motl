<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseColumn.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();


//=====================


	$sql = "SELECT comp.id as idcomp, dbce.idCouple, dbce.place, comp.nCouples, cpl.club "
			."FROM db_events AS evt "
			."INNER JOIN db_events AS comp ON evt.id=comp.pid "
			."INNER JOIN db_couple_event AS dbce ON dbce.idEvent=comp.id "
			."INNER JOIN db_couples AS cpl ON dbce.idCouple=cpl.id "
			."WHERE evt.date>'2010-08-01' AND evt.type='event' AND comp.type='competition' "
				."AND dbce.place<>''";
								
	$rows = MotlDB::queryRows( $sql );
	
	$clubscores = array();
	
	foreach( $rows as $item ) {
	
		if( $item->idcomp >= 1997 ) {
			$score = CalcScoreMore( $item->place, $item->nCouples );
		}
		else {
			$score = CalcScore( $item->place, $item->nCouples );
		}
		
		$sclubs = $item->club;
		if( strlen($sclubs) ) {
			$aclubs = explode( ",", $sclubs );
			foreach( $aclubs as $idClub ) {
				$clubscores[$idClub] ++;
			}
		}
		
	}
print_r($clubscores);
arsort($clubscores);
print_r($clubscores);

//=====================

function CalcScoreMore( $place, $ncouples ) {
//echo "==More==<br>";
	$table = array(14,10,8,7,6,5,4,3,2,1);

	$place = ((int)$place)-1;
	
	return $table[$place];
}

//=====================

function CalcScore( $place, $ncouples ) {
//echo "==score==<br>";
	$table = array(10,8,6,5,4,3,2,1);

	$place = ((int)$place)-1;
	
	return $table[$place];
}

//=====================
?>