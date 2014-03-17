<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_dbproducer.inc";
require_once "dbAll.inc";
require_once "dbEvent.inc";
require_once "regManager.inc";

//print_r( $_REQUEST );die();
//=====================
header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

//=====================

$idComp = Motl::getParameter( "idEvent" );
$compProducer = new DbEventProducerFull();
$compProducer->loadFromDB( $idComp );

$handler = RegManager::getHandlerForCode( $compProducer->row->regType, "accept" );

$handler->processListEdit( $compProducer );

//=====================
?>
