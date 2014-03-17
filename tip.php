<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================

MotlDB::connect();

$context = Motl::getParameter( "context", "reg" );
$field = Motl::getParameter( "field" );
$start = Motl::getParameter( "start" );

$startEscaped = MRES($start);
// COD $startEscaped
$queries['reg']['msurname'] = "SELECT DISTINCT surname FROM db_person WHERE sex='M' and surname like '$startEscaped%'";
$queries['reg']['mname']    = "SELECT DISTINCT name FROM db_person    WHERE sex='M' and name like '$startEscaped%'";
$queries['reg']['fsurname'] = "SELECT DISTINCT surname FROM db_person WHERE sex='F' and surname like '$startEscaped%'";
$queries['reg']['fname']    = "SELECT DISTINCT name FROM db_person    WHERE sex='F' and name like '$startEscaped%'";
$queries['reg']['club']     = "SELECT DISTINCT raw_club FROM db_couple_event WHERE raw_club like '$startEscaped%'";
$queries['reg']['teacher']  = "SELECT DISTINCT raw_teacher FROM db_couple_event WHERE raw_teacher like '$startEscaped%'";
$queries['reg']['city']     = "SELECT DISTINCT raw_city FROM db_couple_event WHERE raw_city like '$startEscaped%'";
$queries['reg']['country']  = "SELECT DISTINCT raw_country FROM db_couple_event WHERE raw_country like '$startEscaped%'";

$sql = null;

if( isset( $queries[$context] ) ) {
  if( isset( $queries[$context][$field] ) ) {
    $sql = $queries[$context][$field];
  }
}

if( ! $sql ) {
  die();
}

$avalues = MotlDB::queryValues( $sql );  
$nvalues = count($avalues);
$size = min(10,$nvalues);
$array = $nvalues ? "Array('".implode( "','", $avalues )."')" : "Array()";

header( "Content-Type: text/html; charset=utf-8" );
die( "Русский текст - для правильной кодировки в IE\n"
    ."$start\n" 
    ."<script>window.parent.onTip('$field',$array)</script>\n" );

//=====================
?>

