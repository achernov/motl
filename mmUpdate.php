<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "mediaManager.inc";
//=====================

MotlDB::connect();

MediaManager::updateFromDisk();


//=====================
?>
