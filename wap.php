<?php 
//setcookie( "prevVisit", "" );setcookie( "lastHit", "" );die();
require_once "_core.inc";
require_once "calendarContent.inc";
//=====================

  
$script = $_SERVER['PHP_SELF'];
//$c = Motl::getParameter("id");
/*
$s = "";
foreach( $_SERVER as $k=>$v ) {
  $s .= "$"."_SERVER"."[".$k."] = $v<br>";
}
*/
  
$timeTest = date( "d.m.Y H:i:s", getPrevVisitTime() );
//  $prevVisit = isset($_COOKIE["prevVisit"]) ? $_COOKIE["prevVisit"] : 0;
//  $lastHit   = isset($_COOKIE["lastHit"]) ? $_COOKIE["lastHit"] : 0;

//print_r($_COOKIE);
//$timeTest = implode(";",$_COOKIE);
  
$s  = "<wml><card>";

$s .= "<p>";
$s .= "$timeTest<br/>";

//$s .= "$timeTest-$prevVisit-$lastHit<br/>";
$s .= "<a href='wapReg.php'>Регистрация</a><br/>";
$s .= "<a href='wapNews.php'>Новости</a><br/>";
$s .= "<a href='wapForum.php'>Форум</a><br/>";
$s .= "<a href='wapCalendar.php'>Соревнования</a><br/>";

$s .= "</p>";

$s .= '</card></wml>';

Motl::writeWap( $s );

//=====================
?>
