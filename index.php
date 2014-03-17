<?php

//for( $k=1; $k<256; $k++ ) {	echo chr($k); }
/*
$s = "'\"%&/\\<>";
for( $k=0; $k<strlen($s); $k++ ) {	
	$c = substr($s,$k,1);
	$d = ord($c);
	echo "$c - $d<br>";
}
die();
*/

$isMainPage = true;
include("page.php");
?>
