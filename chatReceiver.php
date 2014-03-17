<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_motl_user.inc";
//=====================

MotlDB::connect();

$user = User::getCurrentUser();
$message = Motl::getParameter( "message" );
$message = unescape( $message );

echo "<pre>";
print_r($user);

if( $user && $message ) {
  $uid = $user->row->id;
  $time    = date( "Y-m-d H:i:s", time() );
  
  // FLD $uid
  // COD $time
  MotlDB::query( "INSERT INTO chat(messageTime,uid,message) VALUES('$time','$uid','".MRES($message)."')" );
}

//=====================
?>
