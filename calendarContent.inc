<?php
require_once "_dbproducer.inc";
require_once "dbEvent.inc";
require_once "pageContent.inc";
//=====================

class Months {
  static $monthNames  = array( null, "январь", "февраль", "март", "апрель", "май", "июнь", "июль", "август", "сентябрь", "октябрь", "ноябрь", "декабрь" );
  static $monthNamesV = array( null, "январе", "феврале", "марте", "апреле", "мае", "июне", "июле", "августе", "сентябре", "октябре", "ноябре", "декабре" );
  
  public static function getCurYear() {
    return (int)date( "Y", time() );
  }
  
  public static function getCurMonth() {
    return (int)date( "m", time() );
  }
  
  public static function getNextMonth() {
    $m = Months::getCurMonth();
    //$m = 1;
    return (($m-1+1)%12)+1; 
  }
  
  public static function getCurYM() {
  //die( date( "Ym", time() ) );
    return date( "Ym", time() );
  }

  public static function getNextYM() {
    $m1 = Months::getNextMonth();
    $y = Months::getCurYear();
    if( $m1 == 1 ) {
      $y ++;
    }
    $m1 = substr( "00$m1", -2 );
    return "$y$m1";
  }
  
};

//=====================

class CalendarContent extends Producer {
  
//  var $seasonLabel = "2011/12 гг.";
//  var $seasonDays  = array( "2011-08-01", "2012-09-30" );
//  var $seasonLabel = "2012/13 гг.";
//  var $seasonDays  = array( "2012-08-01", "2013-09-30" );
  var $seasonLabel = "2013/14 гг.";
  var $seasonDays  = array( "2013-08-01", "2014-09-30" );
  
  public static $hold = true;
  
  var $free = false;
  
  //----------------
  
  function __construct( $free ) {
    $this->free = $free;
  }
  
  //----------------

  function produce() {

    $script = $_SERVER['PHP_SELF'];
  
    /*if( $this->free ) {
      $id = Motl::getParameter( "id", "curmonth" );
    }
    else {
      $id = Motl::getParameter( "id", "season" );
    }*/
	$id = Motl::getParameter( "id", "curmonth" );

    $s = "";
    $s .= "<h1>Календарь МОТЛ на сезон 2013/2014</h1>";
    
  
    if( $this->free ) { // tabs
      $tabs = new Tabs();
      $tabs->current = $id;
	  
      $tabs->addTab( "curmonth",   "Турниры МОТЛ в ".Months::$monthNamesV[Months::getCurMonth()],        
                                                                     "$script?id=curmonth"  );
                                                                     
      $tabs->addTab( "nextmonth",  "Турниры МОТЛ в ".Months::$monthNamesV[Months::getNextMonth()],        
                                                                     "$script?id=nextmonth" );
      $tabs->addTab( "events",     "Мероприятия МОТЛ в 2013/2014",  "$script?id=events" );
      
      $tabs->addTab( "season",     "Перспективный календарь на 2013/2014",           "$script?id=season" );
	  
                                                                     
      $s .= $tabs->produce();   
      $s .= "<p>";
    }
  
    switch( $id ) {
      case 'curmonth':
        $s .= $this->produceTable( Months::getCurYM(), false );
        break;
        
      case 'nextmonth':
        $s .= $this->produceTable( Months::getNextYM(), false );
        break;
        
      case 'events':
        $eventsPage = new PageContent();
        $eventsPage->id = "motlEvents";
        $eventsPage->loadFromDB( $eventsPage->id );
        //$eventsEditLink = $wapPage->makeEditLink("Edit");
        $s .= $eventsPage->produce();
        break;
        
      default:
      case 'season':
        $s .= $this->produceTable( null, true );
        break;
    }
    
    return $s;
  }
  
  //----------------
  
  function produceTable( $yearmonth=null, $full=false ) {
    $ct = $this->createTable( $yearmonth, $full );
    return $ct->produce();
  }
  
  //----------------
  
  function createTable( $yearmonth=null, $full=false ) {
  
    if( ! $yearmonth ) {
      $periodLabel = $this->seasonLabel;
      $periodRange = $this->rangeFromDays( $this->seasonDays );
    }
    else {
      $y = (int)substr( $yearmonth, 0,4 );
      $m = (int)substr( $yearmonth, 4 );
      $periodLabel = CalendarTable::$monthNames[$m] . " $y г."; 
      $periodRange = $this->monthRange( $yearmonth );
    }
    
    return new CalendarTable( $periodLabel, $periodRange, $full );
  }
  
  //----------------

  function rangeFromDays( $days ) {
    $t0 = strtotime( $days[0]." 00:00:00" );
    $t1 = strtotime( $days[1]." 23:59:59" )+1;
    return array( $t0, $t1 );
  }

  //----------------
  
  function monthRange( $yearmonth ) {
    $y = (int)substr( $yearmonth, 0,4 );
    $m = (int)substr( $yearmonth, 4 );
    $m1 = ($m<12) ? $m+1 : 1;
    $y1 = ($m<12) ? $y   : $y+1;
	
	if( $m==9 ) {
		$m = 8; // Хак, чтобы в сентябре показать августовское
	}
	
    $t0 = strtotime( "$y-$m-01 00:00:00" );
    $t1 = strtotime( "$y1-$m1-01 00:00:00" );
    return array( $t0, $t1 );
  }
  
  //----------------
}

class CalendarTable extends Producer {
  
  static $monthNames = array( null, "январь", "февраль", "март", "апрель", "май", "июнь", "июль", "август", "сентябрь", "октябрь", "ноябрь", "декабрь" );
  
  var $periodLabel = "";
  var $periodRange = array(); // включая первый, не включая последний // SQLPARAM
  
  //----------------

  function __construct( $periodLabel, $periodRange, $full=false ) {

    $this->periodLabel = $periodLabel;
    $this->periodRange = $periodRange;
    $this->full        = $full;
  }
  
  //----------------

  function rangeToSQL( $range ) {
    return array( date( "Y-m-d H:i:s", $range[0] ),
                  date( "Y-m-d H:i:s", $range[1] )
                );
  }

  //----------------
  // SQLPARAM $cond
  function loadEvents( $cond = null ) {
    $editmode = Motl::getParameter( "editmode" ) ? 1 : 0;
  
    list($d0,$d1) = $this->rangeToSQL($this->periodRange);

    $andCond = $cond ? "AND $cond" : "";
    
    // COD $editmode
    // COD $d0,$d1  - безопасно получаются в rangeToSQL()
    // PAR $andCond ($cond)
    $sql = "SELECT db_events.*, db_halls.title as placeTitle, "
                 ."($editmode OR regOpen OR haveScores OR havePlaces) AS NotEmpty "
          ."FROM db_events LEFT JOIN db_halls ON db_events.idPlace=db_halls.id "
          ."WHERE type='event' AND (date>='$d0' OR date1>='$d0') AND date<'$d1' $andCond "
          ."ORDER BY date, position";
    
//    die($sql);
    
    return MotlDB::queryRows( $sql );  
  }
  
  //----------------
  
  function produce() {
    $script = $_SERVER['PHP_SELF'];

 //echo $this->produce;
    $s = "";
    
//    $moreCond = $this->full ? "" : "(organizer like '%МОТЛ%' or db_events.id in(556,559))";
    $moreCond = $this->full ? "" : "(db_events.id NOT in(2313))";
 
    $events = $this->loadEvents( $moreCond ); // COD $moreCond

    
    $s .= "<table border=1 cellspacing=0 cellpadding=5 class=Calendar>";

    
    $s .= "<tr class=CalendarTop> <td>Дата</td>"
                    ."<td>Название турнира<br>Статус турнира</td>"
                    ."<td>Место проведения</td>"
                    ."<td>Организатор</td>"
               ."</tr>";

    $template = "<tr> <td>[editbar|edit,copy,del,medit| ][plainDateRange|date|date1]</td>"
                    ."<td>[if|NotEmpty|<A href='db.php?id=e[plain|id]'>]<i>[plain|title]</i>[if|NotEmpty|</a>]<br>[plain|status]</td>"
                    ."<td>[if|idPlace|<A href='db.php?id=h[plain|idPlace]'>[plain|placeTitle]</a>|[plain|placeLabel]]&nbsp;</td>"
                    ."<td>[plainBB|organizer]&nbsp;</td>"
               ."</tr>";
    
    foreach( $events as $row ) {
      $prototype = new DbEventProducer($row);
//      $prototype = new DbProducer($row);
      $s .= $prototype->makeText( $row, $template );
    }
    $s .= "</table>";
    
    return $s;
  }
  
  //----------------
  //----------------
  
  function produceWAP() {
  
    $s = "";
    $s .= "<p><big>График турниров МОТЛ на $this->periodLabel</big><br/></p>\n";
    
    $events = $this->loadEvents();
    
    $template = "<p>[if|NotEmpty|<A href='db.php?id=e[plain|id]'>]"
                    ."[plainDate|date] [plain|title]"
                    ."[if|NotEmpty|</a>]</p>\n";
    
    $template = "<p><b>[plainDate|date]</b> [plain|title]<br/>"
                    ."[if|regOpen|<a href='wapReg.php?id=e[plain|id]'>регистрация</a><br>]\n"
                    ."[if|haveScores,havePlaces|<a href='wapRes?id=e[plain|id]'>результаты</a><br>]</p>\n";

    foreach( $events as $row ) {
      $prototype = new DbEventProducer($row);
//      $prototype = new DbProducer($row);
      $line = $prototype->makeText( $row, $template );
      $line = str_replace( "<br>", "<br/>", $line );
      $line = str_replace( "<A", "<a", $line );
      $s .= $line;
    }
    
    return $s;
  }
  
  //----------------
}
?>
