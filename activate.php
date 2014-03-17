<?php
require_once "_core.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

//  http://chernov-pc/motl/activate.php?code=8c22e84fcea6fbe99c13e477d0b1bbea

//=====================
	
class ActivationProducer extends Producer {

  //=================

  public function produce() {
    $code = Motl::getParameter( "code" );
    ASSERT_hex( $code );

    $result = null;

    $row = MotlDB::queryRow( "SELECT * FROM users WHERE activationCode='".MRES($code)."'" );
    if( ! $row ) {
      $result = $code ? "BadCode" : "";
    }
    else {
      $uid = $row->id;
      $login = $row->login;

      if( $row->userState != "NotActivated" ) {
        $result = "Ok";
      }
      else {
        MotlDB::query( "UPDATE users SET userState='active' WHERE id='".MRES($uid)."'" );
        $newState = MotlDB::queryValue( "SELECT userState FROM users WHERE id='".MRES($uid)."'", "" );

        if( $newState == 'active' ) {
          $result = "Ok";
        }
        else {
          $result = "Fail";
        }
      }
    }

    $s = "";
    $s .= "<h1>Завершение регистрации</h1>";

    switch( $result ) {

      case "BadCode":
        $s .= "Неправильная или устаревшая ссылка";
        break;
        
      case "Fail":
        $s .= "Не удалось завершить регистрацию для пользователя <b>$login</b>";
        break;
        
      case "Ok":
        $s .= "Регистрация пользователя <b>$login</b> завершена.";

        $lf = new LoginForm();
        $s .= $lf->makeLoginForm( );

        break;
        
    }

    return $s;
  }

  //=================

}

//=====================

$content = new ActivationProducer();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

$layout->write();

//=====================
?>
