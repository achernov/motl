<?php
require_once "_layout.inc";
require_once "_dbproducer_factory.inc";
//=====================

MotlDB::connect();

$what = Motl::getParameter( "what" );

$object = DbProducerFactory::createObject( $what );

if( ! $object )
  motlDie( "Can't create: $what" );

$dbf_id = Motl::getParameter( "dbf_id" );

if( ! is_null($dbf_id) ) {
  $object->loadFromDB( $dbf_id );
  if( Motl::getParameter( "doCopy" ) ) {
    $object->id = null;
    $object->row->id = null;
    $object->isNew = 1;
  }
}
else {
  $object->loadFromQuery();
  $object->isNew = 1;
}

//=====================

$form = $object->produce("edit");

$layout = new MotlLayout(); 
$layout->mainPane = $form;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
