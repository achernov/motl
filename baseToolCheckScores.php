<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
//require_once "dbAll.inc";
require_once "dbEvent.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

echo "<A href='base.php?id=tools'>назад</a><br>";

checkAllEvents();

//=====================

function checkAllEvents() {

  $rows = MotlDB::queryRows( "SELECT * FROM db_events "
                            ."WHERE pid is null AND type='event' "
                            ."ORDER BY date DESC" );
                            
  for( $k=0; $k<count($rows); $k++ ) {
    echo checkEvent( $rows[$k] );
  }
  
}

//=====================

function checkEvent( $eventRow ) {

  $title = "<p><big><big><b>$eventRow->date $eventRow->title</b></big></big><p>\n";
  
  $sql = "SELECT * FROM db_events "
        ."WHERE pid=".MRES($eventRow->id)." AND type='competition' AND (regType is null OR regType='couple') "
        ."ORDER BY position,id";

  $rows = MotlDB::queryRows( $sql );
                     
  $s = "";
  for( $k=0; $k<count($rows); $k++ ) {
    $ds = checkComp( $rows[$k] );
    if( $ds ) {
      $s .= $ds;
    }
  }

  return $s ? $title.$s : null;
}

//=====================

function checkComp( $compRow ) {

  $title = "<br><b>$compRow->title</b><br>\n";
  
  $sql = "SELECT * FROM db_events "
        ."WHERE pid=".MRES($compRow->id)." AND type='round' AND (hidden is null OR not hidden) "
//." AND id=1970 "
        ."ORDER BY round";
  $rows = MotlDB::queryRows( $sql );

  $s = "";
  for( $k=0; $k<count($rows); $k++ ) //if($rows[$k]->id==1079) 
  {
    $ds = checkRound( $compRow, $rows[$k] );
    if( $ds ) {
      $s .= $ds;
    }
  }
  
  //return $title.$s;
  return $s ? $title.$s : null;
}

//=====================

function checkRound( $compRow, $roundRow ) {

  $url = makeCompetitionLink($roundRow->id);
  $title = "<b><A href=$url>$roundRow->title</a>:</b><br>";
      
  $err = getScoreErrors( $compRow, $roundRow );
  
  return $err ? $title.$err : null;
}

//=====================

function makeCompetitionLink($idComp) {
  $pid   = MotlDB::queryValue( "SELECT pid FROM db_events where id=".MRES($idComp), null );
  $pipid = MotlDB::queryValue( "SELECT pid FROM db_events where id=".MRES($pid), null );
  return "db.php?id=e$pipid,r$idComp";
}

//---------------------

function getScoreErrors( $compRow, $roundRow ) {

  $spacer = "&nbsp;&nbsp;";
  $err = "";
  $maxDances = 10;
  $nDances = 0;
  $fields = array( "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da" );
  
  $rows = MotlDB::queryRows( "SELECT * FROM db_couple_event where idEvent=".MRES($roundRow->id) );
  
  //----- get dances
  
  $roundProducer = new DbEventProducerFull($roundRow);
  
  $dances = $roundProducer->getDances( $compRow );
  $nOfficialDances = count($dances);
  for( $d=$nOfficialDances; $d<$maxDances; $d++ ) {
	$dances[$d] = "???";
  }
  
  //----- analyze lengths
  
  $lo = array();
  $hi = array();
  
  if( $rows && count($rows) ) {
  
	  for( $d=0; $d<$maxDances; $d++ ) {
		$lo[$d] = $hi[$d] = mb_strlen( trim( $rows[0]->$fields[$d] ), "utf-8" );
	  }
		
	  for( $r=0; $r<count($rows); $r++ ) {
		for( $d=0; $d<$maxDances; $d++ ) {
		  $f = trim( $rows[$r]->$fields[$d] );
		  $len = mb_strlen( $f, "utf-8" );
	//      echo "$r,$d -> $len:'$f'<br>";
		  $lo[$d] = min( $lo[$d], $len );
		  $hi[$d] = max( $hi[$d], $len );
		}
	  }

	  for( $d=0; $d<$maxDances; $d++ ) {
		if( $hi[$d]> 0 ) {
		  $nDances = $d+1;
		}
	  }
	  
  }
  else {
	$nDances = 0;
  }
  
  if( $nDances != $nOfficialDances ) {
	$err .= $spacer."<font color=red>Не сходится количество танцев</font><br>";
  }
  
  if( ! $nDances ) {
	return $err;
  }
  
  $nJudges = $lo[0];
  
//  for( $d=0; $d<$nDances; $d++ ) {
  for( $d=0; $d<$nOfficialDances; $d++ ) {
    if( $lo[$d]!=$nJudges || $hi[$d]!=$nJudges ) {
      $d1 = $d+1;
      $err .=  $spacer."<font color=#800080>Разное количество оценок за танец $d1 ($dances[$d]): $lo[$d] - $hi[$d] / $nJudges</font><br>";
	  return $err;
    }
  }

  //----- end analyze lengths
  
  //----- собираем бегунки
  
  $begunki = array();
  
  for( $d=0; $d<$nOfficialDances; $d++ ) {
    for( $j=0; $j<$nJudges; $j++ ) {
      $begunok = "";
      for( $r=0; $r<count($rows); $r++ ) {
        $begunok .= mb_substr( $rows[$r]->$fields[$d], $j, 1, "utf-8" );
      }
      $begunki[] = "$begunok";
    }
  }
  //print_r( $begunki );
  
  //----- end собираем бегунки
  
  //----- проверяем совместимость оценок
  
  $derrs = array();
  
  if( isFinal( $begunki ) ) { // FINAL
    //echo "final<br>";
    //return null;
    for( $k=0; $k<count($begunki); $k++ ) {
      $begunok = $begunki[$k];
      if( ! goodFinalBegunok($begunok) ) {
        $j = $k % $nJudges;
        $d = ($k-$j) / $nJudges;
        $j1 = $j+1;
        $d1 = $d+1;
		$derrs[$d1][] = $j1;
        //$err .= $spacer."Ошибка(финал): танец $d1($dances[$d]) судья $j1<br>";
      }
    }
    //return $err;
  }
  else { // CROSSES
    $ns = array();
    for( $k=0; $k<count($begunki); $k++ ) {
      $ns[$k] = countCrosses( $begunki[$k] );
    }
 
    $avg = round( array_avg($ns) );

    for( $k=0; $k<count($ns); $k++ ) {
      if( $ns[$k] != $avg ) {
        $j = $k % $nJudges;
        $d = ($k-$j) / $nJudges;
        $j1 = $j+1;
        $d1 = $d+1;
		$derrs[$d1][] = $j1;
        //$err .= $spacer."Ошибка(кресты): танец $d1($dances[$d]) судья $j1<br>";
      }
    }
    //return $err;
  }
  
  if( count($derrs) ) {
	foreach( $derrs as $d1=>$js ) {
		$d = $d1-1;
        $error = "Ошибка в оценках за танец $d1 ($dances[$d]) "
				. ( (count($js)>1) ? "судьи " : "судья " )
				. implode(",",$js);
		
        $err .= $spacer."<font color=blue>$error</font><br>";
		//print_r( $derrs );
	}
  }
  
  //----- end проверяем совместимость оценок
  
  //----- проверяем дубли оценок
  
  if( count($rows) > 3 )
  {
  
	$flags = array();
  
	for( $d=0; $d<$nOfficialDances; $d++ ) {
		for( $e=$d+1; $e<$nOfficialDances; $e++ ) {
			if( ! isset($flags[$e]) )
			if( scoresEqual($rows,$fields[$d],$fields[$e]) ) {
				$flags[$e] = 1;
				$d1 = $d+1;
				$e1 = $e+1;
				$err .= $spacer."<font color=green>Одинаковые оценки за танцы $d1 ($dances[$d]) и $e1 ($dances[$e])</font><br>";
			}
		}
	}
	
  }
  
  //----- end проверяем дубли оценок
  
	return $err;
}

//---------------------

function scoresEqual( $rows, $f1, $f2 ) {
	foreach( $rows as $row ) {
		if( $row->$f1 != $row->$f2 ) {
			return false;
		}
	}
	return true;
}

//---------------------

function array_avg( $arr ) {
  $sum = 0.0;
  for( $k=0; $k<count($arr); $k++ ) {
    $sum += $arr[$k];
  }
  return $sum/count($arr);
}

//---------------------

function countCrosses( $begunok ) {
  $n = 0;
  for( $k=0; $k<mb_strlen($begunok,"utf-8"); $k++ ) {
    if( mb_substr( $begunok, $k, 1, "utf-8" ) == "1" ) {
      $n ++;
    }
  }
  return $n;
}

//---------------------

function goodFinalBegunok( $begunok ) {
  
  $scores = array();
  
  for( $k=0; $k<mb_strlen($begunok,"utf-8"); $k++ ) {
    $scores[$k] = mb_substr( $begunok, $k, 1, "utf-8" );
  }
  
  sort( $scores );
  
  for( $k=0; $k<mb_strlen($begunok,"utf-8"); $k++ ) {
    if( $scores[$k] != $k+1 )
      return false;
  }
  return true;
}

//---------------------

function isFinal( $begunki ) {
  for( $k=0; $k<count($begunki); $k++ ) {
    $s = $begunki[$k];
    if( is_int(strpos($s,"2"))) return true;
    if( is_int(strpos($s,"3"))) return true;
    if( is_int(strpos($s,"4"))) return true;
    if( is_int(strpos($s,"5"))) return true;
    if( is_int(strpos($s,"6"))) return true;
    if( is_int(strpos($s,"7"))) return true;
    if( is_int(strpos($s,"8"))) return true;
    if( is_int(strpos($s,"9"))) return true;
/*    $s = str_replace( "1", "", $s );
    $s = str_replace( "0", "", $s );
    $s = str_replace( "x", "", $s );
    $s = str_replace( "-", "", $s );
    if( mb_strlen($s,"utf-8") ) {
      return true;
    }*/
  }
  return false;
}

//=====================
?>
