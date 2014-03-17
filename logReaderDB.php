<?php
require_once "logDbContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

$layout = new MotlLayout(); 
$layout->noLog = true;

$layout->mainPane = new LogDbContent();

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
