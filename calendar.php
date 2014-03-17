<?php
require_once "_dbproducer.inc";
require_once "dbAll.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "cald";

//=====================

require_once "calendarContent.inc";

//=====================

$content = new CalendarContent( true );

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
