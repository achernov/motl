<?php
require_once "subjectUtils.inc";
//=====================

define( "PROG_ST", 1 );
define( "PROG_LA", 2 );
define( "PROG_10", 3 );

//=====================

class CoupleResultProducer extends Producer {

	function produce() {
		$s = "";
		$s .= "<h1>Результат выступления пары</h1>";
		
		$idCE = Motl::getIntParameter( "idCE" );
		$rowCE = MotlDB::queryRow( "SELECT * FROM db_couple_event WHERE id=$idCE" ); // COD $idCE
		
		$idComp = (int)($rowCE->idEvent);
		$idCouple = (int)($rowCE->idCouple);
		$place = $rowCE->place;
		
		$rowComp = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=$idComp" ); // COD $idComp
		$compTitle = $rowComp->title;
		$program = programFromGroupCode( $rowComp->groupCode );
		$programName = getProgramName( $program );
		
		$idEvent = (int)($rowComp->pid);
		$eventTitle = MotlDB::queryValue( "SELECT title FROM db_events WHERE id=$idEvent" ); // COD $idEvent

		$placeRows = MotlDB::queryRows( "SELECT * FROM db_couple_event WHERE idEvent=$idComp" ); // COD $idComp
//print_r($placeRows);
		//-----------------------
		
		if( ! checkRating( $rowComp->groupCode ) ) {
			$ratingLevel='class';
		}
		else {
			switch( $rowComp->ratingLevel ) {
				case 'A': $ratingLevel='A'; break;
				case 'B': $ratingLevel='B'; break;
				default:  $ratingLevel='C'; break;
			}
		}
		
		//-----------------------
//$ratingLevel='B';
		//-----------------------
		
		$s .= "<b><big>$eventTitle</big><br>$compTitle</b><p>";

		$txtCompType = "";
		if( $ratingLevel == 'class' ) {
			$txtCompType = "классификационное";
		}
		else {
			$txtCompType = "рейтинговое категории $ratingLevel";
		}
		
		$s .= "<b>Вид соревнования:</b> $txtCompType<p>";
		
		$s .= "<b>Программа:</b> $programName<p>";

		$coupleTitle = SubjectCouple::loadTitle($idCouple);
		$s .= "<b>Пара:</b> $coupleTitle<p>";
		
		$s .= "<hr style='margin-top:30px;margin-bottom:30px'>";
		$s .= "<big><b>Подсчет очков</b></big><p>";
		
		//-----------------------
		
		$placePoints = 0;
		$resultPoints = 0;
		$finalPoints = 0;
		
		//-----------------------

		$placeRange = getPlaceRange( $place );
		
		if( $placeRange[0] != $placeRange[1] ) {
			$s .= "Пара разделила места $placeRange[0]-$placeRange[1].<br>"
				."В таких случаях для каждой из пар, разделивших места, очки расчитываются так,<br>"
				."как если бы она заняла лучшее из разделенных мест.<p>";
		}
		
		//-----------------------
		
		if( $ratingLevel == 'C' ) {
		
			$class = getClass( $rowComp->groupCode, $rowCE->class );
			if( ! $class ) {
				return "Не указан класс."
						."Для турнира категории C класс необходим для расчета очков."
						."Расчет не выполнен";
			}

			if( haveMissingClass($idComp) ) {
				$s .= "Для некоторых пар неизвестен класс.<br>Расчет очков может быть неточен.<p>";
			}

			$placing = getPlacingUsingClass( $rowComp->groupCode,  $idCouple, $placeRows );
			$above = $placing[0]-1;
			$below = $placing[1]-1-$above;
			
			$s .= "Данная пара обошла $below ".nf($below,'пару','пары','пар')." своего или более высокого класса,<br>"
				."ее ".nf($above,'обошла','обошли','обошли')." $above ".nf($above,'пара','пары','пар')." своего или более низкого класса.<br>"
				."Для турниров категории C очки расчитываются исходя из места среди этого состава пар,<br>"
				."в данном случае - место $placing[0] из $placing[1].<p>";
				
			//binlog( 2, 8 );
			//testBinlog();
		}
		else {
			$placing = getPlacingNoClass( $idCouple, $placeRows );
		}
		
		//-----------------------
		//print_r($placing);
		//die($s);
		$placePoints = binlog( $placing[0], $placing[1] );
		
		switch( $ratingLevel ) {
			case 'A': $resultPoints = $placePoints ? 2 : 0; break;
			case 'B': $resultPoints = $placePoints ? 1 : 0; break;
			case 'C': $resultPoints = $placePoints ? 0.5 : 0; break;
		}
		
		$finalPoints = isFinal($rowComp,$idCouple) ? 0.5 : 0;
		
		//-----------------------
		
		$s .= "За место $placing[0] из $placing[1] пара получает ".of($placePoints)."<p>";
		
		if( $resultPoints ) {
			$s .= "За попадание в очковую зону - дополнительно ".of($resultPoints)."<p>";
		}
		
		if( $finalPoints ) {
			$s .= "За участие в финале - дополнительно ".of($finalPoints)."<p>";
		}
		
		//-----------------------

		$txtPointsSt   = "";
		$txtPointsLa   = "";
		$txtPointsComb = "";
		
		$zamesto = ($resultPoints||$finalPoints) ? "за занятое место" : "";
		
		switch( $program ) {
		
			case PROG_ST:
				$txtPointsSt   = formatFormula( $placePoints,false, $resultPoints, $finalPoints );
				$txtPointsLa   = "";
				$txtPointsComb = formatFormula( $placePoints,true,  $resultPoints, $finalPoints );
				break;
				
			case PROG_LA:
				$txtPointsSt   = "";
				$txtPointsLa   = formatFormula( $placePoints,false, $resultPoints, $finalPoints );
				$txtPointsComb = formatFormula( $placePoints,true,  $resultPoints, $finalPoints );
				break;
				
			default:
				$txtPointsSt   = formatFormula( $placePoints,true,  $resultPoints, $finalPoints );
				$txtPointsLa   = formatFormula( $placePoints,true,  $resultPoints, $finalPoints );
				$txtPointsComb = formatFormula( $placePoints,false, $resultPoints, $finalPoints );
				break;
		}
		
		//-----------------------
		/*
		echo "txtPointsSt = $txtPointsSt<br>";
		echo "txtPointsLa = $txtPointsLa<br>";
		echo "txtPointsComb = $txtPointsComb<br>";
		*/
		//-----------------------

		$s .= "<b>Итого получается следующее количество очков:</b><br>";
		$s .= "<table class='MainText'>";
		$s .= "<tr> <td colspan=2>&nbsp;<br><b>Для раздельных программ по St и La<br>(C класс и выше, а также N16, E16, D16)</td> </tr>";
		if( $program != PROG_LA ) {
			$s .= "<tr> <td>По европейской программе: </td> <td>$txtPointsSt</td> </tr>";
		}
		if( $program != PROG_ST ) {
			$s .= "<tr> <td>По латиноамериканской программе: </td> <td>$txtPointsLa</td> </tr>";
		}
		$s .= "<tr> <td colspan=2>&nbsp;<br><b>Без разделения программ по St и La<br>(D класс и ниже, кроме N16, E16, D16):</td> </tr>";
		$s .= "<tr> <td> </td> <td>$txtPointsComb</td> </tr>";
		$s .= "</table>";
		
		//-----------------------

		$s .= "<big><A href='coupleResultPrint.php?idCE=$idCE'><b>Печать наклеек</a></big><p>";
//		$s .= "<hr style='margin-top:30px;margin-bottom:30px'>";
		
		//-----------------------
		if( User::isAdmin() ) {
//			$s .= "<b><big>Печать наклеек</big></b><p>";
			
		}

		//-----------------------
		return $s;
	}
};

//=====================

function formatFormula( $placePoints, $halfPlacePoints, $resultPoints, $finalPoints ) {
	
	$zamesto = ($resultPoints||$finalPoints) ? " за занятое место" : "";
	
	$txtPlacePoints = $halfPlacePoints ? "$placePoints$zamesto / 2"
										: "$placePoints$zamesto";
	$valPlacePoints = $halfPlacePoints ? $placePoints/2 : $placePoints;
	
	//-------
	
	$terms = array();
	/*if( $placePoints )*/  { $terms[] = $txtPlacePoints; }
	if( $resultPoints ) { $terms[] = "$resultPoints за очковую зону"; }
	if( $finalPoints )  { $terms[] = "$finalPoints за финал"; }
	
	$txtSum = implode( " + ", $terms );
	$valSum = $valPlacePoints + $resultPoints + $finalPoints;
	
	//-------
	
	$s = $txtSum;
	if( trim($txtSum) != "$valSum" ) {
		$s .= " = $valSum";
	}
	
	return $s;
}

//=====================

function haveMissingClass( $idComp ) {

	$sql = "SELECT id FROM db_couple_event "
			."WHERE idEvent=$idComp AND (class='' OR class Is Null) LIMIT 1"; // UNSAFE
			
	$problemId = MotlDB::queryValue( $sql );
	return $problemId;
}

//=====================

function isFinal( $compRow, $idCouple ) {
	
	$idComp = $compRow->id;
	
	$sql = "SELECT idCouple FROM db_couple_event WHERE idEvent=$idComp AND place=1"; // UNSAFE
	$idWinner = MotlDB::queryValue( $sql );
	if( ! $idWinner ) {
		return false;
	}
	
	$sql = "SELECT e.id FROM db_events AS e INNER JOIN db_couple_event AS ce ON e.id=ce.idEvent "
			."WHERE e.pid=$idComp AND idCouple=$idWinner AND round=0"; // UNSAFE
	$finalId = MotlDB::queryValue( $sql );
	if( ! $finalId ) {
		return false;
	}
	
	$sql = "SELECT id FROM db_couple_event WHERE idEvent=$finalId AND idCouple=$idCouple"; // UNSAFE
	$inFinal = MotlDB::queryValue( $sql );
	return $inFinal;
}

//=====================

function getPlacingNoClass( $idCouple, $rows ) {
	$myRow = getRowById( $idCouple, $rows );
	$myPlaceRange = getPlaceRange( $myRow->place );
	return array( $myPlaceRange[0], count($rows) );
}

//---------------------

function getPlacingUsingClass( $groupCode, $idCouple, $rows ) {
	$myRow = getRowById( $idCouple, $rows );
	$above = 0;
	$below = 0;
	foreach( $rows as $row ) {
	
		//$cls = getClass( $groupCode, $row->class );
		//echo "$row->place\t$row->number\t$cls<br>";
		switch( comparePlaceUsingClass( $groupCode,  $myRow, $row ) ) {
		
			case 1:  
				$below++; 
				//echo "&gt;<p>"; 
				break;
				
			case -1: 
				$above++; 
				//echo "&lt;<p>"; 
				break;
		}
	}
	//echo "<p>";
	//echo "above = $above<br>";
	//echo "below = $below<br>";
	//print_r($myRow);
	return array( $above+1, $above+1+$below );
}

//---------------------

function comparePlaceUsingClass( $groupCode, $myRow, $otherRow ) {

	if( $myRow->idCouple == $otherRow->idCouple ) {
		return 0;
	}

	$myClass    = getClass( $groupCode, $myRow->class );
	$myPlaces	= getPlaceRange( $myRow->place ); // higher first
	$myPlace	= $myPlaces[0];
	
	$otherClass = getClass( $groupCode, $otherRow->class );
	$otherPlaces = getPlaceRange( $otherRow->place );
	$otherPlace	= $otherPlaces[1];
	
	//echo "$myPlace,$myClass - $otherPlace,$otherClass<br>";

	switch( compareClass( $myClass, $otherClass ) ) {
		case 1: // Наш класс выше
			return ($myPlace<$otherPlace) ? 0 : -1; // победа пофиг, проигрыш плохо
			
		case 0: // Классы равны
			return ($myPlace<$otherPlace) ? 1 : -1; // победа хорошо, проигрыш плохо
			
		case -1: // Наш класс ниже
			return ($myPlace<$otherPlace) ? 1 : 0; // победа круто, проигрыш пофиг
			
		default:
			return 0;
	}
	
}

//---------------------

function compareClass( $myClass, $otherClass ) {
	if( !$myClass || !$otherClass ) {
	//echo "incomparable<br>";
		return 1000;
	}
	
	$classValues = array( "M"=>0, "S"=>1, "A"=>2, "B"=>3, "C"=>4, "D"=>5, "E"=>6, "N"=>7 );
	$myValue = $classValues[$myClass];
	$otherValue = $classValues[$otherClass];

	if( $myValue < $otherValue ) return 1; // Наш класс выше
	if( $myValue > $otherValue ) return -1; // Наш класс ниже
	return 0; // классы равны
}

//---------------------

function getPlaceRange( $place ) {
	$aplace = explode( "-", $place );
	$range[0] = (int)$aplace[0];
	$range[1] = isset($aplace[1]) ? (int)$aplace[1] : (int)$aplace[0];
	//echo "place: $place ($range[0]:$range[1])<br>";
	return $range;
}

//---------------------

function getRowById( $idCouple, $rows ) {
	foreach( $rows as $row ) {
		if( $row->idCouple == $idCouple ) {
			return $row;
		}
	}
	return null;
}

//=====================

function getClass( $groupCode, $classCode ) {
//echo "getClass( $groupCode, $classCode )<p>";
	$aclass = explode( ":", $classCode );
	
	$cSt  = (isset($aclass[1])&&$aclass[1]) ? $aclass[1] : "";
	$cLa  = (isset($aclass[2])&&$aclass[2]) ? $aclass[2] : "";
	$cAll = (isset($aclass[0])&&$aclass[0]) ? $aclass[0] : "";

	$cComb = "";
	$cc = compareClass($cSt,$cLa);
	switch( compareClass($cSt,$cLa) ) {
		case  1: $cComb=$cLa; break; // St is higher, take La
		case -1: $cComb=$cSt; break; // La is higher, take St
		case 0:  $cComb=$cSt; break; // all the same, tke any
	}
//echo "cSt=$cSt|cLa=$cLa|cAll=$cAll|cComb=$cComb|cc=$cc<p>";	
	switch( programFromGroupCode($groupCode) ) {
		case PROG_ST:	return $cSt ? $cSt : $cAll;
		case PROG_LA:	return $cLa ? $cLa : $cAll;
		default:    return $cAll ? $cAll : $cComb;
	}
}

//=====================

function getProgramName( $program ) {
	switch( $program ) {
		case PROG_ST: return "St";
		case PROG_LA: return "La";
		case PROG_10: return "St+La";
		default:      return "";
	}
}

//=====================

function programFromGroupCode( $groupCode ) {
	switch( substr($groupCode,-3) ) {
		case '.st':	return PROG_ST;
		case '.la':	return PROG_LA;
		default:    return PROG_10;
	}
}

//=====================

function testBinlog() {
	$max = 32;
	
	echo "<table border=1 cellspacing=0>\n";
	
	echo "<tr bgcolor='#cccccc'> <td rowspan=2><center><b>N</b></center></td> "
		."<td colspan=$max><center><b>Place</b></center></td> </tr> ";
		
	echo "<tr bgcolor='#cccccc'> ";
	
	for( $p=1; $p<=$max; $p++ ) {
		echo "<td width=30><center><b>$p</b></center></td> ";
	}
	echo "</tr>\n";
	
	for( $n=$max; $n>0; $n-- ) {
		echo "<tr> <td bgcolor='#cccccc'><b>$n</b></td> ";
		for( $p=1; $p<=$max; $p++ ) {
			if( $p <= $n ) {
				$points = binlog( $p, $n );
				echo "<td><center>$points</center></td> ";
			}
			else {
				echo "<td>&nbsp;</td> ";
			}
		}
		echo "</tr>\n";
	}
	echo "</table>\n";
}

//=====================

function binlog( $place, $total ) {
	//die( "=$place=$total=" );
	$points = 0;
	//echo "<p>binlog( $place, $total )<br>";
	for( $t=$total>>1; $t>=$place; $t=$t>>1 ) {
		//echo "-> $t<br>";
		$points++;
	}
	return $points;
}

//=====================

function checkRating( $groupCode ) {
	$part = substr($groupCode,0,2);
	$ratingParts = array('ju','yo','am','se');
	return in_array( $part, $ratingParts );
}

//=====================

function nf( $n, $w1,$w2,$w5 ) {
	if( $n != (int)$n ) {
		//die( "$n" );
		$n = 2; // потому что та же фора слова
	}
	return DataUtil::numForm( $n, $w1,$w2,$w5 );
}

//=====================

function of( $n ) {
	return $n . nf( $n, ' очко',' очка',' очков' );
}

//=====================
?>
