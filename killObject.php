<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_dbproducer.inc";
require_once "_dbproducer_factory.inc";
//=====================

MotlDB::connect();

$what = Motl::getParameter( "what" );

$object = DbProducerFactory::createObject( $what );

if( ! $object )
  motlDie( "Can't create: $what" );

$object->loadFromQuery();

$object->killFromDB();

//=====================

Motl::redirectToRetUrl();

//=====================
?>
