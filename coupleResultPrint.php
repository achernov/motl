<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "coupleResultContent.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='printResult.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
//echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

//echo "<A href='base.php?id=tools'>назад</a><br>";
echo produce();

//=====================

function produce() {

	$idCE  = Motl::getParameter( "idCE" );
	
	$s = "";

//	$s = "<p>".$_SERVER["HTTP_USER_AGENT"]."<p>";
	$url = "coupleResultPrint.php?idCE=$idCE";
	$wholeEvent  = Motl::getIntParameter("wholeEvent")?"1":"0";
	$makeTwo     = Motl::getIntParameter("makeTwo")?"1":"0";
	$delta       = (int)Motl::getParameter("delta","0");

	$deltaLess = $delta-1;
	$delta100 = 100+$delta/2;
	$deltaMore = $delta+1;
	
	$s .= "<table class='printControl' style='padding-left:15px;padding-right:15px;padding-bottom:15px;background-color:#8080ff;border:#8080ff 2px solid;margin-bottom:50px' cellspacing=0 cellpadding=5 border=0>";
	$s .= "<tr><td colspan=2 style='font-size:24px'>";
	$s .= "Напечатать вклейки в книжку:<br>";
	$s .= "</td></tr>";
	$s .= "<tr><td style='padding-left:20px'>";
		$s .= makeCheck( !$wholeEvent, "$url&wholeEvent=0&makeTwo=$makeTwo&delta=$delta", "только для 'Взрослые 10 танцев" );
		$s .= "<br><br style='font-size:3px'>";
		$s .= makeCheck( $wholeEvent, "$url&wholeEvent=1&makeTwo=$makeTwo&delta=$delta", "для всего турнира Russian Cup" );
	$s .= "</td><td style='padding-left:20px;padding-right:20px'>";
		$s .= makeCheck( !$makeTwo, "$url&wholeEvent=$wholeEvent&makeTwo=0&delta=$delta", "1 комплект" );
		$s .= "<br><br style='font-size:3px'>";
		$s .= makeCheck( $makeTwo, "$url&wholeEvent=$wholeEvent&makeTwo=1&delta=$delta", "2 комплекта" );
	$s .= "</td><td style='padding-left:20px;padding-right:20px' valign=top>";
		$s .= "<b>Подогнать размер:</b><br>";
		$s .= "<A href='$url&wholeEvent=$wholeEvent&makeTwo=$makeTwo&delta=$deltaLess'>меньше</a>"
			 ."<b>$delta100%</b>"
			 ."<A href='$url&wholeEvent=$wholeEvent&makeTwo=$makeTwo&delta=$deltaMore'>больше</a>";
	$s .= "</td></tr></table>";

	$scale = $delta100/100;
	
	$s .= makePercentRuler( 176, $scale );

	$set = makeOneSet( $idCE, $wholeEvent, $scale );
	
	if( $makeTwo ) {
		$s .= "<p>Комплект1$set";
		$s .= "Комплект2$set";
	}
	else {
		$s .= $set;
	}
	
	return $s;
}

//=====================

function makeOneSet( $idCE, $wholeEvent, $scale=1 ) {
	$s = "";
	
	if( ! $wholeEvent ) {
		$s .= makeLabel( $idCE, $scale );
	}
	else {
		$sql = "SELECT ce1.id " // UNSAFE
			."FROM db_couple_event AS ce0 "
			."INNER JOIN db_events AS e0 ON ce0.idEvent=e0.id "
			."INNER JOIN db_events AS e1 ON e1.pid=e0.pid "
			."INNER JOIN db_couple_event AS ce1 ON ce1.idEvent=e1.id "
			."WHERE ce0.id=$idCE AND ce1.idCouple=ce0.idCouple";
		//echo $sql;
		$idCEs = MotlDB::queryValues( $sql );
		foreach( $idCEs as $idCE1 ) {
			$s .= makeLabel( $idCE1, $scale );
			//$scale += 0.3;
			//echo "scale=$scale<br>";
		}
	}
	
	return $s;
}

//=====================

function makeCheck( $selected, $url, $text ) {
	$st = $selected ? "1" : "0";
	$s = "<img style='padding-right:10px' border='0' src='syspictures/checkbox$st.png'>$text";
	if( ! $selected ) {
		$s = "<a style='text-decoration:none' href='$url'>$s</a>";
	}
	return $s;
}

//=====================

function makeLabel( $idCE, $p=1 ) {

	$rowCE    = MotlDB::queryRow( "SELECT * FROM db_couple_event WHERE id=".(int)$idCE );
	$rowComp  = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=".(int)($rowCE->idEvent) );
	$rowEvent = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=".(int)($rowComp->pid) );

	$nCouples = $rowComp->ncouples;
	
	$place = $rowCE->place;
	$place = str_replace( "-", " - ", $place );
	
	$event = $rowEvent->title;
	$comp = $rowComp->title;
	$date = DataUtil::formatDate( "d.m.Y", $rowEvent->date );
	

	switch( programFromGroupCode($rowComp->groupCode) ) {
		case PROG_ST:
			$placeSt = $place;
			$placeLa = "";
			break;
			
		case PROG_LA:
			$placeSt = "";
			$placeLa = $place;
			break;
			
		case PROG_10:
			$placeSt = $place;
			$placeLa = $place;
			break;
	}

	$s = "";

	$s .= "<p><div style='position:relative;height:3cm;' >";

	
	$pointsSt = 3.5;
	$pointsLa = 3.5;

	$pointsSt = $pointsLa = '';

	// 10	47.5	82	94	111	122	133	143.5	154	176
	// 10	37.5	34.5	12	17	11	11	10.5	10.5	22

	//$s .= "<table cellpadding=0 cellspacing=0><tr align=center valign=middle>";
	//$dImg = "<img style='height:20mm' src='verticalText.php?text=12.12.2001'>";
	$dImg = "<img style='height:20mm' src='verticalText.php?text=$date'>";
	
	
	$s .= makeLabelCell1( "fullBorder", $p, 0, 10,  "$dImg" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 10, 37.5, "$event" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 47.5, 34.5, "$comp" );
	$s .= makeLabelCell1( "rBorder",    $p, 82, 12,   "" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 94, 17,   "$nCouples" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 111, 11,   "$placeSt" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 122, 11,   "$placeLa" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 133, 10.5, "$pointsSt" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 143.5, 10.5, "$pointsLa" );
	$s .= makeLabelCell1( "rtbBorder",  $p, 154, 22,   "" );
	//$s .= "</table>";
	$s .= "</div>";
	
	return $s;
}

//=====================

function makePercentRuler( $size, $scale=1 ) {

	$s = "";
	
	$s .= "<div style='float:left;text-align:right;width:176mm'>&nbsp;</div>";
	$s .= "<div style='float:left;padding-bottom:2.5mm'>100%</div>";
	$s .= "<p style='clear:both;font-size:1px'>";

	
	$s .= makeStrokeP( $scale, $size,-5, 4.5,  false,false );
	$s .= makeStrokeP( $scale, $size,-4, 3,  true,false );
	$s .= makeStrokeP( $scale, $size,-3, 3,  true,false );
	$s .= makeStrokeP( $scale, $size,-2, 3,  true,false );
	$s .= makeStrokeP( $scale, $size,-1, 3,  true,false );
	$s .= makeStrokeP( $scale, $size, 0, 7,  true,false );
	$s .= makeStrokeP( $scale, $size, 1, 3,  true,false );
	$s .= makeStrokeP( $scale, $size, 2, 3,  true,false );
	$s .= makeStrokeP( $scale, $size, 3, 3,  true,false );
	$s .= makeStrokeP( $scale, $size, 4, 3,  true,false );
	$s .= makeStrokeP( $scale, $size, 5, 4.5,  true,true );
	
	$s .= "<div style='font-family:arial;font-size:3mm'>Если размер таблицы получился шире или уже, чем в Вашей книжке, измерьте правильную таблицу<br>этой шкалой, чтобы узнать, на сколько %% нужно увеличить или уменьшить масштаб при печати</div>";

	return $s;
}

//---------------------

function makeStrokeP( $scale, $w,$p, $h, $doTop, $doBottom ) {
	$w1 = $w*(100+$p)/100.0;
	return makeStroke( $scale*$w1, $h, $doTop, $doBottom );
}

//---------------------

function makeStroke( $w, $h, $doTop, $doBottom ) {
	
	$style = "width:{$w}mm;height:{$h}mm";
	
	if( $doTop ) {
		$style .= ";margin-top:-{$h}mm";
	}
	
	if( $doBottom ) {
		$style .= ";border-bottom:1px solid black";
	}
	
	return "<div class=stroke style='$style'>&nbsp;</div>\n";
}

//=====================

function makeLabelCell( $class, $width, $text ) {
	
	$height = "24mm";
	
	$s = "";
	$s .= "<div class='cell $class' style='width:$width;height:$height'>";
	$s .= "<div class='cellIn'>";
	$s .= "<table cellpadding=0 cellspacing=0 border=0 width=100% height=100% ><tr><td valign=center align=center>";
	$s .= $text;
	$s .= "</td></tr></table>";
	$s .= "</div>";
	$s .= "</div>";
	
	return $s;
}

//=====================

function makeLabelCell1( $class, $p, $x, $width, $text ) {
	$col = dechex(rand(0,255)) . dechex(rand(0,255)) . dechex(rand(0,255));
	//die( "$col" );
	$height = "24mm";
	
	$xs = $p*$x; 
	$ws = $p*$width;
	
	$s = "";
	$s .= "<div class='cell $class' style='position:absolute; left:{$xs}mm;width:{$ws}mm;height:$height'>";
	$s .= "<div class='cellIn'>";
	$s .= "<table cellpadding=0 cellspacing=0 border=0 width=100% height=100% ><tr><td valign=center align=center>";
	$s .= $text;
	$s .= "</td></tr></table>";
	$s .= "</div>";
	$s .= "</div>";
	
	return $s;
}

//=====================
?>
