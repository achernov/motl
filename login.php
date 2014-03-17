<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================

MotlDB::connect();

//=====================

$username = Motl::getParameter( "lgnuser" );
$passHash = Motl::getParameter( "lgnpassHash" );
$remember = Motl::getParameter( "remember" );

//if( ctype_alnum($username) ) {
//  ASSERT_alnum( $username );

  $row = MotlDB::queryRow( "SELECT * FROM users WHERE login='".MRES($username)."'" );
  if( $row ) {
    $uid = $row->id;
  }

  $error = checkUserError( $row, $passHash );

if( ! $error ) {
  $expire = $remember ? time()+60*60*24*30 : 0;

  setcookie( "motlUid",  "$uid",      $expire );
  setcookie( "motlHash", "$passHash", $expire );
}

//=====================

function checkUserError( $row, $passHash ) { // Returns error

  if( ! $row ) {
    return "nouser";
  }

  $dbpass = $row->passHash;

  if( $passHash != $dbpass ) {
    return "badpass";
  }

  if( $row->userState == "NotActivated" ) {
    return "notactivated";
  }

  if( $row->userState == "banned" ) {
    return "banned";
  }

  return false;
}

//=====================

$returl = Motl::getParameter( "returl" );
$returlUrlencoded = urlencode($returl);

if( $error ) {
  setcookie( "motlLoginError", $error );
  Motl::redirect( "loginPage.php?returl=$returlUrlencoded" );
}
else 
if( $returl ) {
  Motl::redirectToRetUrl();
}
else { // Если непонятно, куда идти - идем в форум
  Motl::redirect( "forum.php" );
}

//=====================
?>
