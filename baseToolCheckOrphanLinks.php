<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";

require_once "dbAll.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

checkOrphanLinks();

//=====================


function checkOrphanLinks() {

	$modelCoupleEvent = new DbCoupleEventProducer();
	$modelEvent       = new DbEventProducer();

  echo "<A href='base.php?id=tools'>вернуться</a>";

  //----------
  
  $sql = "SELECT ce.id "
        ."FROM db_couple_event AS ce LEFT JOIN db_events AS e ON ce.idEvent=e.id "
        ."WHERE e.id Is Null";

  $ids = MotlDB::queryValues( $sql );
  $sids = makeList( $modelCoupleEvent, $ids );
  echo "<h2>db_couple_event без db_events:</h2>$sids<br>";
  //echo "удалить<p>";
  
  //----------
  
  $sql = "SELECT ce.id "
        ."FROM db_couple_event AS ce LEFT JOIN db_couples AS c ON ce.idCouple=c.id "
        ."LEFT JOIN db_person AS p ON ce.idSolo=p.id "
        ."WHERE c.id Is Null AND p.id Is Null";
//die($sql);
  $ids = MotlDB::queryValues( $sql );
  $sids = makeList( $modelCoupleEvent, $ids );
  echo "<h2>db_couple_event без участников:</h2>$sids<br>";
  echo "<p>";
  
  //----------
  
  $sql = "SELECT e.id "
        ."FROM db_events AS e LEFT JOIN db_events AS p ON e.pid=p.id "
        ."WHERE p.id Is Null AND e.type<>'event'";

  $ids = MotlDB::queryValues( $sql );
  $sids = makeList( $modelEvent, $ids );
  echo "<h2>Нетоповые события без parent:</h2>$sids<br>";
  echo "<p>";
  
  //----------
  
  $sql = "SELECT e.title "
        ."FROM db_events AS e INNER JOIN db_events AS p ON e.pid=p.id "
        ."WHERE e.type='event'";

  $ids = MotlDB::queryValues( $sql );
  $sids = makeList( $modelEvent, $ids );
  echo "<h2>Топовые события с parent:</h2>$sids<br>";
  echo "<p>";
  
  //----------
//  echo $sql;
}

//=====================

function makeList( $model, $ids ) {
	$list = array();
	foreach( $ids as $id ) {
		$elink = $model->makeEditLink( "Edit", $id );
		$klink = $model->makeKillLink( "Del", $id );
		$link = method_exists( $model, "makeViewLink" ) ? $model->makeViewLink($id,$id) : $id;
		
		$list[] = "$elink&nbsp;$klink&nbsp;$link";
	}
	
	$s = implode( "<br>", $list );
	
	if( count($list) ) {
		$table = $model->getTableName();
		$sids = implode( ",", $ids );
		$sql = "SELECT * FROM $table WHERE id In($sids)";
		
		$s .= "<p>$sql";
	}
	
	return $s;
}

//=====================

function linkedImplode( $url, $ids ) {
  $out = array();
  foreach( $ids as $id ) {
    $out[] = "<A href='$url$id'>$id</a>";
  }
  return implode( ", ", $out );
}

//=====================
?>
