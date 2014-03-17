<?php
require_once "_motl_db.inc";
require_once "forumPoster.inc";
//=====================

MotlDB::connect();

//=====================

$poster = new ForumPoster();
$poster->save();

//=====================
//=====================
?>
