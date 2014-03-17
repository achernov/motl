<?php
//=====================
require_once "__locals.inc";
//=====================

define( "DEBUG_DB", 0 );

define( "DEBUG_query", 1 );
define( "DEBUG_queryRow", 1 );
define( "DEBUG_queryRows", 1 );
define( "DEBUG_queryValue", 1 );
define( "DEBUG_queryValues", 1 );
define( "DEBUG_saveRow", 1 );

//=====================
// Просто для краткости

function MRES( $value ) { return mysql_real_escape_string($value); }

//=====================

function implodeStringsWithMRES( $array ) { // returns Safe SQL
  $out = array();
  foreach( $array as $item ) {
    $out[] = MRES($item);
  }
  return "'" . implode( "','", $out ) . "'";
}
  
//=====================

class MotlDB {
  public static $db;

  //----------------

  public static function isConnected() {
    return MotlDB::$db ? 1 : 0;
  }
  
  //----------------

  public static function connect() {
    $dbName = getLocalSetting('DB_NAME');
    $dbUser = getLocalSetting('DB_USER');
    $dbPass = getLocalSetting('DB_PASS');

    MotlDB::$db = mysql_connect('localhost:3306', $dbUser, $dbPass );

    mysql_set_charset( 'utf8', MotlDB::$db );
    mysql_select_db( $dbName );
	
    global $motlErrorHandler;
	$motlErrorHandler = "MotlDB::logStateToDB";
	$motlErrorHandler = "logStateToDB";
  }

  //----------------

  public static function query( $sql ) {
    if( DEBUG_DB && DEBUG_query ) echo "\n<!--query\n$sql\n-->\n";
    
    $result = mysql_query( $sql, MotlDB::$db );
    return $result;
  }

  //----------------

  public static function queryRow( $sql ) {
    if( DEBUG_DB && DEBUG_queryRow ) echo "\n<!--queryRow\n$sql\n-->\n";
    
    $result = mysql_query( $sql, MotlDB::$db );
    if( ! $result ) return null;
    return mysql_fetch_object($result);
  }

  //----------------

  public static function queryRows( $sql ) {
    if( DEBUG_DB && DEBUG_queryRows ) { echo "\n<!--queryRows\n$sql\n-->\n"; flush(); }
    
    $rows = array();
    $result = mysql_query( $sql, MotlDB::$db );
    if( ! $result ) return $rows;
    
    while( $row = mysql_fetch_object($result) ) {
      $rows[] = $row;
    }
    return $rows;
  }

  //----------------

  public static function queryRowsIndexed( $indexBy, $sql ) {
    if( DEBUG_DB && DEBUG_queryRows ) echo "\n<!--queryRowsIndexed\n$sql\n-->\n";
    
    $rows = array();
    $result = mysql_query( $sql, MotlDB::$db );
    if( ! $result ) return $rows;
    
    while( $row = mysql_fetch_object($result) ) {
      $rows[$row->$indexBy] = $row;
    }
    return $rows;
  }

  //----------------

  public static function queryValue( $sql, $defaultValue=null ) {
    if( DEBUG_DB && DEBUG_queryValue ) echo "\n<!--queryValue\n$sql\n-->\n";
    
    $result = mysql_query( $sql, MotlDB::$db );
    if( ! $result ) return $defaultValue;
    $array = mysql_fetch_row( $result );
    return isset($array[0]) ? $array[0] : $defaultValue;
  }

  //----------------

  public static function queryValues( $sql ) {
    if( DEBUG_DB && DEBUG_queryValues ) echo "\n<!--queryValues\n$sql\n-->\n";
    
    $values = array();
    $result = mysql_query( $sql, MotlDB::$db );
    if( ! $result ) return $values;
    
    while( $array = mysql_fetch_row($result) ) {
      $values[] = $array[0];
    }
    return $values;
  }

  //----------------

  public static function saveRow( $table, $idName,$idValue, $fields, $checkDups=false, $quiet=false ) {

    if( strlen($idValue) )
      $row = MotlDB::queryRow( "SELECT $idName FROM $table WHERE $idName='$idValue'" );
    else
      $row = null;

    $asets   = array();
    $anames  = array();
    $avalues = array();

    if( $idValue != null ) {
      $anames[]  = $idName;
      $avalues[] = "'$idValue'";
    }

    while( list($key, $val) = each($fields) ) {
//echo "$key ".(is_null($val)?1:0)."<br>";

       if( is_null( $val ) ) {
         $v = "null";
       }
       else {
	   //Q($v);
         $v0 = mysql_real_escape_string( $val, MotlDB::$db );
         $v = "'$v0'";
       }

       $asets[] = "$key=$v";
       $anames[] = "$key";
       $avalues[] = $v;
    }

    $conds  = implode( " and ", $asets );
    $sets   = implode( ",", $asets );
    $names  = implode( ",", $anames );
    $values = implode( ",", $avalues );

    if( $row ) {
      $sql = "UPDATE $table SET $sets WHERE $idName='$idValue'";
//Q($sql);
      $saved = MotlDB::query( $sql );
      if( !$saved && !$quiet ) motlDie( $sql );
      return $idValue;
    }
    else {
      if( $checkDups ) {
        $old = MotlDB::queryValue( "SELECT id FROM $table WHERE $conds", null );
//        Q("$old = SELECT id FROM $table WHERE $conds");
        if( !is_null($old) ) return $old;
      }
      $sql = "INSERT INTO $table($names) VALUES($values)";
//Q($sql);
      $saved = MotlDB::query( $sql );
      if( !$saved && !$quiet ) motlDie( $sql );
      return $idValue ? $idValue : mysql_insert_id( MotlDB::$db );
    }

  }

  //----------------

}

//=====================
  
  function logStateToDB( $message ) {
    static $cycle = false;
    if($cycle) die("cycle($message)");
    $cycle = true;
      $fields = array();
  	
  	$fields[ "errorTime" ] = date( "Y-m-d H:i:s", time() );
  	$fields[ "message" ]   = $message;
  	
  	$backTrace = myBackTrace();
  	
  	$fields[ "file" ] = basename( $backTrace[0]['file'] );
  	$fields[ "line" ] = $backTrace[0]['line'];
  	$fields[ "callStack" ] = valueToString( $backTrace );
  	
  	$fields[ "cookies" ]  = valueToString( $_COOKIE );
  	$fields[ "getVars" ]  = valueToString( $_GET );
  	$fields[ "postVars" ] = valueToString( $_POST );
    
    MotlDB::saveRow( "errors", "id",null, $fields );
  $cycle = false;
  }

  //----------------
  
  function myBackTrace() {
    $raw = debug_backtrace();
	$out = array();

    // Skip internal levels
    for( $k=count($raw)-1; $k>=0; $k-- ) {
	  $function = $raw[$k]['function'];
	  if( $function == 'logStateToDB' ) break;
	  if( $function == 'motlASSERT' ) break;
	  if( substr($function,0,7) == 'ASSERT_' ) break;
	}
	
	// Output the rest
    for( ; $k<count($raw); $k++ ) {
	  $out[] = $raw[$k];
	}
	return $out;
  }
  
  //----------------
  
  function valueToString( $obj ) {
    ob_start();
	print_r( $obj );
	$s = ob_get_contents();
	ob_end_clean();
	return $s;
  }

  //=====================

?>
