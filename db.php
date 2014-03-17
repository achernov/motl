<?php
require_once "dbContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();
Motl::$Rubric = "base";

//DataUtil::updateEventBase();
$content = new DbContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();


//=====================
?>
