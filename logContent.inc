<?php
require_once "_core.inc";
//=====================

class LogLine {

  public $valid = 0;
  public $ip, $when, $method, $url, $http, $code, $size, $ref, $browser;

  function __construct( $s, $quick=false ) {
    $this->ip = strtok( $s, " " );
    $a2 = strtok( " " );
    $a3 = strtok( " " );

$quick=0;

    if( ! $quick ) {
      $date = substr( strtok( " " ), 1 );
      $this->timestamp = $this->parseLogDate( $date );
      $this->when = date( "d.m.Y H:i:s", $this->timestamp );

      $a5 = strtok( " " );
      $query = strtok( "\"" );
      $this->code = strtok( " " );
      $this->size = strtok( " " );
      $this->ref = strtok( "\"" );
      $this->browser = strtok( "" );

      list($this->method,$this->url,$this->http) = explode( " ", $query );
    }
    else {
      $this->timestamp = null;
    }

    $this->valid = 1;
  }

  //----------------

  private function parseLogDate( $s ) {
    $a = explode( ":", $s );
    $b = str_replace( "/", " ", $a[0] );
    $c = "$b $a[1]:$a[2]:$a[3]";
    return strtotime( $c );
//    $q = date( "d.m.Y H:i:s", $t );
  }

  //----------------

  public function important() {
    $url = $this->url;

    if( strstr( $url, "/favicon.ico" ) ) return 0;
    if( strstr( $url, "/tip.php" ) ) return 0;
    return 1;
  }

  //----------------
}

//=====================

class LogVisit {

  public $lines;
  public $ip, $minTS, $maxTS;
  public $nAll=0, $nImportant=0;

  //----------------

  function __construct( $logLine ) {
    $this->ip = $logLine->ip;
    $this->minTS = $this->maxTS = $logLine->timestamp;
    $this->add( $logLine );
  }

  //----------------

  function add( $logLine ) {
    if( $logLine->ip != $this->ip ) motlDie( "Wrong IP!" );
    $this->lines[] = $logLine;
    $this->maxTS = $logLine->timestamp;
    $this->nAll ++;
    if( $logLine->important() )
      $this->nImportant ++;
  }

  //----------------

  function getId() {
    return $this->minTS.".".$this->ip;
  }

  //----------------

  function toString() {
    return date( "d.m.Y H:i", $this->minTS ) ." -> ". $this->ip;
  }

  //----------------

  public function produce( $format = null ) {
    $arefs = array();
    for( $k=0; $k<count($this->lines); $k++ ) {
      $ref = $this->lines[$k]->ref;

      if( $ref != "-" 
          && substr($ref,0,25) != "http://www.danceleague.ru" 
          && substr($ref,0,21) != "http://danceleague.ru" 
        ) {
        $arefs[] = $ref;
      }
    }
    $refs = implode( "<br>", $arefs );

    $s = "";

    $s .= "<b>info</b>";
    $s .= "<table border=1>";
    $s .= "<tr><td> IP </td><td> $this->ip </td></tr>";
    $t = date( "d.m.Y H:i", $this->minTS );
    $s .= "<tr><td> Start </td><td> $t </td></tr>";
    $t = date( "d.m.Y H:i", $this->maxTS );
    $s .= "<tr><td> End </td><td> $t </td></tr>";
    $b = $this->lines[0]->browser;
    $s .= "<tr><td> Browser </td><td> $b </td></tr>";
    $s .= "<tr><td> Referrers </td><td> $refs </td></tr>";
    $s .= "</table>";

    $s .= "<p>";

    $s .= "<b>requests</b>";
    $s .= "<table border=1 class='MainText'>";

    $s .= "<tr style='font-weight:bold'>";
    $s .= "<td>when</td>";
    $s .= "<td>method</td>";
    $s .= "<td>url</td>";
    $s .= "<td>code</td>";
    $s .= "<td>size</td>";
//    $s .= "<td>ref</td>";
    $s .= "</tr>";

    for( $k=0; $k<count($this->lines); $k++ ) {
      $line = $this->lines[$k];
      if( ! $line->important() ) continue;

      $time = date( "H:i:s", $line->timestamp );

      $s .= "<tr>";
      $s .= "<td>$time</td>";
      $s .= "<td>$line->method</td>";
      $s .= "<td>$line->url</td>";
      $s .= "<td>$line->code</td>";
      $s .= "<td>$line->size</td>";
//      $s .= "<td>$line->ref</td>";
      $s .= "</tr>";
    }
    $s .= "</table>";

    return $s;
  }

  //----------------
}

//=====================

class LogContent extends Producer {

  //----------------

  public function getLogPath() {
    return getLocalSetting( 'LOG_PATH' );
  }

  //----------------

  public function produce( $format = null ) {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );
    $aid = Motl::splitId( $id );
//print_r($aid);

    if( !$id ) {
      return $this->produceDays();
    }

    if( $aid['c'] == 'd' ) {
      return $this->produceDayGroups( $aid[0] );
    }

    if( $aid['c'] == 'g' ) {
      return $this->produceGroup( $aid[0], $aid[1] );
    }

/*
    $all = file( $this->getLogPath() );

    $groups = array();
    $groupsByIP = array();

    for( $k=0; $k<1000; $k++ ) {
//    for( $k=0; $k<count($all); $k++ ) {
      $logLine = new LogLine( $all[$k] );

      $ip = $logLine->ip;

      $grp = isset($groupsByIP[$ip]) ? $groupsByIP[$ip] : null;

      if( $grp ) {
        $grp->add( $logLine );
      }
      else {
        $grp = new LogVisit( $logLine );
        $groups[] = $grp;
        $groupsByIP[$ip] = $grp;
      }
    }

    $s = "";

    if( ! $id ) {
      for( $k=0; $k<count($groups); $k++ ) {
        $idGrp = $groups[$k]->getId();
        $label = $groups[$k]->toString();
        $s .= "<A href='$script?id=$idGrp'>$label</a><br>";
      }
    }
    else {
      for( $k=0; $k<count($groups); $k++ ) {
        if( $groups[$k]->getId() == $id ) {
          $s .= $groups[$k]->produce();
        }
      }
    }
*/
    return $s;
  }

  //----------------

  private function produceDays() {
    $script = $_SERVER['PHP_SELF'];

    $all = file( $this->getLogPath() );

    $line0 = new LogLine( $all[0] );
    $line1 = new LogLine( $all[count($all)-1] );
    $day0 = date( "d.m.Y", $line0->timestamp );

    $ts0 = strtotime( $day0 );
    $day0a = date( "d.m.Y", $ts0 );

    $s = "";
    for( $ts=$ts0; $ts<$line1->timestamp; $ts+=86400 ) {
      $label = date( "d.m.Y", $ts );
      $s .= "<A href='$script?id=d$ts'>$label</a><br>";
    }
    return $s;
  }

  //----------------

  private function produceDayGroups( $ts0 ) {
    $script = $_SERVER['PHP_SELF'];

    $all = file( $this->getLogPath() );

    $k0 = $this->lineByTS( $all, $ts0 );
    $k1 = $this->lineByTS( $all, $ts0+86400 );

    $groups = array();
    $groupsByIP = array();

    for( $k=$k0; $k<$k1; $k++ ) {
      $logLine = new LogLine( $all[$k], true );

      $ip = $logLine->ip;

      $grp = isset($groupsByIP[$ip]) ? $groupsByIP[$ip] : null;

      if( $grp ) {
        $grp->add( $logLine );
      }
      else {
        $grp = new LogVisit( $logLine );
        $groups[] = $grp;
        $groupsByIP[$ip] = $grp;
      }
    }

    $s = "";

    $s .= "<h2>" .count($groups). " visits</h2>";

    $s .= "<table border=1 class='MainText'>";
      for( $k=0; $k<count($groups); $k++ ) {
        $grp = $groups[$k];
        $time = date( "H:i:s", $grp->minTS );
        $ip   = $grp->ip;
        $s .= "<tr> <td><A href='$script?id=g$grp->minTS,$grp->ip'>$time</a></td> <td>$ip</td> <td>$grp->nAll</td> <td>$grp->nImportant</td> </tr>";
      }
    $s .= "</table>";

    return $s;
  }

  //----------------

  private function produceGroup( $ts0, $ip ) {
    $all = file( $this->getLogPath() );

    $k0 = $this->lineByTS( $all, $ts0 );
    $k1 = $this->lineByTS( $all, $ts0+86400 );

    $grp = null;

    for( $k=$k0; $k<$k1; $k++ ) {
      $logLine = new LogLine( $all[$k], true );

      if( $logLine->ip != $ip ) continue;

      if( $grp ) {
        $grp->add( $logLine );
      }
      else {
        $grp = new LogVisit( $logLine );
      }
    }
    return $grp->produce();
  }

  //----------------

  private function lineByTS( $strings, $ts ) {
    $nlines = count($strings);
    $p0 = 0;
    $p1 = $nlines-1;

    $line0 = new LogLine( $strings[$p0] );   
    $ts0 = $line0->timestamp;

    $line1 = new LogLine( $strings[$p1] );   
    $ts1 = $line1->timestamp;

    for( $k=0; $k<20; $k++ ){
      $p = ($p1+$p0)>>1;

      $linep = new LogLine( $strings[$p] );   
      $tsp = $linep->timestamp;

      if( $ts < $tsp ) {
        if( $p1 == $p ) break; // stabilized
        $p1  = $p;
        $ts1 = $tsp;
      }
      else
      if( $ts > $tsp ) {
        if( $p0 == $p ) break; // stabilized
        $p0  = $p;
        $ts0 = $tsp;
      }
      else {
        return $p;
      }
    }

    return $p1;
  }

  //----------------
}

//=====================
//002184
?>
