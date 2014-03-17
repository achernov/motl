<?php
require_once "_core.inc";
//=====================

class ChatContent extends Producer {

  //----------------

  public function produce() {

    if( ! User::isPresidium() ) {
      return "";
    }
  
    $s = "";

    $s .= "<h1>Чат МОТЛ</h1>";
    
    $s .= "<script src='ajax.js'></script>";
    $s .= "<script src='chat.js'></script>";
    
    $s .= "<b>Присутствуют: </b><font id='chatWho' class='Nick'></font><p>";
    
    $s .= "<div id='chatScroller' style='overflow:auto; border:1px solid black; width:100%;height:400'>";
    $s .= "<div id='chatWindow'></div><p>";
    $s .= "</div>";
    
    $user = User::getCurrentUser();
    if( $user->row->login != "marylove" ) {
      $s .= "<b>Ваша реплика:</b><br>";
      $s .= "<textarea id='chatMessage' style='width:100%;height:50'></textarea>";
      $s .= "<input type='button' value='Отправить' onclick='chatSay()'><p>";
    }
    
    return $s;
  }

  //----------------
}

//=====================
?>
