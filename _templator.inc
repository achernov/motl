<?php
//=====================

class Templator {

  //================

  function subdivide( $array, $separator ) {
    $out = array();
  
    for( $i=0; $i<count($array); $i++ ) {
  
      $a = explode( $separator, $array[$i] );
  
      for( $j=0; $j<count($a); $j++ ) {
        if( $j > 0 ) $out[] = $separator;
        if( strlen($a[$j]) > 0 ) $out[] = $a[$j];
      }
    }
    return $out;
  }

  //----------------

  function tokenizeTemplate( $str ) {
    $a = array($str);
    $a = $this->subdivide( $a, "[" );
    $a = $this->subdivide( $a, "]" );
    $a = $this->subdivide( $a, "|" );
    return $a;
  }

  //----------------

  function processLevel( $array, $pos ) {  // returns [text,nextpos] or [args[0],huge] on finish

    $args = array();
    $curArg = 0;
    for( $p=(int)$pos; $p<count($array); ) {
  
      $a = $array[$p];
  
      switch( $a ) {
        case '[':
          $res = $this->processLevel( $array, $p+1 );
          $args[$curArg] = isset($args[$curArg]) ? $args[$curArg].$res[0] : $res[0];
          $p = $res[1];
          break;
  
        case ']':
          return array( $this->processTag($args), $p+1 );
          break;
  
        case '|':
          $p++;
          $curArg ++;
          break;
  
        default:
          $p++;
          $args[$curArg] = isset($args[$curArg]) ? $args[$curArg].$a : $a;
          break;
      }
    }
    return array($args[0],100000);
  }

  //----------------

  public function process( $template ) {

    if( strlen($template) <= 0 ) return "";

    $arr = $this->tokenizeTemplate( $template );

    $res = $this->processLevel( $arr, 0 );
    return $res[0];
  }

  //================

  public function processTag( $tagParts ) {
    $tagName = $tagParts[0];
    $handlerName = "processTag_".$tagName;

    if( method_exists( $this, $handlerName ) )
      return $this->$handlerName( $tagParts );

    return "";
  }

  //================
}

//=====================
?>