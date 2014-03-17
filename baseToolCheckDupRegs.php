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

echo "<A href='base.php?id=tools'>назад</a><p>";
/*
$rows = MotlDB::queryRows(	 "SELECT idEvent,idSolo,idCouple,count(*) as n "
							."FROM `db_couple_event` group by idEvent,idSolo,idCouple HAVING n>1" );
*/
$ids = MotlDB::queryValues(	 "SELECT idEvent "
							."FROM db_couple_event GROUP BY idEvent,idSolo,idCouple "
							."HAVING count(*)>1 "
							."ORDER BY idEvent" );
$sids = implode( ",", $ids );
//die($sids);
$sql = 	 "SELECT DISTINCT comp.regType, comp.id as cid, comp.title as ctitle, event.id as eid, event.title as etitle "
		."FROM db_events AS comp INNER JOIN db_events AS event ON comp.pid=event.id "
		."WHERE comp.id In($sids) ORDER BY comp.id";

//echo "$sql<p>";
		
$rows = MotlDB::queryRows( $sql );

for( $k=0; $k<count($rows); $k++ ) {
	$row = $rows[$k];
	$sort = ($row->regType=="solo") ? "dancer" : "man";
	echo "<A href='db.php?id=e$row->eid,c$row->cid&sort=$sort'>$row->etitle -&gt; $row->ctitle</a><br>";
}

//print_r( $rows );

//=====================


//=====================
?>
