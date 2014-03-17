<?php
require_once "_motl_user.inc";
//=====================

class LoginForm extends Producer {

  //---------------------

  private $errorMessages = array(
      "nouser"       => "Нет такого пользователя",
      "badpass"      => "Неправильный пароль",
      "notactivated" => "Пользователь не активирован",
      "banned"       => "Пользователь забанен",
    );

  //---------------------

  function __construct( $format = null ) {
  }

  //---------------------

  public function produce() {
  
    $user = User::getCurrentUser();

    if( $user ) {

      return "\n". $this->makeUserForm( $user );

    }
    else {

      if( Motl::$Rubric != "RegLogin" ) {

        $returl = Motl::makeRetUrl();
        
        $loginlink = "<a href='loginPage.php?returl=$returl'>Регистрация и Вход</a>";

        return "\n<div class='RegLogin'>$loginlink</div>";
      }

    }

    return "";
  }

  //---------------------

  public function checkLoginErrors() {
//print_r($_COOKIE);
    if( isset( $_COOKIE["motlLoginError"] ) ) {
      $error = $_COOKIE["motlLoginError"];

      setcookie( "motlLoginError", null ); // Don't show again

      if( isset($this->errorMessages[$error]) ) {
        $message = $this->errorMessages[$error];

        return "<script>setTimeout( \"alert('$message')\", 100 )</script>";
      }
    }

    return "";
  }

  //---------------------

  function makeLoginForm( $template = null ) {

    $templator = new LoginTemplator();

    $templator->tags['FieldUser']     = 
        "<input class='LoginInput' type='text' name='lgnuser'>";

    $templator->tags['FieldPassword'] = 
        "<input class='LoginInput' type='password' name='lgnpass'>";

    $templator->tags['CheckRemember'] = 
        "<input style='margin-left:-4' type='checkbox' id='chkRemember' name='remember'>";

    $templator->tags['Button'] = 
        "<input class='LoginButton' type='submit' value='Войти'>";

    if( ! $template ) {
      $template = "<form name='LoginF1' class='LoginForm' onsubmit='DoLoginSubmit();return false;' action='ERROR'>"
         ."<table>\n"
         ."<tr><td class='LoginLabel'>Логин:</td><td>[FieldUser]</td></tr>\n"
         ."<tr><td class='LoginLabel'>Пароль:</td><td>[FieldPassword]</td></tr>\n"
         ."<tr><td colspan=2>"
            ."<table style='margin-bottom:4' border=0 cellpadding=0 cellspacing=0 width=100%><tr><td> "
               ."<nobr class='LoginLabel'>[CheckRemember]<font onclick='document.all.chkRemember.checked=!document.all.chkRemember.checked'> запомнить</font></nobr> </td>"
            ."<td align=right> [Button] </td></tr></table>"
            ."</td></tr>\n"
         ."</table>\n"
         ."</form>\n";
    }

    $s = $this->checkLoginErrors();

    return $s . $templator->process( $template ) . $this->makeLoginSupport();
  }

  //---------------------

  function makeLoginSupport() {

    $returl = Motl::getParameter( "returl" );
    $returlEscaped = Motl::htmlEscape( $returl );
    
    $s = "";
    //-------
    $s .= "<form name='LoginF2' action='login.php'>";
    $s .= "<input type='hidden' name='returl' value='$returlEscaped'>\n";
    $s .= "<input type='hidden' name='lgnuser'>\n";
    $s .= "<input type='hidden' name='lgnpassHash'>\n";
    $s .= "<input type='hidden' name='remember'>\n";
    $s .= "</form>\n";
    //-------
    $s .= "<script src='md5.js'></script>\n";
    $s .= "<script>\n";
    $s .= "function DoLoginSubmit() {\n";

    $s .= "document.forms['LoginF2'].elements['lgnuser'].value   = document.forms['LoginF1'].elements['lgnuser'].value\n";
    $s .= "document.forms['LoginF2'].elements['lgnpassHash'].value   = MD5( document.forms['LoginF1'].elements['lgnpass'].value )\n";
    $s .= "document.forms['LoginF2'].elements['remember'].value   = (document.forms['LoginF1'].elements['remember'].checked?1:0)\n";
    $s .= "document.forms['LoginF2'].submit();\n";
    $s .= "}\n";
    $s .= "</script>\n";
    //-------
    return $s;
  }

  //---------------------

  function makeUserForm( $user ) {
  
    $returl = Motl::makeRetUrl();
    
    $uid   = $user->id;
    $login = $user->getName();

    $s = "";

    $s .= "<div class='RegLogin'>";
    $s .= "Вы вошли как: <A class='Nick' href=profile.php?id=$uid>$login</a><br>\n";
    $s .= "<A href='logout.php?returl=$returl'>Выйти</a>\n";
    $s .= "</div>";

    return $s;
  }

  //---------------------
}

//=====================

class LoginTemplator extends Templator {

  public $tags = array();

  //===========

  public function processTag( $tagParts ) {
    $tagName = $tagParts[0];

    if( isset( $this->tags[$tagName] ) )
      return $this->tags[$tagName];
  }
}

//=====================

?>
