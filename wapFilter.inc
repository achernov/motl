<?php
//=====================

function charAt( $str, $pos ) {
  return mb_substr( $str, $pos,1, "utf-8" );
}

function instr( $ch, $str ) {
  return is_int( mb_strpos( $str, $ch, 0, "utf-8" ) );
}

function isspace($ch) {
  return instr( $ch, " \t\r\n" );
}

function isalpha($ch) {
  return instr( $ch, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
}

function isalnum($ch) {
  return isalpha($ch) || instr( $ch, "0123456789" );
}

//================

class WapFilter {

  var $text;
  var $curPos;

  //================
  
  public function filterWap( $text ) {
  //set_time_limit(1);
  //echo "$text<hr>";
  
    //$this->filterTags( $text ); die('sss');
    
    $text = str_replace( "<A ", "<a ", $text );
    $text = str_replace( "<br>", "<br/>", $text );
    return $text;
  }
  
  //================
  
  public function filterTags( $text ) {
    $this->text = $text;
    $this->curPos = 0;
    $this->len = mb_strlen( $this->text, "utf-8" );
    
    $s = "";
    
    for( $cnt=0; $cnt<10; $cnt++ ) {
      $chop = $this->chopPlain();
      if( is_null($chop) ) break;
      $s .= $chop;
echo "<b>plain:</b> $chop<br>";

      $chop = $this->chopTag();
      if( is_null($chop) ) break;
      $s .= $chop;
echo "<b>tag:</b> $chop<br>";
    }
/*    
    $nextPod = 
    echo "$pos $text";
  */  
    return $s;
  }
  
  //================
  
  private function chopPlain() {
    $tagPos = mb_strpos( $this->text, "<", $this->curPos, "utf-8" );
    $plain = mb_substr( $this->text, $this->curPos,$tagPos-$this->curPos, "utf-8" );
    $this->curPos = $tagPos;
    //$tag = mb_substr( $this->text, $tagPos,1000, "utf-8" );
    //echo "$tagPos  $tag<p>\n";
    //echo "$tagPos  $this->text<p>\n";
    //echo "$tagPos($plain)<p>\n";
    return $plain;
  }

  //================
  
  private function skipSpace() {
    while( isspace(charAt($this->text,$this->curPos)) ) $this->curPos++;
  }
  
  //================
  
  private function readUntil( $s) {
    $res = "";
    while( !instr(charAt($this->text,$this->curPos),$s) ) {
      $res .= charAt($this->text,$this->curPos);
      $this->curPos++;
    }
    return $res;
  }
  
  //================
  
  private function chopTag() {
   
    // get tag name
    $tagName = "";
    for( $this->curPos++; $this->curPos<$this->len; ) {
      $ch = charAt( $this->text, $this->curPos );
      if( $ch=='>' || isspace($ch) ) {
        break;
      }
      else {
        $tagName .= $ch;
        $this->curPos++;
      }
    }
    
    if( $ch != '>' ) { // Еще не конец, обрабоатываем внутренности
      $this->skipSpace();
      $s = $this->readUntil(">= \t\r\n");
      die("($s)");
    }
    
    echo "-$tagName-$ch<br>";
    return "";
  }

  //================
}

//=====================
?>
