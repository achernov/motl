<?php
require_once "loginPageContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "RegLogin";

//=====================

$content = new RegLoginProducer();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
