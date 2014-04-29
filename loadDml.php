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
$dmlGroupId = Motl::getParameter( 'dmlGroupId', -1 );
$dbGroupIds = Motl::getParameter( 'dbGroupIds' );

if( $dbGroupIds ) {
	echo continueCompetition( $idEvent, $dmlGroupId, $dbGroupIds );
}
else 
if( $dmlGroupId >= 0 ) {
	echo makeCompetitionForm( $idEvent, $dmlGroupId );
}
else 
if( isset( $_FILES['datafile'] ) ) {
	loadFileToDB( $_FILES['datafile']['tmp_name'] );
	echo makeSelectForm($idEvent);
}
else {
	echo makeUploadForm( $idEvent );
}

//=====================

function continueCompetition( $idEvent, $dmlGroupId, $dbGroupIds ) {
//phpinfo();
	$dml = unpackDataFromDB();
	$dmlGroup = $dml->groups[$dmlGroupId];
	
	$dbGroup = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=$dbGroupIds[0]" );
	//print_r( $dmlGroup );
	//print_r( $dbGroup );
	//print_r( $dbGroupIds );

	for( $k=1; $k<count($dbGroupIds); $k++ ) {
		mergeGroupTo( $dbGroupIds[$k], $dbGroupIds[0] );
	}

	$judges = prepareJudges( $dmlGroup );
	MotlDB::query( "UPDATE db_events SET judges='$judges' WHERE id=$dbGroupIds[0]" );
	
	$couples = prepareCouples($dmlGroup,$dbGroup);
	
	$s = "";
	
	$s .= "<h2>$dmlGroup->label</h2>";
	
	$s .= makeRegForm( $dbGroupIds[0], $couples );

	if( $dbGroup->regType == 'solo' ) {
		$scores = prepareRound( $dmlGroup->results[1] );
		$s .= makeRoundForm( $dbGroupIds[0], $couples, $scores, null );
	}
	else {
		
		$nrounds = count($dmlGroup->results);
		
		for( $k=1; $k<=$nrounds; $k++ ) {
			$scores = prepareRound( $dmlGroup->results[$k] );
			$s .= makeRoundForm( $dbGroupIds[0], $couples, $scores, $nrounds-$k );
		}
	}

	return $s;
}

//=====================

function prepareJudges( $dmlGroup ) {
	$judges = array();
	foreach( $dmlGroup->judges as $k=>$judge ) {
		if( $k > 0 ) {
			$judges[] = "$judge->surname $judge->name";
		}
	}
	
	return implode( "\r\n", $judges );
}

//=====================

function mergeGroupTo( $compFrom, $compTo ) {
	$sql = "UPDATE db_couple_event SET idEvent=$compTo WHERE idEvent=$compFrom";
	//die( $sql );
	MotlDB::query( $sql );
}

//=====================

function makeRegForm( $idComp, $couples ) {

	$s = "";
	
	$s .= "<form target=_blank method='POST' action='dbProcessEventCouples.php'>\r\n";
	$s .= "<input type=hidden name=idEvent value=$idComp>\r\n";
	
	$s .= "<input type=hidden name=number value='$couples->number'>\r\n";
	$s .= "<input type=hidden name=place value='$couples->place'>\r\n";
	$s .= "<input type=hidden name=dancer value='$couples->dancer'>\r\n";
	$s .= "<input type=hidden name=man value='$couples->man'>\r\n";
	$s .= "<input type=hidden name=lady value='$couples->lady'>\r\n";
	$s .= "<input type=hidden name=country value='$couples->country'>\r\n";
	$s .= "<input type=hidden name=city value='$couples->city'>\r\n";
	$s .= "<input type=hidden name=club value='$couples->club'>\r\n";
	$s .= "<input type=hidden name=teacher value='$couples->teacher'>\r\n";
	
    $s .= "<input type='submit' name='processFill' value='Регистрация' />\n";
	$s .= "</form>\r\n\r\n";
	
	return $s;
}

//=====================

function makeRoundForm( $idComp, $couples, $scores, $roundLevel ) {
//die("$roundLevel");
	$isPoints = !is_int($roundLevel);

	if( $isPoints ) {
		$button = "Scores";
	}
	else {
		$levels = array( "final", "1/2", "1/4", "1/8", "1/16" );
		$button = $levels[$roundLevel];
	}

	$s = "";
	
	$ndances = count($scores->dances);
	$sdances = implode( ",", $scores->names );
	
	$s .= "<form target=_blank method='POST' action='dbProcessEventCouples.php'>\r\n";
	$s .= "<input type=hidden name=idEvent value=$idComp>\r\n";
	$s .= "<input type=hidden name=dbf_round value=$roundLevel>\r\n";

	$s .= "<input type=hidden name=number value='$scores->number'>\r\n";
	
	if( $isPoints ) {
		$s .= "<input type=hidden name=place value='$scores->points'>\r\n";
	}
	else {
		$s .= "<input type=hidden name=place value='$scores->place'>\r\n";
	}
	
	$s .= "<input type=hidden name=ndances value='$ndances'>\r\n";
	$s .= "<input type=hidden name=dances value='$sdances'>\r\n";
	
	for( $k=0; $k<$ndances; $k++ ) {
		$k1 = $k+1;
		$s .= "<input type=hidden name=d$k1 value='".$scores->dances[$k]."'>\r\n";
	}
	
    $s .= "<input type='submit' name='processFill' value='$button' />\n";
	$s .= "</form>\r\n";
	
	return $s;
}

//=====================

function prepareRound( $dmlRound ) {

	$obj = new stdClass();
	
	$map = array(
		array( "code"=>"W", "name"=>"Вальс" ),
		array( "code"=>"T", "name"=>"Танго" ),
		array( "code"=>"V", "name"=>"Венский вальс" ),
		array( "code"=>"F", "name"=>"Медленный фокстрот" ),
		array( "code"=>"Q", "name"=>"Квикстеп" ),
		array( "code"=>"Ch", "name"=>"Ча-ча-ча" ),
		array( "code"=>"S", "name"=>"Самба" ),
		array( "code"=>"R", "name"=>"Румба" ),
		array( "code"=>"P", "name"=>"Пасодобль" ),
		array( "code"=>"J", "name"=>"Джайв" ),
		array( "code"=>"Polka", "name"=>"Полька" )
	);

	$obj->names = array();
	$codes = array();
	$tables = array();
	
	foreach( $map as $k=>$pair ) {
		$code = $pair["code"];
		if( isset($dmlRound->dances[$code]) ) {
			$dmlDance = $dmlRound->dances[$code];
			$obj->names[] = $pair["name"];
			$codes[] = $pair["code"];
			$tables[] = $dmlDance->results;
		}
	}
	
	$obj->numbers = array();
	$obj->number = "";
	$obj->place = "";
	foreach( $dmlRound->places as $i=>$res ) {
		//print_r($res);  
		if( ! in_array($res->number,$obj->numbers) ) {
			$obj->numbers[] = $res->number;
			$obj->number .= $res->number . "\r\n";
			$obj->place .= $res->place . "\r\n";
			
			$points = str_replace( ",", ".", $res->sum );
			$obj->points .= $points . "\r\n";
		}
	}
	
	/*print_r($names);
	print_r($codes);
	print_r($numbers);
	print_r($tables);*/
	
	$obj->dances = array();
	foreach( $codes as $k=>$code ) {
		//$k1 = $k+1;
		//$d = "d$k1";
		$dmlDance = $dmlRound->dances[$code];
		
		$obj->dances[$k] = "";
		
		foreach( $obj->numbers as $i=>$number ) {
			$res = $dmlDance->results[$number];
			$obj->dances[$k] .= tabulate( $res->scores ) . "\r\n";
		}
	}

	return $obj;
}

//=====================

function tabulate( $s ) {
	$arr = array();
	for( $k=0; $k<strlen($s); $k++ ) {
		$arr[] = substr($s,$k,1);
	}
	return implode( "\t", $arr );
}

//=====================

function prepareCouples( $dmlGroup, $dbGroup ) {
	$obj = new stdClass();
	
	$obj->numbers = array();
	$obj->number = "";
	$obj->dancer = "";
	$obj->man = "";
	$obj->lady = "";
	$obj->place = "";
	$obj->points = "";
	$obj->country = "";
	$obj->city = "";
	$obj->club = "";
	$obj->teacher = "";
	
	foreach( $dmlGroup->couples as $number=>$couple ) {
		$man = $couple->man;
		$lady = $couple->lady;
		$obj->numbers[] = $number;
		$obj->number .= "$number\r\n";
		$obj->dancer .= "$man->surname $man->name\r\n";
		$obj->man .= "$man->surname $man->name\r\n";
		$obj->lady .= "$lady->surname $lady->name\r\n";
		
		//if( $dbGroup->regType == 'solo' ) {
		$points = str_replace( ",", ".", $couple->points );
		$obj->points  .= "$points\r\n";
		
		$obj->place  .= "$couple->place\r\n";
		
		$obj->country  .= "$couple->country\r\n";
		$obj->city  .= "$couple->city\r\n";
		$obj->club  .= "$couple->club\r\n";
		$obj->teacher  .= "$couple->teacher\r\n";
		//echo "$number, $man->surname, $man->name<br>";
	}
	
	return $obj;
}

//=====================

function makeCompetitionForm( $idEvent, $dmlGroupId ) {
	$dml = unpackDataFromDB();
	$dmlGroup = $dml->groups[$dmlGroupId];
	//print_r( $dmlGroup );
	$dbGroups = getGroupsFromDB( $idEvent );
	//print_r( $dbGroups );
	
	$s = "";
	
	$s .= "<form enctype='multipart/form-data' method='POST' action='loadDml.php'>";
	$s .= "<input type=hidden name=id value=$idEvent>";
	$s .= "<input type=hidden name=dmlGroupId value=$dmlGroupId>";
	$s .= "<h2>$dmlGroup->label</h2>";
	
	$options = "<option value=0>&lt;new&gt;";
	foreach( $dbGroups as $dbGroupId=>$dbGroupLabel ) {
		$options .= "<option value=$dbGroupId>$dbGroupLabel";
	}
	$selector = "<select size=45 multiple name=dbGroupIds[]>$options</select>";
	
	$s .= $selector . "<p>";
	
    $s .= "<input type='submit' value='Дальше' />\n";
	$s .= "</form>";
	
	return $s;
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

function makeSelectForm($idEvent) {

	$dml = unpackDataFromDB();
	
	$dmlGroups = getGroupsFromDml( $dml );
	///$dbGroups = getGroupsFromDB( $idEvent );
	
	$s = "";
	//$s .= "<form enctype='multipart/form-data' method='POST' action='loadDml.php'>";
	//$s .= "<input type=hidden name=id value=$idEvent>";
	//$s .= "<table border=1 cellspacing=0>\r\n";
	foreach( $dmlGroups as $dmlGroupId=>$dmlGroupLabel ) {
		$s .= "<A target=_blank href='loadDml.php?id=$idEvent&dmlGroupId=$dmlGroupId'>$dmlGroupLabel</a><br>\r\n";
	}
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
	
	$round->places  = parseTotalResults( childByTag( $inRound, "Total" ) );

	$round->dances = parseDances( childByTag( $inRound, "Dances" ) );
	
	return $round;
}

//=====================

function parseTotalResults( $inTotal ) {

	$aResults = childrenByTag( $inTotal, "Result" );

	$results = array();
	foreach( $aResults as $k=>$aResult ) {
		$result = parseTotalResult( $aResult );
		$results[ $result->number ] = $result;
	}
	return $results;
}

//=====================

function parseTotalResult( $inResult ) {
	//print_r($inResult); die();
	$result = new stdClass();
	$result->number = $inResult->getAttribute( "n" );
	$result->sum = $inResult->getAttribute( "sum" );
	$result->place = $inResult->getAttribute( "place" );
	return $result;
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
	
	$couple->country = $club->getAttribute( "country" );
	$couple->city = $club->getAttribute( "city" );
	$couple->club = $club->getAttribute( "name" );
	
	$teacher = array();
	if( $club->getAttribute("trener1LastName") ) {
		$teacher[] = trim( $club->getAttribute("trener1LastName")." ".$club->getAttribute("trener1FirstName") );
	}
	if( $club->getAttribute("trener2LastName") ) {
		$teacher[] = trim( $club->getAttribute("trener2LastName")." ".$club->getAttribute("trener2FirstName") );
	}
	$couple->teacher = implode( ", ", $teacher );
	
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
