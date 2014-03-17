<?php
require_once "_core.inc";
require_once "_dbproducer.inc";
require_once "_layout.inc";
require_once "captcha.inc";
//=====================
	
class RestorationFormProducer extends Producer {

  private $codeLen = 4;

  //=================

  public function produce() {

    $s = "<h1>Восстановление пароля:</h1>";

    //-------------------
  
    if( Motl::getParameter("processStart") ) {
      $s .= $this->processStart();
    }
    else
    if( Motl::getParameter("change") ) {
      $s .= $this->produceChangeForm();
    }
    else
    if( Motl::getParameter("processChange") ) {
      $s .= $this->processChange();
    }
    else {
      $s .= $this->produceStartPage();
    }
    
    //-------------------

    return $s;
  }

  //=================
  
  function produceStartPage() {
  
    $s = "";
    
    $s .= "Мы не можем выслать Вам Ваш пароль, так как из соображений безопасности он не хранится на сервере.<br>"
         ."Вместо этого на указанный при регистрации адрес будет выслана ссылка, перейдя по которой можно заново установить пароль.<p>";

    $s .= $this->produceStartForm();
    
    return $s;
  }
  
  //=================
  
  function produceStartForm() {
  
    $s = "";
    
    $s .= "<form action='registrationRestore.php'>";
    $s .= "<input type=hidden name=processStart value=1>";
    /*$s .= "<table class='MainText'>"
          ."<tr> <td>Введите логин</td> <td><input type=text name=name></td> </tr>"
          ."<tr> <td>или e-mail</td> <td><input type=text name=mail></td> </tr>"
          ."<tr> <td colspan=2><input type=submit value='Продолжить'></td> </tr>"
          ."</table>";*/
          
    $s .= "Введите e-mail, который Вы указывали при регистрациии:<br><input size=100 type=text name=mail><br>";
    $s .= "<input type=submit value='Продолжить'>";

    $s .= "</form>";
    
    return $s;
  }
  
  //=================

  function processStart() {
  
    $uid   = Motl::getParameter("uid");
    $login = Motl::getParameter("name");
    $email = Motl::getParameter("mail");
    
    if( !$uid && !$login && !$email ) {
      return $this->produceStartPage();
    }
  
    $rows = null;
    if( $uid ) {
      $rows = MotlDB::queryRows( "SELECT * FROM users WHERE id='".MRES($uid)."'" );
    }
    else {
      $descr = "";
      if( $login ) {
        $rows = MotlDB::queryRows( "SELECT * FROM users WHERE login='".MRES($login)."'" );
        $descr = "с логином '$login'";
      }
      
      if( !$rows ) {
        if( $email ) {
          $rows = MotlDB::queryRows( "SELECT * FROM users WHERE mail='".MRES($email)."'" );
          $descr = "с email '$email'";
        }
      }
    }
    
    
    $s = "";
    
    if( ! $rows ) {
      $s .= "Не найден пользователь $descr<p>";
      $s .= $this->produceStartForm();
      return $s;
    }

    if( count($rows) > 1 ) {
      $s .= $this->makeSelectUser( $rows, $descr );
      return $s;
    }
    
    $s .= $this->processUser( $rows[0] );
   
    return $s;
  }
  
  //=================
  
  function makeSelectUser( $rows, $descr ) {
    $s = "";
    $s .= "Найдено несколько учетных записей, зарегистрированных $descr<br>";
    $s .= "Выберите одну из них для восстановления:<p>";
    
    for( $k=0; $k<count($rows); $k++ ) {
      $row = $rows[$k];
      $s .= "<A href='registrationRestore.php?processStart=1&uid=$row->id'>$row->login</a><br>";
    }
    
    return $s;
  }
  
  //=================
  
  private function makeCode() {
    $m = md5( "".mt_rand() );
    return substr( $m, 0, $this->codeLen );
  }
  
  //=================
  
  function processUser( $row ) {
    $user = new User($row );
    
    $code = $this->makeCode();
    $uid  = $user->id;
    $md5  = md5("$code$uid");
    
    $changeParam = "$md5$uid";
    
    $url = Motl::getScriptsFolderUrl()."/registrationRestore.php?change=$changeParam";
    
    $s = "";
    $s .= "По адресу, указанному при регистрации, выслана ссылка для смены пароля - откройте ее в браузере.<br>"
         ."Чтобы ей никто не смог воспользоваться без Вашего ведома, понадобится ввести вот этот код: "
         ."<font color=red>$code</font>"
         ."<p>";
         
    $this->sendLetter( $user, $url, $code );
    
    return $s;
  }
  
  //=================
  
  private function produceChangeForm(  ) {
  
    $changeParam = Motl::getParameter("change");
    $md5  = substr( $changeParam, 0, 32 );
    $uid  = substr( $changeParam, 32 );

    $s = "";
    
    $s .= "<form name=F1 onsubmit='DoSubmit();return false;' action='registrationRestore.php'>";
    $s .= "<input type=hidden name=processChange value=$changeParam>";
    $s .= "<table class='MainText'>"
          ."<tr> <td>Введите код</td> <td><input name='code' style='width:80' type=text ></td> </tr>"
          ."<tr> <td>Введите новый пароль</td> <td><input name='pass1' style='width:155'  type=password ></td> </tr>"
          ."<tr> <td>Повторите новый пароль</td> <td><input name='pass2' style='width:155'  type=password ></td> </tr>"
          ."<tr> <td colspan=2><input type=submit value='Продолжить'></td> </tr>"
          ."</table>";

    $s .= "</form>";
    
    $s .= "<form name=F2 action='registrationRestore.php'>";
    $s .= "<input type=hidden name=processChange value=$changeParam>";
    $s .= "<input type=hidden name=code>";
    $s .= "<input type=hidden name=passHash1>";
    $s .= "</form>";
    
    return $s;
  }
  
  private function processChange(  ) {
    $changeParam = Motl::getParameter("processChange");
    $code        = Motl::getParameter("code");
    $passHash1   = Motl::getParameter("passHash1");
    ASSERT_alnum($passHash1);
    
    $s = "";
    
    $md5  = substr( $changeParam, 0, 32 );
    ASSERT_alnum($md5);
    $uid  = substr( $changeParam, 32 );
    ASSERT_int($uid);
    
    $codeOK =  ($md5==md5("$code$uid"));
    
    if( ! $codeOK ) {
      return "Неправильный код или ссылка.";
    }
    else {
      $ok = MotlDB::query( "UPDATE users SET passHash='".MRES($passHash1)."' WHERE id='".MRES($uid)."'" );
      
      if( $ok ) {
        $login = MotlDB::queryValue( "SELECT login FROM users WHERE id='".MRES($uid)."'" ); 
        $s .= "Установлен новый пароль для пользователя '$login'";
      }
    }
    return $s;
  }
  
  //=================

  private function sendLetter( $user, $url, $code ) {
    $mail = $user->row->mail;
    $login = $user->row->login;
    $name  = $user->row->name;

    $subject = "Востановление пароля на www.danceleague.ru";

    $message = $name ? "Здравствуйте, $name<br><br>\n" : "Здравствуйте<br><br>\n";

    $message .= "Вы запросили восстановление пароля на сайте <A href='http://www.danceleague.ru'>www.danceleague.ru</a><br><br>\n";
    $message .= "Если Вы не запрашивали восстановление пароля, просто игнорируйте это сообщение.<br><br>\n";
    $message .= "Чтобы завершить восстановление, откройте в браузере ссылку: <A href='$url'>$url</a><br><br>\n";
    $message .= "Ваш логин: <b>$login</b><br>\n";

    $headers = 'From: registration@danceleague.ru' . "\r\n" .
    'Content-Type: text/html; charset=utf-8';

    $send = getLocalSetting( "SEND_ACTIVATION" );

    if( $send ) {
      return mail( $mail, $subject, $message, $headers );
    }
    else {
      echo $message;
      return true;
    }
 }

  //=================
  
}

//=====================

MotlDB::connect();

$content = new RestorationFormProducer();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>

<script src="md5.js"></script>

<script>

function DoSubmit() {

  if( ! checkReg( document.forms['F1'] ) ) return;

  document.forms["F2"].elements["code"].value    = document.forms["F1"].elements["code"].value
  document.forms["F2"].elements["passHash1"].value   = MD5( document.forms["F1"].elements["pass1"].value )
//  document.forms["F2"].elements["passHash2"].value   = MD5( document.forms["F1"].elements["pass2"].value )

  document.forms["F2"].submit();
}

//-------------------

function checkReg( form ) {

  if( ! checkCode( form.elements["code"].value ) ) {
    form.elements["code"].focus();
    return false;
  }

  if( ! checkPass( form.elements["pass1"].value, form.elements["pass2"].value ) ) {
    form.elements["pass1"].focus();
    return false;
  }

  return true;  
}

//-------------------

function checkSymbols( str, good ) {
  var k;
  for( k=0; k<str.length; k++ ) {
    var ch = str.substr(k,1);
    if( good.indexOf(ch) < 0 ) return false;
  }
  return true;
}

//-------------------

var symbols_alnum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
var symbols_punct = "\"#$%&\'()*+,-./:;<=>?@[\\]^_\`{|}~";

function checkCode( str ) {

  if( str.length <= 0 ) {
    alert( 'Не введен код' );
    return false;
  }

  return true;
}

//-------------------

function checkPass( str, str1 ) {

  if( str.length <= 0 ) {
    alert( 'Не введен пароль' );
    return false;
  }

/*  if( str.length < 4 ) {
    alert( 'Пароль слишком короткий - нужно хотя бы 4 символа' );
    return false;
  }*/

  if( str != str1 ) {
    alert( '1й и 2й ввод пароля не совпадают' );
    return false;
  }

  return true;
}

//-------------------

function checkMail( str ) {

  if( str.length <= 0 ) {
    alert( 'Не указан e-mail' );
    return false;
  }

  if( str.indexOf('@') < 0 ) {
    alert( 'Указан неправильный e-mail' );
    return false;
  }

  return true;
}

//-------------------
</script>

