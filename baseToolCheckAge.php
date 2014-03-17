<?php
require_once "_motl_db.inc";
require_once "_dbproducer.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

//=========================================

define( "BAD_DATE", "---" );

//=========================================

function makeSqlOneAge( $prevMinAge, $minAge, $maxAge, $groupCodePatterns ) {
				
	$gc = array();
	foreach( $groupCodePatterns as $pattern ) {
		$gc[] = "e.groupCode Like '$pattern'";
	}
	$groupCodeCond = implode( " OR ", $gc );

	$prevCond = 
		$prevMinAge 
			? "(who.birthdate>(p.date - INTERVAL $prevMinAge YEAR))"
			: "false";
			
	$minCond = 
		$minAge 
			? "(who.birthdate>(p.date - INTERVAL $minAge YEAR))"
			: "false";
			
	$maxCond = 
		$maxAge 
			? "(who.birthdate<(p.date - INTERVAL $maxAge+1 YEAR))"
			: "false";
			
	$ageCond = "$prevCond OR $maxCond";
	//$ageCond = "$minCond OR $maxCond";
	//$ageCond = "$maxCond";
	
	$fieldsToSelect = "p.date, "
		."e.groupCode, e.pid, e.id as cid, ce.id as ceid, "
		."$prevCond as cPrev, $minCond as cMin, $maxCond as cMax, "
		."4*$maxCond+2*$prevCond+$minCond as score, "
		."who.birthdate, DATEDIFF(p.date,who.birthdate)/365.25 as age, "
		."who.id, who.surname, who.name";
	
	$sqlMan = "SELECT $fieldsToSelect "
			."FROM db_couple_event AS ce "
				."INNER JOIN db_events AS e  ON ce.idEvent=e.id "
				."INNER JOIN db_events AS p  ON p.id=e.pid "
				."INNER JOIN db_couples AS c ON ce.idCouple=c.id "
				."INNER JOIN db_person AS who  ON c.idMan=who.id "
			."WHERE e.type='competition' "
				."AND who.birthdate Is Not Null "
				."AND ($groupCodeCond) "
				."AND ($ageCond)";
				
	$sqlLady = "SELECT $fieldsToSelect "
			."FROM db_couple_event AS ce "
				."INNER JOIN db_events AS e  ON ce.idEvent=e.id "
				."INNER JOIN db_events AS p  ON p.id=e.pid "
				."INNER JOIN db_couples AS c ON ce.idCouple=c.id "
				."INNER JOIN db_person AS who  ON c.idLady=who.id "
			."WHERE e.type='competition' "
				."AND who.birthdate Is Not Null "
				."AND ($groupCodeCond) "
				."AND ($ageCond)";
				
	$sqlSolo = "SELECT $fieldsToSelect "
			."FROM db_couple_event AS ce "
				."INNER JOIN db_events AS e  ON ce.idEvent=e.id "
				."INNER JOIN db_events AS p  ON p.id=e.pid "
				."INNER JOIN db_person AS who  ON ce.idSolo=who.id "
			."WHERE e.type='competition' "
				."AND who.birthdate Is Not Null "
				."AND ($groupCodeCond) "
				."AND ($ageCond)"
//			."ORDER BY 4*cMax+2*cPrev+cMin DESC, surname, name"
			;
				
	$sql = "$sqlMan UNION $sqlLady UNION $sqlSolo";	
	
	return $sql;
}

$ageGroups = array(
/*		array(  0, 0,7, array('%._7.%','%._7') ),
		array(  0, 0,9, array('%._9.%','%._9','juv1.%') ),
		array(  0, 10,11, array('%.10_11.%','%.10_11','juv2.%') ),
		array(  0, 0,11, array('%._11.%','%._11','juv.%') ),
*/		array( 10, 12,13, array('%.12_13.%','%.12_13','jun1.%') ),
		array( 12, 14,15, array('%.14_15.%','%.14_15','jun2.%') ),
		array( 10, 12,15, array('%.12_15.%','%.12_15','jun.%') ),
/*		array( 14, 16,18, array('%.16_18.%','%.16_18','you1.%') ),
		array( 16, 19,20, array('%.19_20.%','%.19_20','you2.%') ),
		array( 14, 16,0, array('%.16_.%','%.16_','am.%') )*/
	);

$sqls = array();
foreach( $ageGroups as $group ) {
	$s = makeSqlOneAge( $group[0], $group[1],$group[2], $group[3] );
	$sqls[] = makeSqlOneAge( $group[0], $group[1],$group[2], $group[3] );
	//echo "$s<p>\r\n";
}
$sql = implode( " UNION ", $sqls );
$sql .= "ORDER BY score DESC, surname, name";

$rows = MotlDB::queryRows( $sql );
//print_r($rows);

echo "<table border=1 cellspacing=0>";
foreach( $rows as $row ) {
//print_r($row);die();
	$color = scoreColor( $row->score );
	$style = "font-weight:bold;color:$color";
	
	$urlE = "db.php?id=e$row->pid,c$row->cid&hc=$row->id";
	$urlP = "db.php?id=p$row->id";
	
	echo "<tr style='$style'>";
	echo "<td class='DbTableCell'>$row->groupCode</td>";
	echo "<td class='DbTableCell'>$row->age</td>";
	
	echo "<td class='DbTableCell'>"
		."<a style='$style' href='$urlE'>$row->date</a></td>";
	
	echo "<td class='DbTableCell'>$row->birthdate</td>";
	echo "<td class='DbTableCell'>$row->id</td>";
	
	echo "<td class='DbTableCell'"
		."<a style='$style' href='$urlP'>$row->surname $row->name</a></td>";
		
	echo "</tr>";
}
echo "</table>";

die();

function scoreColor( $score ) {
	switch( $score ) {
		case 0: return '#008000';
		case 1: return '#808000';
		case 3: return '#c06000';
		case 4: return '#c00000';
	}
}

$sql = "SELECT DISTINCT * FROM
(SELECT man.id, man.birthdate, ce.mbirthdate, ce.id as ceid, ce.idEvent
FROM db_couple_event AS ce INNER JOIN db_couples AS c ON ce.idCouple=c.id 
	INNER JOIN db_person AS man  ON c.idMan=man.id 
WHERE ce.mbirthdate<>''
UNION
SELECT lady.id, lady.birthdate, ce.fbirthdate, ce.id as ceid, ce.idEvent
FROM db_couple_event AS ce INNER JOIN db_couples AS c ON ce.idCouple=c.id 
	INNER JOIN db_person AS lady ON c.idLady=lady.id
WHERE ce.fbirthdate<>''
UNION
SELECT solo.id, solo.birthdate, ce.mbirthdate, ce.id as ceid, ce.idEvent
FROM db_couple_event AS ce INNER JOIN db_person AS solo  ON ce.idSolo=solo.id
WHERE ce.mbirthdate<>'') AS Q
ORDER BY id, ceid";
//die($sql);
$rows = MotlDB::queryRows( $sql );

$ids = array();
foreach( $rows as $r ) {
	$ids[] = $r->id;
}
$sids = implode( ",", $ids );

$nameRows = MotlDB::queryRows( "SELECT id, CONCAT_WS(' ',surname,name) As title "
							  ."FROM db_person WHERE id In($sids)" );
$names = array();
foreach( $nameRows as $r ) {
	$names[$r->id] = $r->title;
}
//print_r($names);die();
$curId = $rows[0]->id;
$curRows = array();
for( $k=0; $k<count($rows); $k++ ) {
	$row = $rows[$k];
	if( $row->id == $curId ) {
		$curRows[] = $row;
	}
	else {
		groupRows( $curId, $names[$curId], $curRows );
		//print_r( $curRows );
		$curId = $row->id;
		$curRows = array( $row );
	}
}
groupRows( $curId, $names[$curId], $curRows );

//=========================================

  
function groupRows( $id, $name, $rows ) {
	//if( $id != 1400) return;
	//echo "<p><b>$id $name</b><br>";
	//print_r( $rows );
	
	$date = $rows[0]->birthdate;
	
	$uniDate = uniformDate($rows);
	//die("qwe $id $uniDate");
	if( $date && $uniDate && ($date==$uniDate) ) {
		//echo "date OK: $id, $uniDate<br>";
	}
	else
	if( !$date && $uniDate ) {
		setBirthDate( $id, $uniDate );
	}
	else {
		makeCell( $id, $name, $date, $rows );
	}
	
}

//-----------------------------------------

function uniformDate( $rows ) {
	$date = null;
	foreach( $rows as $r ) {
		$d = normalizeDate($r->mbirthdate);
		//echo "$r->mbirthdate $d<br>";
		
		if( $d == BAD_DATE ) { // Если есть нечитаемая дата
			//echo "ud1<br>";
			return null; // То это по-любому не ОК
		}
		else 
		if( $d ) { // Если есть осмысленная дата
			//echo "q<br>";
			if( ! $date ) {
				$date = $d;
			}
			else {
				if( $d != $date ) {
					return null;
				}
			}
		}
		else { // Просто нет даты, разночтения нет
		}
	}
	return $date;
}

//-----------------------------------------

function reformatDate( $date ) {
	//-- patterns
	$pd12 = "[0-9]{1,2}";
	$pd2 = "[0-9]{2}";
	$pd4 = "[0-9]{4}";
	$psep = "[\/\-.]";
	$ps = '$';
	//-- tests
	//die("q".date("Y-m-d",strtotime("2008-8-1")));
	
	if( preg_match( "/^($pd12)$psep($pd12)$psep($pd4)$ps/", $date ) ) return $date;
	if( preg_match( "/^$pd4-$pd2-$pd2 $pd2:$pd2:$pd2$ps/", $date ) ) return $date;
	if( preg_match( "/^$pd4-$pd2-$pd2$ps/", $date ) ) return $date;

	if( preg_match( "/^($pd4).($pd12).($pd12)$ps/", $date, $matches ) ) {
		//print_r($matches);
		return "$matches[1]-$matches[2]-$matches[3]";
	}

	if( preg_match( "/^($pd2)$psep($pd2)$psep($pd2)$ps/", $date, $matches ) ) {
		$y = (int)$matches[3];
		if( $y>=60 && $y<=99 ) {
			$date = "$matches[1]-$matches[2]-19$matches[3]";
			//$dateTS = strtotime( $date );
			//$normDate = date( "Y-m-d", $dateTS );
			//echo( "YESS!======$date======$dateTS====$normDate=<br>");
			return $date;
		}
		echo "<font color=blue>rfd1 $date</font><p>";
		return null;
		die( "======$date======$y");
	}
	
	//echo "<font color=blue>rfd2 $date</font><p>";
	return null;	
}

//-----------------------------------------

function normalizeDate( $date ) {
	$date0 = trim($date);
	if( ! strlen($date0) ) return null;
	
	$date = reformatDate($date0);
	if( ! strlen($date) ) {
		//echo("<font color=blue>qqqqqq $date0</font><br>");
		return BAD_DATE;
	}
	
	$dateTS = strtotime( $date );
	if( ! $dateTS ) {
		echo("<font color=red>qqqqqq $date0</font><br>");
	}
	$normDate = $dateTS ? date( "Y-m-d", $dateTS ) : BAD_DATE;
    return $normDate;
}

//-----------------------------------------

function makeCell( $id, $name, $date, $rows ) {
	
	$dummy = new DbCoupleEventProducer();
	
	//echo "manual selection: $id, $name, $date<br>";
	echo "<p><b><A href='db.php?id=p$id'>$id - $name</a></b><br>";
	echo "<table>";
	echo "<tr bgcolor='#d0d0f0'><td></td><td>$date</td></tr>";
	foreach( $rows as $r ) {
		$d = normalizeDate( $r->mbirthdate );
		if( $d == BAD_DATE ) {
			$d = $r->mbirthdate; // Не нормализуется
			$d = "<font color='red'>$d</font>";
		}
		$url = makeRegAddr( $r->ceid );
		echo "<tr>";
		echo "<td>" . $dummy->makeEditLink( 'Edit', $r->ceid ) . "</td>";
		echo "<td><A href='$url'>$d</a></td>";
		echo "</tr>";
	}
	echo "</table>";
}

//-----------------------------------------

function makeRegAddr( $idCE ) {
	$row = MotlDB::queryRow( "SELECT ce.idEvent, e.pid, idCouple, idSolo "
						."FROM db_couple_event as ce "
						."INNER JOIN db_events AS e ON ce.idEvent=e.id "
						."WHERE ce.id='$idCE'" );
	//print_r( $row ); die();
	$idWho = $row->idCouple ? $row->idCouple : $row->idSolo;
	return "db.php?id=e$row->pid,c$row->idEvent&hc=$idWho'";
}

//-----------------------------------------

function setBirthDate( $id, $uniDate ) {
//	echo "setBirthDate: $id, $uniDate<br>";
	//echo "UPDATE db_person SET birthdate='$uniDate' WHERE id='$id'<p>";
	MotlDB::query( "UPDATE db_person SET birthdate='$uniDate' WHERE id='$id'" );
}

//=========================================
die();
//=====================
?>


SELECT man.id, man.surname, man.name, man.birthdate, ce.id, ce.mbirthdate
FROM db_couple_event AS ce INNER JOIN db_couples AS c ON ce.idCouple=c.id 
	INNER JOIN db_person AS man  ON c.idMan=man.id 
	INNER JOIN db_person AS lady ON c.idLady=lady.id 

-------------------

SELECT man.id, man.birthdate, ce.mbirthdate
FROM db_couple_event AS ce INNER JOIN db_couples AS c ON ce.idCouple=c.id 
	INNER JOIN db_person AS man  ON c.idMan=man.id 
WHERE ce.mbirthdate<>''
UNION
SELECT lady.id, lady.birthdate, ce.fbirthdate
FROM db_couple_event AS ce INNER JOIN db_couples AS c ON ce.idCouple=c.id 
	INNER JOIN db_person AS lady ON c.idLady=lady.id 
WHERE ce.fbirthdate<>''
UNION
SELECT solo.id, solo.birthdate, ce.mbirthdate
FROM db_couple_event AS ce INNER JOIN db_person AS solo  ON ce.idSolo=solo.id
WHERE ce.mbirthdate<>''
