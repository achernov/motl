<?php
require_once "_motl_user.inc";
//=====================

$myIPs = array( "192.168.6.117"=>1, "78.153.156.131"=>1 );

function writeLog() {
//---  Is It The Author Himself?
global $myIPs;
  if( isset( $myIPs[ $_SERVER['REMOTE_ADDR'] ] ) ) {
    return;
  }

//---  No, go on

  $user = User::getCurrentUser();

  $fields = array();
  $fields['hitDay']  = date( "Y-m-d", time() );
  $fields['hitTime'] = date( "Y-m-d H:i:s", time() );
  $fields['script']  = $_SERVER['PHP_SELF'];
  $fields['idparam'] = Motl::getParameter('id');
  $fields['ip']      = $_SERVER['REMOTE_ADDR'];
  $fields['idUser']  = $user ? $user->getId() : null;

  $id = MotlDB::saveRow( "logTable", "id",null, $fields, false, true );
  //die( "==$id==" );
  if( ! $id ) {
  
    MotlDB::query( "CREATE TABLE  logtable ("
                  ."  id int(11) NOT NULL auto_increment,"
                  ."  hitDay date default NULL,"
                  ."  hitTime datetime NOT NULL,"
                  ."  script varchar(50) default NULL,"
                  ."  idparam varchar(50) default NULL,"
                  ."  ip varchar(50) default NULL,"
                  ."  idUser int(11) default NULL,"
                  ."  PRIMARY KEY  (id),"
                  ."  KEY indexDay (hitDay),"
                  ."  KEY indexDayUser (hitDay,ip,idUser),"
                  ."  KEY indexDayPage (hitDay,script,idparam)"
                  .") ENGINE=MyISAM DEFAULT CHARSET=utf8" );
    $id = MotlDB::saveRow( "logTable", "id",null, $fields, false, true );
  }
}

//=====================
?>
