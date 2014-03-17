<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_layout.inc";
require_once "regManager.inc";
//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

class RegFormReceiver extends DbProducer {

  function produce() {
    $regType = Motl::getParameter( "regType" );
    $handler = RegManager::getHandlerForCode( $regType, "accept" );

    return $handler ? $handler->processUserInput() : null;
  }

}

//=====================

MotlDB::connect();
Motl::$Rubric = "reg";

$layout = new MotlLayout(); 
$layout->mainPane = new RegFormReceiver();

$layout->write();

//=====================
?>
