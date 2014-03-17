<?php
require_once "logErrContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

$layout = new MotlLayout(); 
$layout->noLog = true;

$layout->mainPane = new LogErrContent();

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
