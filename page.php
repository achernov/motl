<?php
require_once "pageContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

$id = Motl::getParameter( "id", "about" );

Motl::$Rubric = $id;


//=====================

$content = new PageContent();
$content->loadFromDB( $id );

$layout = new MotlLayout(); 
$layout->mainPane = $content;
//Q($content);
if( $content->row ) {
  MotlLayout::$noRight = $content->row->noRight ? true : false;
}
$layout->write();

//=====================
?>
