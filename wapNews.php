<?php 
require_once "_core.inc";
require_once "calendarContent.inc";
//=====================

$script = $_SERVER['PHP_SELF'];
$c = Motl::getParameter("id");
/*
$s = "";
foreach( $_SERVER as $k=>$v ) {
  $s .= "$"."_SERVER"."[".$k."] = $v<br>";
}
*/

MotlDB::connect();

$s  = "<wml><card>";

$s .= "<p><a href='wap.php'>Главная</a> <b>/ Новости</b></p>";

$s .= MotlDB::queryValue( "SELECT pageText FROM staticpages WHERE id='wapNews'" );

//$content = new CalendarContent( "2010009" );
//$content = new CalendarContent( );
//$s .= $content->produceWAP();

//$s .= "<p><table columns='3'><tr><td>1</td><td>2</td><td>3</td></tr></table></p>";
$s .= "</card></wml>";
Motl::writeWap( $s );

//=====================
?>
