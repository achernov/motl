<?php
require_once "forumContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "forum";

$content = new ForumContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
