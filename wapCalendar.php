<?php 
require_once "_core.inc";
require_once "calendarContent.inc";
//=====================

$script = $_SERVER['PHP_SELF'];

MotlDB::connect();

$s  = "<wml><card>";

$s .= "<p><a href='wap.php'>Главная</a> <b>/ Соревнования</b></p>";

$content = new CalendarContent( "2010009" );
$content = new CalendarContent( );
$s .= $content->produceWAP();

$s .= "</card></wml>";
Motl::writeWap( $s );

//=====================
?>
