<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );
header( "Expires: Tue, 01 Jun 1999 09:17:32 GMT" );

$ids = Motl::getParameter( "ids" );

MotlDB::connect();

ASSERT_ints($ids);
// ASS $ids
$names = MotlDB::queryValues( "SELECT login FROM users WHERE id In($ids)" );  
$s = implode( ", ", $names );
echo $s;

//=====================
?>
