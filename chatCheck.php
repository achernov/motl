<?php
require_once "_motl_db.inc";
require_once "_motl_user.inc";
//=====================

header( "Expires: Tue, 01 Jun 1999 09:17:32 GMT" );

MotlDB::connect();

$uid = User::getCurrentUID();
if( ! $uid ) die();

$timeout = 5;
$invisibles = "3,4,5";

// ASS $uid - ASSERT'ится внутри getCurrentUID()
MotlDB::query( "INSERT INTO chat_who(idUser,lastActive) VALUES( '$uid', Now() )" );  

MotlDB::query( "DELETE FROM chat_who WHERE Now()-lastActive>1" );

// COD $timeout, $invisibles
$people = MotlDB::queryValue( "SELECT convert(GROUP_CONCAT(idUser) USING latin1) "
                               ."FROM "
                               ." (SELECT DISTINCT idUser "
                                 ."FROM chat_who WHERE Now()-lastActive<$timeout AND idUser Not In($invisibles) "
                                 .") as uids", 
                             "" );

//--- Last Message
$maxMsg = MotlDB::queryValue( "SELECT max(id) FROM chat", 0 );

//--- Output

echo "$maxMsg:$people";

//=====================
?>