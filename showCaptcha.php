<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "captcha.inc";
//=====================

MotlDB::connect();

//=====================

  $captchaId  = Motl::getParameter( "captchaId" );
  $captcha = new Captcha( $captchaId );

  $captcha->showCaptcha();

//=====================

?>



