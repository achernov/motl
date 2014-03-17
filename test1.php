<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseColumn.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

$programs = array (
	array( "B+A 12-15 La", array("ba.12_15.la") ),
	array( "B+A 12-15 St", array("ba.12_15.st") ),
	array( "B+A 16+ La", array("ba.16_.la") ),
	array( "B+A 16+ St", array("ba.16_.st") ),

	array( "C 12-15 La", array("c.12_15.la") ),
	array( "C 12-15 St", array("c.12_15.st") ),
	array( "C 16+ La", array("c.16_.la") ),
	array( "C 16+ St", array("c.16_.st") ),

	array( "D 10-11", array("d.10_11") ),
	array( "D 12-15", array("d.12_15","d.12_13","d.14_15") ),
	array( "D 16+ La", array("d.16_.la") ),
	array( "D 16+ St", array("d.16_.st") ),

	array( "E ..11", array("e._9","e.9_11","e.10_11") ),
	array( "E 12-15", array("e.12_15","e.12_13","e.14_15") ),
	array( "E 16+ La", array("e.16_.la") ),
	array( "E 16+ St", array("e.16_.st") ),

	array( "N3 ..11", array("n3._9","n3._11","n3.10_11") ),
	array( "N3 ..9", array("n3._9") ),
	array( "N3 10-11", array("n3.10_11") ),
	
	array( "N4 ..11", array("n4._9","n4.10_11") ),
	array( "N4 ..9", array("n4._9") ),
	array( "N4 10-11", array("n4.10_11") ),
	
	array( "N4 12-13", array("n4.12_13") ),
	
	array( "N5 ..11", array("n5._9","n5._11","n5.10_11") ),
	array( "N5 ..9", array("n5._9") ),
	array( "N5 10-11", array("n5.10_11") ),

	array( "Sen La", array("sen.la") ),
	array( "Sen St+La", array("sen.10") ),

	array( "Am St", array("am.st") ),
	array( "Am La", array("am.la") ),
	array( "Am St+La", array("am.10") ),
	
	array( "Youth St", array("you.st","you1.st","you2.st") ),
	array( "Youth La", array("you.la","you1.la","you2.la") ),
	array( "Youth St+La", array("you.10","you1.10","you2.10") ),
/*	
	array( "Youth2 St", array("you2.st") ),
	array( "Youth2 La", array("you2.la") ),
	array( "Youth2 St+La", array("you2.10") ),

	array( "Youth1 St", array("you1.st") ),
	array( "Youth1 La", array("you1.la") ),
	array( "Youth1 St+La", array("you1.10") ),
*/
	//	array( "Jun St", array("jun.st","jun1.st","jun2.st") ),
//	array( "Jun La", array("jun.la","jun1.la","jun2.la") ),

	array( "Jun2 St", array("jun2.st") ),
	array( "Jun2 La", array("jun2.la") ),
	array( "Jun2 St+La", array("jun2.10") ),

	array( "Jun1 St", array("jun1.st") ),
	array( "Jun1 La", array("jun1.la") ),
	array( "Jun1 St+La", array("jun1.10") ),

	//	array( "Juv St", array("juv.st","juv1.st","juv2.st") ),
//	array( "Juv La", array("juv.la","juv2.la") ),
//	array( "Juv St+La", array("juv","juv1.6","juv.8","juv2.8","juv.10") ),

	array( "Juv2 St", array("juv2.st") ),
	array( "Juv2 La", array("juv2.la") ),
	array( "Juv2 St+La", array("juv.8","juv2.8") ),

	array( "Juv1 St", array("juv1.st") ),
	array( "Juv1 St+La", array("juv1.6") ),
);
//$programs = array (	array( "C 16+ La", array("c.16_.la") ) };

foreach( $programs as $prog ) {
	echo "<big><u>$prog[0]</u></big><br>";
	echo MakeProgRating($prog[1]);
	echo "<p>";
}

//=====================

function MakeProgRating( $codes ) {

	$s = "";

	$scodes = "'" . implode( "','", $codes ) . "'";

	$sql = "SELECT comp.id as idcomp, dbce.idCouple, dbce.place, comp.nCouples "
								."FROM db_events AS evt "
								."INNER JOIN db_events AS comp ON evt.id=comp.pid "
								."INNER JOIN db_couple_event AS dbce ON dbce.idEvent=comp.id "
								."WHERE evt.date>'2010-08-01' AND evt.type='event' AND comp.type='competition' "
									."AND comp.groupCode In($scodes) "
									."AND dbce.place<>'' "
//									."AND idCouple=290 "
//									."AND idCouple=385 "
								."ORDER BY evt.date, comp.id";
								
	$rows = MotlDB::queryRows( $sql );
	
	$cplids = array();
	$cplscores = array();
	$compids = array();
	$ratings = array();
	$places = array();
	$ns = array();
	
	foreach( $rows as $item ) {
		$idCouple = $item->idCouple;
		$idComp   = $item->idcomp;
		if( ! in_array($idCouple,$cplids) ) {
			$cplids[] = $idCouple;
		}
		if( ! in_array($idComp,$compids)  ) {
			$compids[] = $idComp;
		}
		$ns[$idComp] = $item->nCouples;
		
		if( $item->idcomp >= 1997 ) {
			$cplscores[$idCouple][] = CalcScoreMore( $item->place, $item->nCouples );
		}
		else {
			$cplscores[$idCouple][] = CalcScore( $item->place, $item->nCouples );
		}
		//$ratings[$idCouple] += CalcScore( $item->place, $item->nCouples );
		$places[$idCouple][$idComp] = $item->place;
	}

    $colClub    = BaseColumn::createColumn( "club" );
    $colTeacher = BaseColumn::createColumn( "teacher" );

	
	$scplids = implode( ",", $cplids );
	
	$cpltitles = GetIndexed( "SELECT c.id, CONCAT_WS(' ',m.surname,m.name,'-',f.surname,f.name) as value "
						."FROM db_couples AS c LEFT JOIN db_person AS m ON c.idMan=m.id "
								."LEFT JOIN db_person AS f ON c.idLady=f.id "
						."WHERE c.id In($scplids)" );

	$cplclubs = GetIndexed( "SELECT c.id, c.club as value "
						."FROM db_couples as c "
						."WHERE c.id In($scplids)" );

	$cplteachers = GetIndexed( "SELECT c.id, c.teacher as value "
						."FROM db_couples as c "
						."WHERE c.id In($scplids)" );
						
	foreach( $cplids as $idCouple ) {
		$ratings[$idCouple] = CalcBestN( $cplscores[$idCouple] );
	}
	
	arsort($ratings);

//	$s .= "cplids="  . implode( ",", $cplids ) ."<br>";
//	$s .= "compids=" . implode( ",", $compids ) ."<br>";


	$useLinks = true;

	$s .= "<table border=1>\n";
	foreach( $ratings as $idCouple=>$score ) {
		$ctitle = $cpltitles[$idCouple];
		$clabel = $useLinks ? "<A href=db.php?id=c$idCouple>$ctitle</a>" : $ctitle;
		$sscore = round( $score, 2 );
		
		$clubs = $cplclubs[$idCouple];
		$clubs = $colClub->makeListImploded( explode(",",$clubs), ", " );
		
		$teachers = $cplteachers[$idCouple];
		$teachers = $colTeacher->makeListImploded( explode(",",$teachers), ", " );

		if( ! $useLinks ) {
			$clubs = strip_tags($clubs);
			$teachers = strip_tags($teachers);
		}
		
		$s .= "<tr> <td><nobr>$clabel</nobr></td> ";
		$s .= "<td >$clubs</td> ";
		$s .= "<td >$teachers</td> ";
		$s .= "<td style='color:#008000'>$sscore</td> ";
		
		foreach( $compids as $idComp ){
			$place = $places[$idCouple][$idComp];
			if( ! $place ) {
				$place="&nbsp";
			}
			else {
				$place .= " <small style='color:#808080'>/ $ns[$idComp]</small>";
			}
			$s .= "<td><nobr>$place</nobr></td>";
		}
		$s .= "</tr>\n";
	}
	$s .= "</table>\n";
	
	//$s .= $sql;
	return $s;
}

//=====================

function GetIndexed( $sql ) {
	$rows = MotlDB::queryRows( $sql );
	return IndexRows( $rows );
}

//=====================

function IndexRows( $rows ) {
	$out = array();
	foreach( $rows as $row ) {
	//print_r($row);
		$out[$row->id] = $row->value;
	}
	return $out;
}

//=====================

function CalcBestN( $scores ) {
	rsort( $scores );
	$n = 3;
	//$n = 7;
	$sum = 0;
	for( $k=0; $k<$n; $k++ ) {
		if( isset($scores[$k]) ) {
			$sum += $scores[$k];
		//	echo "+$scores[$k]<br>";
		}
	}
	$sss = implode(",",$scores);
	//echo "$sum = $sss<br>";
	return $sum;
}

//=====================

function NormPlace( $place ) {
	$aplace = explode( "-", $place );
	return (count($aplace)>1) ? ($aplace[0]+$aplace[1])/2.0 : $aplace[0]*1.0;
}

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
