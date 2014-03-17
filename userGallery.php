<?php
require_once "_layout.inc";
require_once "userGallery.inc";
//=====================

MotlDB::connect();

Motl::$Rubric = "forum";

//=====================

$userGallery = new UserGallery();
$userGallery->loadFromDB( Motl::getIntParameter("uid") );

$layout = new MotlLayout(); 
$layout->mainPane = $userGallery;

$layout->write();

//=====================
?>
