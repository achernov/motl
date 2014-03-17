<?php
require_once "_core.inc";
require_once "_layout.inc";
require_once "coupleResultContent.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "res";

//=====================

$content = new CoupleResultProducer();
//die( $content->produce() );

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
