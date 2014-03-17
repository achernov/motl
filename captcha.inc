<?php
require_once "_motl_db.inc";
//=====================

class Captcha {

  private $captchaId;

  private $isValid;

  private $code = null;

  //===========

  function __construct( $captchaId = null ) {

    if( strlen($captchaId) ) {
      ASSERT_alnum( $captchaId );

      // ASS $captchaId
      $row = MotlDB::queryRow( "SELECT * FROM captchas WHERE id='$captchaId'" );  
      if( $row ) {
        if( isset($row->id) && isset($row->code) ) {
          $this->isValid   = 1;
          $this->captchaId = $row->id;
          $this->code      = $row->code;
        }
      }

    }
    else {

      $captchaId = mt_rand();

      $code = $this->makeCode();

      // COD $captchaId $code
      $this->isValid = MotlDB::query( "INSERT INTO captchas(id,created,code) VALUES('$captchaId',now(),'$code')" );
      if( $this->isValid ) {
        $this->captchaId = $captchaId;
        $this->code      = $code;
      }

      $this->killOldCaptchas();
    }
  }

  //-----------

  private function makeCode() {
    $a = md5( "".mt_rand() );
    $b = intval(substr($a,0,5),16);
    return substr( "0000$b", -4 );
  }

  //-----------

  private function killOldCaptchas() {

    $minutesToLive = 10; // Captcha's life time

    $secondsToLive = 60*$minutesToLive;
    MotlDB::query( "DELETE FROM captchas WHERE now()-created>$secondsToLive" );
  }

  //===========

  public function getId() {
    return  $this->captchaId;
  }

  //===========

  public function checkInput( $input ) {
    ASSERT_alnum( $this->captchaId );
    // ASS $this->captchaId
    MotlDB::query( "DELETE FROM captchas WHERE id='$this->captchaId'" );
    
    if( ! $this->code ) return 0;
    return  ( $input == $this->code ) ? 1 : 0;
  }

  //===========

  public function showCaptcha() {
    header ('Content-type: image/png');

    $im = imagecreatetruecolor(50, 25);

    $text_color = imagecolorallocate($im, 233, 14, 91);
    imagestring($im, 8, 5, 5,  $this->code, $text_color);
    imagepng($im);
    imagedestroy($im);
  }

  //===========
}

//=====================

?>
