<?php
require_once "chatContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "chat";

$content = new ChatContent();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

//$layout->write( "noRight" );
$layout->write( );

//=====================
?>
