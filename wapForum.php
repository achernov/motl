<?php 
require_once "_core.inc";
require_once "wapFilter.inc";
require_once "forumContent.inc";
//=====================

$script = $_SERVER['PHP_SELF'];
$id = Motl::getParameter("id");

$s  = "<wml><card>";

MotlDB::connect();

$producer = new ForumItemProducer();
$producer->loadFromDB( $id ? $id : "0" );

$s .= $producer->produceWap();
    
$s .= "</card></wml>";
Motl::writeWap( $s );

//=====================
?>
