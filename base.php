<?php
require_once "baseContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();
Motl::$Rubric = "base";

//DataUtil::updateEventBase();
$content = new BaseContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();


//=====================
?>
