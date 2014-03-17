<?php
//=====================

$_coreSetupPreviousVisitTime_prevVisitGlobal = null;

function _coreSetupPreviousVisitTime() {
  $threshold = 30000;

  $theCurrentTime = time();
  $prevVisit = isset($_COOKIE["prevVisit"]) ? $_COOKIE["prevVisit"] : 0;
  $lastHit   = isset($_COOKIE["lastHit"]) ? $_COOKIE["lastHit"] : 0;

  if( $theCurrentTime - $lastHit > $threshold ) {
    $prevVisit = $lastHit;
  }
  
  $lastHit = $theCurrentTime;
  
  if( ! $prevVisit ) {
    $prevVisit = $theCurrentTime;
  }

  //$expire = time()+60*60*24*365;
  $expire = time()+60*30;
  setcookie( "prevVisit",  "$prevVisit",  $expire );
  setcookie( "lastHit",    "$lastHit",    $expire );
  
global $_coreSetupPreviousVisitTime_prevVisitGlobal;
  $_coreSetupPreviousVisitTime_prevVisitGlobal = $prevVisit;
}
_coreSetupPreviousVisitTime();

function getPrevVisitTime() {
global $_coreSetupPreviousVisitTime_prevVisitGlobal;
  return $_coreSetupPreviousVisitTime_prevVisitGlobal;
}

//=====================

function Q( $obj ) {
  echo "<!--\n";
  print_r( $obj );
  echo "\n-->";
}

//---------------------

function dumpString( $s ) {
	echo "<table border=1 cellspacing=0>";
	echo "<tr><td colspan=3>$s</td></tr>";
	for( $k=0; $k<mb_strlen($s,"utf-8"); $k++ ) {
		$ch = mb_substr( $s, $k, 1, "utf-8" );
		$ord = ord($ch);
		echo "<tr><td>$k.</td><td>$ord</td><td>$ch</td></tr>";
	}
	echo "</table>";
}

//---------------------

function escapeJsString( $s ) {
  $s = str_replace( "\\", "\\x5C", $s );
  $s = str_replace( "=", "\\x3D", $s );
  $s = str_replace( " ", "\\x20", $s );
  $s = str_replace( "\t", "\\x09", $s );
  $s = str_replace( "\n", "\\n", $s );
  $s = str_replace( "\r", "\\r", $s );
  $s = str_replace( "\"", "\\x22", $s );
  $s = str_replace( "'", "\\x27", $s );

  $s = str_replace( "<", "\\x3C", $s );
  $s = str_replace( "&", "\\x26", $s );
  $s = str_replace( ">", "\\x3E", $s );

  return $s;
}

//---------------------

function unescape( $s ) {
  $s1 = "";
  for( $k=0; $k<strlen($s); ) {
    if( substr($s,$k,2) == "%u" ) {
      $utf16 = rawurldecode( "%".substr($s,$k+2,2) ."%". substr($s,$k+4,2) );
      $utf8 = mb_convert_encoding( $utf16, "UTF-8", "UTF-16" );
      $s1 .= $utf8;
      $k+=6;
    }
    else
    if( substr($s,$k,1) == "%" ) {
      $ascii = rawurldecode( substr($s,$k,3) );
      $s1 .= $ascii;
      $k+=3;
    }
    else {
      $s1 .= substr($s,$k,1);
      $k ++;
    }
  }
  return $s1;
}

//---------------------

function motlDie( $message ) {
  global $motlErrorHandler;
  
  if( $motlErrorHandler ) {
    $motlErrorHandler( $message );
  }
  die( $message );
}

//---------------------

$motlErrorHandler = null;

function motlASSERT( $value, $cond ) {
  if( ! $cond ) {
    motlDie( "ERROR: Assertion failed ($value)" );
  }
}

//---------------------

function ASSERT_null( $s ) {
  motlASSERT( $s, is_null($s) );
}

//---------------------

function ASSERT_int( $s ) {
  motlASSERT( $s, ctype_digit("$s") || strlen("$s")==0 );
}

//---------------------

function ASSERT_hex( $s ) {
  motlASSERT( $s, ctype_xdigit("$s") || strlen("$s")==0 );
}

//---------------------

function ASSERT_alnum( $s ) {
  motlASSERT( $s, ctype_alnum("$s") || strlen("$s")==0 );
}

//---------------------

function ASSERT_ints( $s ) {
  $s = str_replace( ",", "", $s );
  ASSERT_int( $s );
}

//---------------------

function ASSERT_subpath( $s ) {
  $s = str_replace( "/", "", $s );
  $s = str_replace( "_", "", $s );
  ASSERT_alnum( $s );
}

//---------------------

function ASSERT_name( $s ) {
  $s = str_replace( "_", "", $s );
  motlASSERT( $s, ctype_alpha(substr($s,0,1)) );
  ASSERT_alnum( $s );
}

//---------------------


//=====================

class Producer {

  public function produce() {
  }

  public function useProducer( $p ) {
    if( ! $p ) return "";
    if( $p instanceof Producer ) return $p->produce();
    return (string)$p;
  }
}

//=====================

class Layout extends Producer {

  public function write($format=null) {
    header( "Content-Type: text/html; charset=utf-8" );
	header( "Expires: Tue, 01 Jun 1999 09:17:32 GMT" );
    echo $this->produce($format);
  }

}

//=====================

class VBox extends Producer {
  public $items = null;

  function __construct( $items=null ) {
    $this->items = $items;
  }

  public function produce() {
    if( ! $this->items )
      return "";

    $s = "";

    for( $k=0; $k<count($this->items); $k++ ) {
      $s .= $this->useProducer( $this->items[$k] );
  //    $s .= $this->items[$k]->produce();
    }

    return $s;
  }
}

//=====================

class Motl {
  public static $Rubric;
  public static $db;

  public static $layout;
  public static $format;

  public static $script;
  public static $id;

  //===============

  public static function makeRetUrl() {
    return urlencode( $_SERVER["REQUEST_URI"] );
  }
  
  public static function makeRetUrlPost() {
    return Motl::htmlEscape( $_SERVER["REQUEST_URI"] );
  }

  //===============

  public static function htmlEscape( $s ) {
    return htmlspecialchars( $s, ENT_QUOTES );
  }

  //===============

  public static function redirectToRetUrl() {
  
    $returl = Motl::getParameter( "returl" );
    //die("($returl)");
    if( ! $returl ) {
      die( "Error: No 'returl'" );
    }
    
    Motl::redirect( $returl );
  }

  //===============

  public static function getScript() {
    return $_SERVER['PHP_SELF'];
  }

  //===============

  public static function splitId( $id ) {

    $a = explode( ",", $id );

    if( $a[0] && ctype_alpha($a[0][0]) && isset($a[0][1]) && !ctype_alpha($a[0][1]) ) {
      $a['c'] = $a[0][0];
      $a[0] = substr( $a[0], 1 );
    }
    else {
      $a['c'] = null;
    }

    return $a;
  }

  //===============

  public static function getScriptsFolder() {
    $self = $_SERVER["PHP_SELF"];
    $dir = dirname($self);
    return ($dir=="/") ? "" : $dir;
  }

  public static function getScriptsFolderUrl() {
    $dir = Motl::getScriptsFolder();
    if( $dir[0] != '/' ) $dir = '/'.$dir;
    return "http://".$_SERVER['SERVER_NAME'].$dir;
  }

  public static function getFilesystemScriptsFolder() {
    $self = $_SERVER["SCRIPT_FILENAME"];
    return dirname($self);
  }

  //---------------

  public static function getPicturesFolder() {
    return  Motl::getScriptsFolder() . "/pictures";
  }


  public static function getFilesystemPicturesFolder() {
    return  Motl::getFilesystemScriptsFolder() . "/pictures";
  }

  //---------------

  public static function getSysPicturesFolder() {
    return  Motl::getScriptsFolder() . "/syspictures";
  }


  //===============

  public static function getIntParameter($name,$default=null) {
    $value = Motl::getParameter($name,$default);
    ASSERT_int($value);
    return $value;
  }
  
  //===============

  public static function getParameter($name,$default=null) {

    if( $name == "editmode" ) {
      if( ! User::canEdit() ) {
        return 0;
      }
    }

    if( isset( $_REQUEST[$name] ) ) {
		if( is_array( $_REQUEST[$name] ) ) {
			return $_REQUEST[$name];
		}
		return strlen($_REQUEST[$name]) ? $_REQUEST[$name] 
										: $default;
	}
	
    if( isset( $_COOKIE[$name] ) ) {
		if( is_array( $_COOKIE[$name] ) ) {
			return $_COOKIE[$name];
		}
		return strlen($_COOKIE[$name]) ? $_COOKIE[$name] 
										: $default;
	}
	
	return $default;
  }

  //===============

  public static function redirect( $url ) {
    header( "HTTP/1.1 302 Redirect" );
    header( "Location: ".$url );
    die();
  }

  //===============
  
  public static function writeWap( $docText ) {
    $isMozilla = strstr( $_SERVER['HTTP_USER_AGENT'], "Mozilla" ) ? 1 : 0;
    $isMobile = ! $isMozilla;
  
    if( $isMobile ) {
      header( "Content-Type: text/vnd.wap.wml" );
      echo '<?xml version="1.0" encoding="UTF-8"?>'; 
      echo '<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.1//EN" "http://www.wapforum.org/DTD/wml_1.1.xml" >';
    }
    else {
      header( "Content-Type: text/html; charset=utf-8" );
    }
    echo $docText;
  }
  
  //===============
}

//=====================

?>
