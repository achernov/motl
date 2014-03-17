<?php
require_once "_core.inc";
require_once "_dbproducer.inc";
require_once "_layout.inc";
require_once "captcha.inc";
//=====================
	
class RegistrationFormProducer extends Producer {

  //=================

  public function produce() {

    //-------------------
    $again  = Motl::getParameter( "again" );

    $script = Motl::getParameter( "script" );
    $id     = Motl::getParameter( "id" );

    $dbf_id    = Motl::getParameter( "dbf_id" );
    $passHash1 = Motl::getParameter( "passHash1" );
    $passHash2 = Motl::getParameter( "passHash2" );

    $name      = Motl::getParameter( "name" );
    $surname   = Motl::getParameter( "surname" );
    $mail      = Motl::getParameter( "mail" );
    $captchaId = Motl::getParameter( "captchaId" );
    $captchaCode = Motl::getParameter( "captchaCode" );
    //-------------------

    $oldCaptcha = new Captcha( $captchaId );
    $captchaOK = $oldCaptcha->checkInput( $captchaCode );

    $captcha = new Captcha();
    $captchaId = $captcha->getId();

    $s = "<h1>Регистрация на сайте:</h1>";

    $error = null;

    if( $again ) {

      if( !$dbf_id || !$passHash1 || !$passHash2 || !$mail )
        $error = "Заполнены не все обязательные поля";
      else
      if( $passHash1 != $passHash2 ) {
        $error = "Пароль введен по-разному";
      }
      else
        $error = null;
/*
      if( ! $captchaOK )
        $error = "Код введен неправильно";
*/

      if( $error ) {
        $s .= "<font color=red><b>$error</b></font><br>";
      }
      else {
        $newUser = new User();
//        $newUser->id = $newUser->row->id = $dbf_id;
        $newUser->row->login = $dbf_id;
        $newUser->row->registrationDate = date( "Y-m-d H:i:s", time() );
        $newUser->row->passHash = $passHash1;
        $newUser->row->mail = $mail;
        $newUser->row->name = $name;
        $newUser->row->surname = $surname;

        $newUser->row->activationCode = $this->makeActivationCode();

Q($newUser->row);

        $newUser->saveToDB();

        $s .= $this->produceRegistrationEnd( $newUser );

        return $s;
      }

    }


    //-------------------

    $req = "<font color=red><b>*</b></font>";

    $s .= "<div style='padding-right:100'>\n";
    $s .= "Значком $req отмечены поля, обязательные для заполнения.<br>\n";
    $s .= "Для регистрации необходим Ваш действующий адрес e-mail,"
         ." поэтому после отправки данных нужно будет подтвердить его правильность,"
         ." открыв в браузере ссылку, которая будет Вам выслана.<br>\n";
    $s .= "</div>\n";

    $s .= "<form name='F1' style='padding-top:10'>\n";

    $s .= "<table class='Registration' cellspacing=0 cellpadding=5 border=1>\n";

    $s .= "<tr> <td class='RegL'> $req Логин <br> <input type='text' name='dbf_id' class='RegInput' value='$dbf_id'/> </td>"
         ."<td class='RegR'> Это Ваше имя на этом сайте<br> </td> </tr>\n";

    $s .= "<tr> <td class='RegL'> $req Пароль <br> <input type='password' name='pass1' class='RegInput' /><br> "
              ." $req еще раз <br> <input type='password' name='pass2' class='RegInput' /> </td>"
         ."<td class='RegR'> Пароль должен быть не короче 4 знаков<br>"
              ."Пароль нужно ввести дважды, чтобы исключить ошибки </td> </tr>\n";

    $s .= "<tr> <td class='RegL'> Имя <br> <input type='text' name='name' class='RegInput' value='$name' /> </td>"
         ."<td rowspan=2 class='RegR'> Ваши имя и фамилия не будут раскрыты кому бы то ни было.<br>"
         ." Указывать их не обязательно. </td> </tr>\n";

    $s .= "<tr> <td class='RegL'> Фамилия <br> <input type='text' name='surname' class='RegInput' value='$surname' /> </td>"
         ."<!--td class='RegR'> &nbsp; </td--> </tr>\n";

    $s .= "<tr> <td class='RegL'> $req e-mail <br> <input type='text' name='mail' class='RegInput' value='$mail' /> </td>"
         ."<td class='RegR'> На этот адрес будет выслана ссылка для завершения регистрации.<br>"
         ." Никому, кроме администратора сайта он виден не будет </td> </tr>\n";

    $s .= "<tr> <td class='RegL'> $req Введите цифры: <A href='showCaptcha.php?captchaId=$captchaId'><img border=0 align=middle vspace=10 src='showCaptcha.php?captchaId=$captchaId'></a> <br>"
         ." <input type='text' name='captchaCode' /> </td>"
         ."<td class='RegR'> Это необходимо для защиты от автоматических регистраций </small></td> </tr>\n";

    $s .= "<tr> <td colspan=2 align=center> <input type='button' onclick='DoSubmit()' value='  OK  ' /> </td> </tr>\n";

    $s .= "</table>\n";

    $s .= "\n";

    $s .= "</form>\n";

    //-------------------
    $s .= "<form name='F2' action='registration.php' method='GET'>\n";
    $s .= "    <input type='hidden' name='again' value='1' />\n";

    $s .= "    <input type='hidden' name='script' value='$script' />\n";
    $s .= "    <input type='hidden' name='id'     value='$id' />\n";
    $s .= "    <input type='hidden' name='captchaId'     value='$captchaId' />\n";

    $s .= "    <input type='hidden' name='dbf_id' />\n";
    $s .= "    <input type='hidden' name='passHash1' />\n";
    $s .= "    <input type='hidden' name='passHash2' />\n";
    $s .= "    <input type='hidden' name='name' />\n";
    $s .= "    <input type='hidden' name='surname' />\n";
    $s .= "    <input type='hidden' name='mail' />\n";
    $s .= "    <input type='hidden' name='captchaCode' />\n";

    $s .= "</form>\n";
    //-------------------

    return $s;
  }

  //=================

  private function produceRegistrationEnd( $newUser ) {

    $sent = $this->sendActivationLetter( $newUser );

    $s = "";
    if( $sent ) {
      $s .= "Данные сохранены.<br><br>";

      $s .= "На Ваш e-mail <font color=blue>".$newUser->row->mail."</font> выслано письмо с информацией.<br><br>";

      $s .= "Для завершеия регистрации получите это письмо и откройте указанную в нем ссылку.";
    }
   else {
      $s .= "Не удалось выслать ссылку для завершения регистрации<br>";
      $s .= "на email ".$newUser->row->mail."<br>";
    }
    return $s;
  }

  //=================

  private function makeActivationCode() {
    return md5( "".mt_rand() );
  }

  //=================

  private function sendActivationLetter( $newUser ) {
    $mail = $newUser->row->mail;
    $code = $newUser->row->activationCode;
    $login = $newUser->row->login;
    $name  = $newUser->row->name;

    $subject = "регистрация на www.danceleague.ru";

    $url = Motl::getScriptsFolderUrl()."/activate.php?code=$code";

    $message = $name ? "Здравствуйте, $name<br><br>\n" : "Здравствуйте<br><br>\n";

    $message .= "Ваш адрес e-mail был введен при регистрации на сайте <A href='http://www.danceleague.ru'>www.danceleague.ru</a><br><br>\n";
    $message .= "Если Вы не регистрировались на www.danceleague.ru, просто игнорируйте это сообщение.<br><br>\n";
    $message .= "Чтобы завершить регистрацию, откройте в браузере ссылку: <A href='$url'>$url</a><br><br>\n";
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

$content = new RegistrationFormProducer();

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

  document.forms["F2"].elements["dbf_id"].value   = document.forms["F1"].elements["dbf_id"].value
  document.forms["F2"].elements["passHash1"].value   = MD5( document.forms["F1"].elements["pass1"].value )
  document.forms["F2"].elements["passHash2"].value   = MD5( document.forms["F1"].elements["pass2"].value )
  document.forms["F2"].elements["name"].value    = document.forms["F1"].elements["name"].value
  document.forms["F2"].elements["surname"].value = document.forms["F1"].elements["surname"].value
  document.forms["F2"].elements["mail"].value        = document.forms["F1"].elements["mail"].value
  document.forms["F2"].elements["captchaCode"].value = document.forms["F1"].elements["captchaCode"].value

  document.forms["F2"].submit();
}

//-------------------

function checkReg( form ) {

  if( ! checkLogin( form.elements["dbf_id"].value ) ) {
    form.elements["dbf_id"].focus();
    return false;
  }

  if( ! checkPass( form.elements["pass1"].value, form.elements["pass2"].value ) ) {
    form.elements["pass1"].focus();
    return false;
  }

  if( ! checkMail( form.elements["mail"].value ) ) {
    form.elements["mail"].focus();
    return false;
  }

  if( form.elements["captchaCode"].value.length <= 0 ) {
    form.elements["captchaCode"].focus();
    alert( 'Нужно ввести цифры, написанные на черном прямоугольнике,\nчтобы подтвердить, что форму заполняет человек, а не программа' );
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

function checkLogin( str ) {

  if( str.length <= 0 ) {
    alert( 'Не введен логин' );
    return false;
  }
/*
  if( ! checkSymbols( str, symbols_alnum ) ) {
    alert( 'Логин содержит неправильные символы.\nМожно использовать: латиницу, цифры' );
    return false;
  }
*/
  return true;
}

//-------------------

function checkPass( str, str1 ) {

  if( str.length <= 0 ) {
    alert( 'Не введен пароль' );
    return false;
  }

  if( str.length < 4 ) {
    alert( 'Пароль слишком короткий - нужно хотя бы 4 символа' );
    return false;
  }
/*
  if( ! checkSymbols( str, symbols_alnum+symbols_punct ) ) {
    alert( 'Пароль содержит неправильные символы.\nМожно использовать: латиницу, цифры, знаки препинания' );
    return false;
  }
*/
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

