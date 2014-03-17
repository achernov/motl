<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );
header( "Expires: Tue, 01 Jun 1999 09:17:32 GMT" );

$idFrom = Motl::getIntParameter( "idFrom" );
$idTo   = Motl::getIntParameter( "idTo" );

MotlDB::connect();

// ASS $idFrom, $idTo
$rows = MotlDB::queryRows( "SELECT chat.id, users.login, messageTime, message "
                          ."FROM chat INNER JOIN users ON chat.uid=users.id "
                          ."WHERE chat.id>'$idFrom' and chat.id<='$idTo' "
                          ."ORDER BY chat.id" );  

$maxRows = 200;
$start = max( 0, count($rows)-$maxRows );

for( $k=$start; $k<count($rows); $k++ ) {
  $time  = date( "d M H:i:s", strtotime($rows[$k]->messageTime) );
  $login = $rows[$k]->login;
  $text  = $rows[$k]->message;
  //echo "$login, $text<p>";
  echo "<b>$time - $login:</b><br>";
  echo "<div style='padding-left:20;padding-bottom:5'>$text</div>";

}

//=====================
?>
