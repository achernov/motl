<?php
require_once "_core.inc";
//=====================

setcookie( "motlUid", "" );
setcookie( "motlHash", "" );
//=====================

Motl::redirectToRetUrl();

//=====================
?>
