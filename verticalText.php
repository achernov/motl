<?php
require_once "_core.inc";
//=====================

$text  = Motl::getParameter( "text" );

header ('Content-type: image/png');

$th = 14;
$tl = 80;

$im = imagecreatetruecolor($th,$tl);

$back_color = imagecolorallocate($im, 255,255,255 );
imagefilledrectangle ( $im, 0,0,1000,1000, $back_color );

$text_color = imagecolorallocate($im, 0,0,0 );
imagestringup($im, 4, 0, $tl,  $text, $text_color);
imagepng($im);
imagedestroy($im);
//=====================
?>
