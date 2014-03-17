<?php
require_once "profileContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "forum";

$id = Motl::getParameter( "id" );

$content = new ProfileProducer();
$content->loadFromDB( $id );

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
