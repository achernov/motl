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

if( ! Motl::getParameter("submitted") )  {

  $s .= "<p>";

  $s .= "Фамилия партнера: <input name='msurname' />";
  $s .= "Имя партнера: <input name='mname' />";
  $s .= "Фамилия партнерши: <input name='fsurname' />";
  $s .= "Имя партнерши: <input name='fname' />";

  $s .= "Класс: <select name='class' >
  <option value='H'>H</option>
  <option value='E'>E</option>
  <option value='D'>D</option>
  <option value='C'>C</option>
  <option value='B'>B</option>
  <option value='A'>A</option>
  <option value='S'>S</option>
  <option value='M'>M</option>
  </select>";

  $s .= "Программы: <select name='program' multiple='true' >
  <option value='C'>C 16 и ст., St</option>
  <option value='C'>C 16 и ст., La</option>
  <option value='C'>A+B 16 и ст., St</option>
  <option value='C'>A+B 16 и ст., La</option>
  <option value='C'>Взрослые, St</option>
  <option value='C'>Взрослые, La</option>
  </select>";

  $s .= "</p>";

  $s .= '<do type="accept">
  <go href="wapReg.php" method="post">
  <postfield name="submitted" value="1" />
  <postfield name="msurname" value="$msurname" />
  <postfield name="mname" value="$mname" />
  <postfield name="fsurname" value="$fsurname" />
  <postfield name="fname" value="$fname" />
  <postfield name="class" value="$class" />
  </go>
  </do>';
}
else {
$msurname = Motl::getParameter( "msurname" );
$mname    = Motl::getParameter( "mname" );
$fsurname = Motl::getParameter( "fsurname" );
$fname    = Motl::getParameter( "fname" );
$class    = Motl::getParameter( "class" );

  $s .= "<p>msurname=$msurname<br/>
mname=$mname<br/>
fsurname=$fsurname<br/>
fname=$fname<br/>
class=$class<br/></p>";
}

//MotlDB::connect();
//$content = new CalendarContent( "2010009" );
//$content = new CalendarContent( );
//$s .= $content->produceWAP();

//$s .= "<p><table columns='3'><tr><td>1</td><td>2</td><td>3</td></tr></table></p>";
$s .= '</card>
</wml>';
Motl::writeWap( $s );

//=====================
?>
