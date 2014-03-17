<?php
require_once "logContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

$layout = new MotlLayout(); 
$layout->noLog = true;

$layout->mainPane = new LogContent();

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
