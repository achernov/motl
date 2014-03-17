<?php
require_once "galleryContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "gallery";

$content = new GalleryContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
