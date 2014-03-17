<?php
require_once "newsContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "news";

//=====================

$content = new NewsContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

$layout->write();

//=====================
?>
