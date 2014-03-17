<?php
require_once "_motl_db.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=========================================
//print_r( $_REQUEST ); echo "<p>";die();
//=========================================

updateLinks();
echo "Done. " . date( "Y-m-d H:i:s", time() );

function updateLinks() {
	
	$tmp = "tmp_link";
	
	MotlDB::query( "DROP TABLE IF EXISTS $tmp" );
	createLinkTableIfNOtExists( $tmp, "TEMPORARY", "MEMORY" );
	
	fillRelationCouplesAndSolo( $tmp, "country" );
	fillRelationCouplesAndSolo( $tmp, "city" );
	fillRelationCouplesAndSolo( $tmp, "club" );
	fillRelationCouplesAndSolo( $tmp, "teacher" );

	fillCoupleCount( $tmp, "person-nCouples" );

	fillEventStat( $tmp, "idCouple", "couple-nEvents" );
	fillEventStat( $tmp, "idSolo",   "person-nEvents" );

	fillMediaStat( $tmp, "idCouple", "couple-haveMedia" );
	fillMediaStat( $tmp, "idSolo",   "person-haveMedia" );
	
	updateLinksFromTmp( $tmp );
	
	MotlDB::query( "DROP TABLE IF EXISTS $tmp" );
}

//=========================================

function updateLinksFromTmp( $tmptable ) {
	
	createLinkTableIfNOtExists( "db_link", "", "MyISAM" );
	
	$sqlAdd = 
		"SELECT t.* "
		."FROM $tmptable t LEFT JOIN db_link d "
			."ON t.id1=d.id1 AND  t.id2=d.id2 AND  t.tag=d.tag "
		."WHERE d.id is null";
	$rowsAdd = MotlDB::queryRows( $sqlAdd );

	$sqlDel = 
		"SELECT d.id "
		."FROM $tmptable t RIGHT JOIN db_link d "
			."ON t.id1=d.id1 AND  t.id2=d.id2 AND  t.tag=d.tag "
		."WHERE t.id is null";
	$idsDel = MotlDB::queryValues( $sqlDel );
	
	if( count($idsDel) ) {
		$sidsDel = implode( ",", $idsDel );
		$sqlDoDel = "DELETE FROM db_link WHERE id In($sidsDel)";
		MotlDB::query( $sqlDoDel );
	}
	
	foreach( $rowsAdd as $row ) {
		$sqlDoAdd = "INSERT INTO db_link(id1,id2,tag)"
				."VALUES('$row->id1','$row->id2','$row->tag')";
		MotlDB::query( $sqlDoAdd );
	}
}

//=========================================

function createLinkTableIfNOtExists( $name, 
				$temporary="", $engine="MyISAM" ) {	
	
	$temporary="";
	
	MotlDB::query(
"CREATE $temporary TABLE IF NOT EXISTS $name (
	id int(10) unsigned NOT NULL auto_increment,
	id1 int(10) unsigned NOT NULL,
	id2 int(10) unsigned NOT NULL,
	tag varchar(45) NOT NULL,
	PRIMARY KEY  (id),
	UNIQUE KEY UniqueAll USING BTREE (id1,id2,tag),
	KEY Index_id1_tag USING BTREE (id1,tag),
	KEY Index_id2_tag USING BTREE (id2,tag)
) ENGINE=$engine DEFAULT CHARSET=utf8"
	);
}
	
//=========================================

function fillRelationCouplesAndSolo( $outtable, $listFld ) {
	fillRelation( $outtable, "db_couples", "id", $listFld, "couple-$listFld" );
	fillRelation( $outtable, "db_person",  "id", $listFld, "person-$listFld" );
}

//-----------------------------------------

function fillRelation( $outtable, $table, $idFld, $listFld, $tag ) {
	
	$sql = "SELECT DISTINCT $idFld as id, $listFld as lst FROM $table WHERE $listFld<>''";
	$rows = MotlDB::queryRows( $sql );

	//echo "$sql<br>";
	
	$hash = array();
	foreach( $rows as $row ) {
		$id = $row->id;
		$list = explode( ',', $row->lst );
		foreach( $list as $link ) {
			//echo "$id:$link<br>";
			$hash["$id:$link"] = 1;
		}
	}
	
	foreach( $hash as $h=>$v ) {
		$pair = explode( ':', $h );
		$id   = $pair[0];
		$link = $pair[1];
		$sql = "INSERT IGNORE INTO $outtable(id1,id2,tag) VALUES('$id','$link','$tag')";
		MotlDB::query( $sql );
	}
	
}

//-----------------------------------------

function fillCoupleCount( $outtable, $tag ) {
	$sql = 
		"INSERT INTO $outtable(id1,id2,tag) "
		."SELECT idPerson, count(distinct idCouple) AS N, '$tag' "
		."FROM ( "
			."SELECT idMan as idPerson,id as idCouple "
				."FROM db_couples "
				."WHERE split is null or not split "
			."UNION ALL "
			."SELECT idLady as idPerson,id as idCouple "
				."FROM db_couples "
				."WHERE split is null or not split "
			.") Q "
		."GROUP BY idPerson";
	//die($sql);
	
	MotlDB::query( $sql );
}
	
//-----------------------------------------

function fillEventStat( $outtable, $idFld, $tag ) {
	
	//$rows = MotlDB::query( "DELETE FROM $outtable WHERE tag='$tag'" );
	
	$sql = "INSERT INTO $outtable(id1,id2,tag)
		SELECT $idFld, count(DISTINCT db_events.pid), '$tag' 
		FROM db_couple_event INNER JOIN db_events ON db_couple_event.idEvent=db_events.id 
		WHERE db_events.type='competition' and $idFld<>''
		GROUP BY $idFld";
	$rows = MotlDB::query( $sql );

	//echo "$sql<br>";
}

//-----------------------------------------

function fillMediaStat( $outtable, $idFld, $tag ) {
	
	//$rows = MotlDB::query( "DELETE FROM $outtable WHERE tag='$tag'" );
	
	$sql = "INSERT INTO $outtable(id1,id2,tag)
		SELECT DISTINCT $idFld, 1, '$tag'
		FROM db_couple_event AS ce INNER JOIN db_events AS e ON ce.idEvent=e.id
		WHERE idGallery<>''";
		
	$rows = MotlDB::query( $sql );

	//echo "$sql<br>";
}

//=========================================
?>
