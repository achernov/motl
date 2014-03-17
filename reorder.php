<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_dbproducer.inc";
require_once "_dbproducer_factory.inc";
require_once "sortManager.inc";
//=====================

MotlDB::connect();

$what = Motl::getParameter( "what" );

$object = DbProducerFactory::createObject( $what );

if( ! $object )
  motlDie( "Can't create: $what" );

//=====================

$ids = Motl::getParameter( "ids" );

SortManager::setOrder( $object, $ids );

//=====================

Motl::redirectToRetUrl();

//=====================
?>
