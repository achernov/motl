<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_dbproducer_factory.inc";
require_once "initEventData.inc";
//=====================
header( "Content-Type: text/html; charset=utf-8" );
//=====================

MotlDB::connect();

//=====================

//print_r( $_REQUEST );
//print_r( $_FILES );

$idEvent = Motl::getParameter( 'id' );
$dmlGroupIds = Motl::getParameter( 'dmlGroupIds' );

if( $dmlGroupIds ) {
	//print_r( $_REQUEST );
	//print_r( $dmlGroupIds );
	importEvent( $dmlGroupIds );
}
else 
if( isset( $_FILES['datafile'] ) ) {
	loadFileToDB( $_FILES['datafile']['tmp_name'] );
	echo makeMatchForm( $idEvent );
}
else {
	echo makeUploadForm( $idEvent );
}

//=====================

function importEvent( $dmlGroupIds ) {
	foreach( $dmlGroupIds as $k=>$dmlGroupId ) {
		$dbGroupId = Motl::getParameter( "groupSelector$dmlGroupId" );
		importGroup( $dmlGroupId, $dbGroupId );
	}
}

//=====================

function importGroup( $dmlGroupId, $dbGroupId ) {
	$dml = unpackDataFromDB();
	$dmlGroups = getGroupsFromDml( $dml );
	$dmlGroupLabel = $dmlGroups[$dmlGroupId];
	echo "dmlGroupId=$dmlGroupId, dmlGroupLabel=$dmlGroupLabel, dbGroupId=$dbGroupId<br>";
}

//=====================

function makeUploadForm( $idEvent ) {
	$s = "";
	$s .= "<form enctype='multipart/form-data' method='POST' action='loadDml.php'>";
	$s .= "<input type=hidden name=id value=$idEvent>";
    $s .= "<input name='datafile' type='file' /><p>\n";
    $s .= "<input type='submit' value='Загрузить' />\n";
	$s .= "</form>";
	return $s;
}

//=====================

function loadFileToDB( $path ) {
	$doc = DOMDocument::load( $path );

	$dml = parseFile( $doc );

	packDataToDB( $dml );
}

//=====================

function makeMatchForm( $idEvent ) {

	$dml = unpackDataFromDB();
	
	$dmlGroups = getGroupsFromDml( $dml );
	$dbGroups = getGroupsFromDB( $idEvent );
	
	$s = "";
	$s .= "<form enctype='multipart/form-data' method='POST' action='loadDml.php'>";
	$s .= "<input type=hidden name=id value=$idEvent>";
	$s .= "<table border=1 cellspacing=0>\r\n";
	foreach( $dmlGroups as $k=>$dmlGroup ) {
		$s .= "<input type=hidden name=dmlGroupIds[] value=$k>\r\n";
		$s .= makeGroupMatcher( $k,$dmlGroup, $dbGroups );
	}
	$s .= "</table>\r\n";
    $s .= "<input type='submit' value='Дальше' />\n";
	$s .= "</form>";
	return $s;
}

//=====================

function makeGroupMatcher( $dmlGroupId, $dmlGroupLabel, $dbGroups ) {
	$options = "<option value=0>&lt;new&gt;";
	foreach( $dbGroups as $dbGroupId=>$dbGroupLabel ) {
		$options .= "<option value=$dbGroupId>$dbGroupLabel";
	}
	$selector = "<select name=groupSelector$dmlGroupId>$options</select>";
	return "<tr> <td><b>$dmlGroupLabel</b></td> <td>$selector</td> </tr>\r\n";
}

//=====================

function getGroupsFromDB( $idEvent ) {
	$rows = MotlDB::queryRows( "SELECT id, title FROM db_events "
						."WHERE pid=$idEvent AND type='competition' ORDER BY position" );
						
	$arr = array();
	foreach( $rows as $k=>$row ) {
		$arr[$row->id] = $row->title;
	}
	return $arr;
}

//=====================

function getGroupsFromDml( $dml ) {
	$arr = array();
	foreach( $dml->groups as $k=>$group ) {
		$arr[$k] = $group->label;
	}
	return $arr;
}

//=====================

function parseFile( $doc ) {

	$obj = new stdClass();
	
	$dd = $doc->childNodes->item(0);

	$obj->groups = array();
	
	if( $dd->nodeName == "DanceData" ) {
		
		$inGroups = childrenByTag( $dd, "GroupData" );

		foreach( $inGroups as $k=>$groupData ) {
			$group = parseGroup( $groupData );
			$obj->groups[] = $group;
		}
	}
	
	return $obj;
}

//=====================

function packDataToDB( $obj ) {
	MotlDB::query( "DROP TABLE IF EXISTS tmp" );
	MotlDB::query( "CREATE TABLE tmp(value MEDIUMTEXT) ENGINE=MyISAM" );
	
	$serial = serialize( $obj );
	MotlDB::query( "INSERT INTO tmp(value) VALUES('" . MRES($serial) . "')" );
}

//=====================

function unpackDataFromDB() {
	$serial = MotlDB::queryValue( "SELECT value FROM tmp" );
	return unserialize( $serial );
}

//=====================

function parseGroup( $groupData ) {

	$group = new stdClass();

	$header = childByTag( $groupData, "Header" );
	$inGroup = childByTag( $header, "Group" );

	$group->label = $inGroup->textContent;
	
	$reg = childByTag( $groupData, "Registration" );
	
	$group->couples = parseCouples( childByTag( $reg, "Couples" ) );
	//print_r($couples); die();
//print_r( childrenByTag( $groupData, "Judges" ) );die();
	$group->judges =  parseJudges( childByTag( $groupData, "Judges" ) );
	//print_r($judges); die();
	$group->results = parseResults(  childByTag( $groupData, "Results" ) );
	//print_r($group); die();
	return $group;
}

//=====================

function parseResults( $inResults ) {
	//print_r($inResults); die();
	$aRounds = childrenByTag( $inResults, "Round" );
	
	$rounds = array();
	foreach( $aRounds as $k=>$aRound ) {
		$round = parseRound( $aRound );
		//print_r($cpl); die();
		$rounds[ $round->number ] = $round;
	}
	return $rounds;
}

//=====================

function parseRound( $inRound ) {
	$round = new stdClass();
	$round->number = $inRound->getAttribute( "no" );
	
	$round->dances = parseDances( childByTag( $inRound, "Dances" ) );
	
	return $round;
}

//=====================

function parseDances( $inDances ) {

	$aDances = childrenByTag( $inDances, "Dance" );

	$dances = array();
	foreach( $aDances as $k=>$aDance ) {
		$dance = parseDance( $aDance );
		//print_r($cpl); die();
		$dances[ $dance->name ] = $dance;
	}
	return $dances;
}

//=====================

function parseDance( $inDance ) {
	//print_r($inDance); die();
	$dance = new stdClass();
	$dance->name = $inDance->getAttribute( "name" );
	
	$dance->results = parseDanceResults( $inDance );
	
	return $dance;
}

//=====================

function parseDanceResults( $inDance ) {

	$aResults = childrenByTag( $inDance, "Result" );

	$results = array();
	foreach( $aResults as $k=>$aResult ) {
		$result = parseDanceResult( $aResult );
		$results[ $result->number ] = $result;
	}
	return $results;
}

//=====================

function parseDanceResult( $inResult ) {
	//print_r($inDance); die();
	$result = new stdClass();
	$result->number = $inResult->getAttribute( "n" );
	$result->sum = $inResult->getAttribute( "sum" );
	$result->scores = $inResult->textContent;
	return $result;
}

//=====================

function parseJudges( $inJudges ) {
//print_r( $inJudges->childNodes->item(0) );die();	
	$aJudges = childrenByTag( $inJudges, "Judge" );

	$judges = array();
	foreach( $aJudges as $k=>$aJudge ) {
		$judge = parseJudge( $aJudge );
		//print_r($cpl); die();
		$judges[ $judge->number ] = $judge;
	}
	return $judges;
}

//=====================

function parseJudge( $inJudge ) {
	$judge = new stdClass();
	$judge->surname = $inJudge->getAttribute( "lastName" );
	$judge->name = $inJudge->getAttribute( "firstName" );
	
	$code = $inJudge->getAttribute( "id" );
	$judge->number = preg_replace( "/[^0-9]+/", " ", $code );
	
	return $judge;
}

//=====================

function parseCouples( $inCouples ) {
	$aCouples = childrenByTag( $inCouples, "Couple" );
	//print_r( $aCouples );
	$couples = array();
	foreach( $aCouples as $k=>$aCouple ) {
		$cpl = parseCouple( $aCouple );
		//print_r($cpl); die();
		$couples[ $cpl->number ] = $cpl;
	}
	return $couples;
}

//=====================

function parseCouple( $inCouple ) {

	$couple = new stdClass();
	
	$couple->man  = parsePerson( childByTag( $inCouple, "Male" ) );
	$couple->lady = parsePerson( childByTag( $inCouple, "Female" ) );
	
	$club = childByTag( $inCouple, "Club" );
	
	$couple->city = $club->getAttribute( "city" );
	$couple->club = $club->getAttribute( "name" );
	
	$couple->number = $inCouple->getAttribute( "n" );
	$couple->place = $inCouple->getAttribute( "place" );
	$couple->classPlace = $inCouple->getAttribute( "classPlace" );
	$couple->class = $inCouple->getAttribute( "class" );
	$couple->points = $inCouple->getAttribute( "points" );
	
	return $couple;
}

//=====================

function parsePerson( $inPerson ) {
	$person = new stdClass();
	$person->surname = $inPerson->getAttribute( "lastName" );
	$person->name = $inPerson->getAttribute( "firstName" );
	return $person;
}

//=====================

function childByTag( $node, $tag ) {
	$arr = childrenByTag( $node, $tag );
	return isset($arr[0]) ? $arr[0] : null;
}

//=====================

function childrenByTag( $node, $tag ) {
	$res = array();
	for( $k=0; $k<$node->childNodes->length; $k++ ) {
		$item = $node->childNodes->item($k);
		if( $item->tagName == $tag ) {
			$res[] = $item;
		}
	}
	//print_r($res);die("dddddddd");
	return $res;
}

//=====================
?>
