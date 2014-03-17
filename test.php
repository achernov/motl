<?php 
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseUtils.inc";
//=====================
//============================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

$sql = "SELECT GROUP_CONCAT(id) AS idsCE, binary raw_teacher as names, teacher as ids FROM db_couple_event "
		."WHERE raw_teacher<>'' and id=145650 "
		."GROUP BY binary raw_teacher, teacher "
		//."ORDER BY raw_teacher"
		;
die($sql);
		
$sql = "SELECT binary raw_teacher as names FROM db_couple_event "
		."WHERE raw_teacher<>'' and id=145650 ";
		
$sql = "SELECT surname FROM db_person WHERE id=6028";		

$sql = "SELECT raw_teacher FROM db_couple_event WHERE id=145650";
		
		//die($sql);		
$rows = MotlDB::queryRows( $sql );

print_r( $rows );

die();

?>
