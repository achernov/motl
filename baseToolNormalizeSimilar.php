<?php 
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

//=====================


function commonLen(  $a1, $a2  ) {

  //----- Build Crosses
  
  $crosses = array();
  $n1 = count($a1);
  $n2 = count($a2);
/*
  for( $i=0; $i<$n1; $i++ ) {
    $ch1 = $a1[$i];
    
    for( $k=0; $k<$n2; $k++ ) {
      $ch2 = $a2[$k];
      
      if( $ch1 == $ch2 ) {
        $crosses[] = array( $i,$k, 0 );
      }
    }
  }
*/
//return 0;
  $ar = array();
  for( $k=0; $k<$n2; $k++ ) {
    $ch2 = $a2[$k];
    if( isset($ar[$ch2]) ) {
      $ar[$ch2][] = $k;
    }
    else {
      $ar[$ch2] = array($k);
    }
  }
//return 0;  
  for( $i=0; $i<$n1; $i++ ) {
    $ch1 = $a1[$i];
  
    if( isset($ar[$ch1]) ) {
      $arch = $ar[$ch1];
      for( $j=0; $j<count($arch); $j++ ) {
        $crosses[] = array( $i,$arch[$j], 0 );
      }
    }
  }

  
//return 0;
  //----- Measure Crosses
  for( $k=0; $k<count($crosses); $k++ ) {
    $maxUnder = 0;
    for( $i=0; $i<$k; $i++ ) {
      if( $crosses[$i][0]<$crosses[$k][0] && $crosses[$i][1]<$crosses[$k][1]  ) {
        $maxUnder = max( $maxUnder, $crosses[$i][2] );
      }
    }
    $crosses[$k][2] = $maxUnder+1;
  }
    
  //----- Collect Crosses
  
  $max = 0;
  for( $k=0; $k<count($crosses); $k++ ) {
    $max  = max( $max, $crosses[$k][2] );
  }
  /*
  print_r($a1);
  print_r($a2);
  print_r($crosses);
  echo "$max<hr>";*/
  return $max;
}

//=====================

function bk( $a1, $a2 ) { // Byki & Korovy ;-)
	
	$dif = array();
	
	foreach( $a1 as $c ) {
		if( ! isset($dif[$c]) ) { 
			$dif[$c] = 0;
		}
		$dif[$c] --;
	}
	
	foreach( $a2 as $c ) {
		if( ! isset($dif[$c]) ) { 
			$dif[$c] = 0;
		}
		$dif[$c] ++;
	}

	$sum = 0;
	foreach( $dif as $d ) {
		$sum += abs($d);
	}

	return $sum;
}

//=====================

function strDist( $a1, $a2 ) {
	if( bk($a1,$a2) > 4 ) { return 100;	}
	$l = commonLen( $a1, $a2 );
	$dist = max(count($a1),count($a2))-$l;
	return $dist;
}

//=====================

function str2array( $s ) {
  $a = array();
  $n = mb_strlen($s,"utf-8");
  
  for( $i=0; $i<$n; $i++ ) {
    $a[$i] = mb_substr($s,$i,1,"utf-8");
  }
  return $a;
}

//=====================

$timerStartTime = microtime(true);
//print_r($timerStartTime);

function printTime() {
	global $timerStartTime;
	$t = microtime(true) - $timerStartTime;
	echo "T = $t<br>";
}

//=====================

MotlDB::connect();

$type = Motl::getParameter("type");
motlASSERT( $type, in_array($type,array("country","city","club","person")) );

  echo "<A href='base.php?id=tools'>назад</a><br>";

//--------------------- ADD EXCLUSIONS

//print_r( $_REQUEST );
foreach( $_REQUEST as $k=>$v ) {
  $ar = explode( "_", $k );
  if( $ar[0] == "chk" ) {
    $id1 = (int) $ar[1];
    $id2 = (int) $ar[2];
    
    // ASS $type
    // COD $id1, $id2
    MotlDB::query( "INSERT INTO db_ok_similar(type,id1,id2) VALUES('$type','$id1','$id2')" );
  }
}

//--------------------- LOAD EXCLUSIONS

// ASS $type
$exclRows = MotlDB::queryRows( "SELECT id1,id2 FROM db_ok_similar WHERE type='$type'" ); 

$exclusions = array();
foreach( $exclRows as $k=>$row ) {
  $exclusions[$row->id1][$row->id2] = 1;
}

//---------------------

switch( $type ) {
  case 'person':
    $rows = MotlDB::queryRows( "SELECT id, surname as title, name as subtitle "
                              ."FROM db_person ORDER BY surname,name" );
    break;
    
  default:
    // ASS $type
    $rows = MotlDB::queryRows( "SELECT id, name as title FROM db_$type ORDER BY name" );
    break;
}

//$anames = array();
for( $i=0; $i<count($rows); $i++ ) {
  //$anames[$i] = str2array( $names[$i] );
  $rows[$i]->aname = str2array( $rows[$i]->title );
}

$codes = array( "country"=>"s", "city"=>"g", "club"=>"k", "person"=>"p" );
$code = $codes[$type];

echo "<form>\n";
echo "<table cellspacing=0 cellpadding=5 border=1>\n";

for( $i=0; $i<count($rows); $i++ ) {

  set_time_limit(30);

  for( $j=0; $j<$i; $j++ ) {
    
    $dist = strDist( $rows[$i]->aname, $rows[$j]->aname );

	  if( $dist <= 2 ) {
    
      $label1 = $rows[$i]->title;
      $label2 = $rows[$j]->title;
      $score  = $dist;

      $id1 = $rows[$i]->id;
      $id2 = $rows[$j]->id;
      
      if( $type == 'person' ) {
        $s1 = $rows[$i]->subtitle;
        $s2 = $rows[$j]->subtitle;
        $a1 = str2array( $s1 );
        $a2 = str2array( $s2 );
        
        $subdist = strDist( $a1, $a2 );
        if( $subdist > 2 ) {
          continue;
        }
        else {
          $label1 .= " $s1";
          $label2 .= " $s2";
          $score .= " $subdist";
          
          $mergeUrl = "baseTools.php?dbf_id[]=$id1&dbf_id[]=$id2&dbf_0_check&dbf_1_check&mergePersons";
          $merge = "<A href='$mergeUrl'>merge</a>";
          $score .= " $merge";
        }
      }

      if( isset($exclusions[$id1]) && isset($exclusions[$id1][$id2]) ) continue;

  
      $label1 = "<A href='db.php?id=$code$id1'>$label1</a>";
      $label2 = "<A href='db.php?id=$code$id2'>$label2</a>";
//      echo "$type,$id1,$id2<br>";
      echo "<tr> <td>$label1</td> <td>$label2</td> <td>$score</td> "
               ."<td><input type=checkbox name=chk_".$id1."_".$id2."></td> </tr>\n";
      flush();
    }
  }
}
echo "</table>\n";
echo "<input type=hidden name='type' value='$type'>";
echo "<input type=submit value='Mark selected as DIFFERENT'>";
echo "</form>\n";
//	printTime();

?>
