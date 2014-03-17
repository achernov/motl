<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "dbAll.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

$baseDate = "2011-09-01";

//=====================

if( isset($_REQUEST["code"]) ) {
  $code = Motl::getParameter("code");
  
  if( isset($_REQUEST["title"]) ) {
    echo "<A href='baseToolCheckGroups.php?code=$code'>назад</a><br>";
	
	$title = Motl::getParameter("title");
    
    if( Motl::getParameter("edit") ) {
      echo editGroupTitleCompetitions( $code, $title );
    }
    else
    if( Motl::getParameter("save") ) {
      echo saveGroupTitleCompetitions( $code, $title );
    }
    else {
      echo showGroupTitleCompetitions( $code, $title );
    }
  }
  else {
    echo "<A href='baseToolCheckGroups.php'>назад</a><br>";
    echo showGroupTitles( $code );
  }
}
else
if( Motl::getParameter("mark") ) {
	setConfirm( 1 );
	echo showGroups( 0 );
}
else
if( Motl::getParameter("unmark") ) {
	setConfirm( 0 );
	echo showGroups( 1 );
}
else {
  echo "<A href='base.php?id=tools'>назад</a><br>";
  //echo showGroupsOverview();
  echo showGroups( Motl::getParameter("confirmed") );
}

//echo btcgGroupCmpPart( array("e","16_","la"), array("e","16_","st"), 2 );
$a->gc = "e.16_.st";
$b->gc = "e.16_.la";
//echo btcgGroupCmp( $a, $b );

//=====================

function makeTab( $selected, $url, $title ) {
	return $selected ? "<b>$title</b>"
					 : "<A href='$url'>$title</a>";
}

//=====================

function setConfirm( $confirmed ) {
	for( $k=0; $k<10000; $k++ ) {
		if( isset($_REQUEST["chk$k"]) )	{
			$code  = $_REQUEST["code$k"];
			$title = $_REQUEST["title$k"];
			setConfirmOne( $code, $title, $confirmed );
			//echo "$code $title<br>";
		}
	}
}

//=====================

function setConfirmOne( $code, $title, $confirmed ) {

	$eCode = MRES($code);
	$eTitle = MRES($title);

	if( $confirmed ) {
		$sql = "INSERT INTO confirmation(property,value,date) "
				."VALUES( 'groupcodetitle.$eCode', '$eTitle', now() )";
	}
	else{
		$sql = "DELETE FROM confirmation "
				."WHERE property='groupcodetitle.$eCode' AND value='$eTitle'";
	}
	//die($sql);
	
	MotlDB::query($sql);
}

//=====================

function showGroups( $confirmed=false ) {
global $baseDate;
//print_r($_REQUEST);
	$script = $_SERVER['PHP_SELF'];

	$s = "";

	$s .= "<p>";
	$s .= makeTab( !$confirmed, "$script?confirmed=0", "Неподтвержденные" );
	$s .= "&nbsp;";
	$s .= makeTab( $confirmed,  "$script?confirmed=1", "Подтвержденные" );
	$s .= "<p>";
	
	$confCond = $confirmed ? "c.id Is Not Null" : "c.id Is Null";
	
	$sql = "SELECT comp.title, comp.groupcode, count(*) AS N "
			."FROM db_events AS comp "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
			."LEFT JOIN confirmation AS c "
				."ON CONCAT('groupcodetitle.',comp.groupcode)=c.property "
					."AND comp.title=c.value "
			."WHERE comp.type='competition' "
//			."AND ts_created>'2010-09-01' "
			."AND event.date>'$baseDate' "
			."AND $confCond "
//			."AND groupcode Like '%.10' "
//			."AND groupcode Like '%.la' "
//			."AND groupcode Like '%.st' "
			."GROUP BY title, groupcode "
//			."ORDER BY title";
			."ORDER BY groupcode, title";
//die($sql);
	$rows = MotlDB::queryRows( $sql );
	
	
	$s .= "<form action='$script' method=POST>";
	
	$s .= "<table border=1>";
	foreach( $rows as $k=>$row ) {
		$escCode  = htmlspecialchars($row->groupcode,ENT_QUOTES,"UTF-8");
		$escTitle = htmlspecialchars($row->title,ENT_QUOTES,"UTF-8");
		$s .= "<tr>";
		$s .= "<td><input type=checkbox name=chk$k></td>";
		$s .= "<td>$row->N</td>";
		$s .= "<td>$row->groupcode&nbsp;</td>";
		$ccode = urlencode($row->groupcode);
		$ctitle = urlencode($row->title);
		$url ="$script?code=$ccode&title=$ctitle";
		$s .= "<td><a href='$url'>$row->title</a></td>";
		$s .= "</tr>";
		$s .= "<input type=hidden name=code$k  value='$escCode'>";
		$s .= "<input type=hidden name=title$k value='$escTitle'>";
	}
	$s .= "</table><p>";

	if( $confirmed ) {
		$s .= "<input type=submit name='unmark' value='Unconfirm selected'>";
	}
	else {
		$s .= "<input type=submit name='mark' value='Confirm selected'>";
	}
	
	$s .= "</form>";
	
	//print_r($rows);
	return $s;
}

//=====================

function getStartDateParameter() {
global $baseDate;
	return $baseDate;
	$startDate = Motl::getParameter( "date0" );
	if( ! $startDate ) $startDate = "2000-01-01";
	return $startDate;
}

//=====================

function getEventSidsByStartDate( $startDate ) {
	$sql = "SELECT id FROM db_events WHERE type='event' AND date>='$startDate'";
	$eventIds = MotlDB::queryValues( $sql );
	return implode( ",", $eventIds );
}

//=====================

function showGroupsOverview() {

	$startDate = getStartDateParameter();
	
	// UNSAFE $startDate
	$sql = "SELECT DISTINCT date FROM db_events "
			."WHERE type='event' AND date<Now() ORDER BY date DESC";
	//die($sql);
  $dates = MotlDB::queryValues( $sql );
  
	$dateOptions = "";
	foreach( $dates as $d ) {
		//$hd = DataUtil::dateDbToHumanNoTime($d);
		//echo "$d $hd<br>";
		$sel = ($d==$startDate) ? " selected" : "";
		$dateOptions .= "<option value='$d'$sel>$d";
	}
	$dateSelector = "<select onchange='onSelDate(this)'>$dateOptions</select>";

	$eventSids = getEventSidsByStartDate( $startDate );
	
	// CODFLD $eventSids
	$sql = "SELECT groupCode AS gc, count(*) as N FROM db_events "
                                ."WHERE pid In($eventSids) AND type='competition' "
                                //."AND ncouples>0 "
                                ."GROUP BY gc";
	
  $codeRows = MotlDB::queryRows( $sql );
    
  $asolo = array('s','sn3','sn4','sn5');
  $solo    = filterAndSort( $codeRows, $asolo );
  
  $aclasses = array('n3','n4','n5','n','e','d','c','ba');
  $classes = filterAndSort( $codeRows, $aclasses );
  
  $aages = array('juv','juv1','juv2','jun','jun1','jun2','you','you1','you2','youam','am','sen');
  $ages    = filterAndSort( $codeRows, $aages );
  
//  $rest    = filterNotAndSort( $codeRows, array('sn3','sn4','sn5','n3','n4','n5','n','e','d','c','ba','juv','jun','jun1','jun2','you','you1','you2','youam','am') );
  $rest    = filterNotAndSort( $codeRows, array_merge($asolo,$aclasses,$aages) );
  
  $s = "";
  
  $s .= "<p><b>Начиная с даты: $dateSelector</b><p>";
  
  $s .= "<p><b>Солисты</b><br>";
  $s .= listGroups( $solo, $startDate );
  $s .= "<p><b>Классы</b><br>";
  $s .= listGroups( $classes, $startDate );
  $s .= "<p><b>Возраста</b><br>";
  $s .= listGroups( $ages, $startDate );
  $s .= "<p><b>Прочие</b><br>";
  $s .= listGroups( $rest, $startDate );
  
  //print_r( $codeRows );
  return $s;
}

//=====================

function showGroupTitles( $groupCode ) {

	$startDate = getStartDateParameter();
	$eventSids = getEventSidsByStartDate( $startDate );
  
  $cond = $groupCode ? "(groupCode='".MRES($groupCode)."')" : "(groupCode Is Null Or groupCode='')";
  $compRows = MotlDB::queryRows( "SELECT title,count(*) AS N FROM db_events "
                                ."WHERE pid in($eventSids) AND (type='competition') and $cond GROUP BY title ORDER BY title" );
  
  $s = "";
  
  $s .= "<p><b>groupCode = '$groupCode'</b><p>";

  $s .= "<p><b style='color:#0000a0;font-size:15px;font-family:arial'>Названия с таким кодом:</b><p>";
  
  $prototype = new DbEventProducer();
  
  foreach( $compRows as $row ) {
    $title = $row->title;
    $n     = $row->N;
    $ns = " <sub style='color:green'>$n</sub>";
    $ctitle = urlencode($title);
    $edlink = "<A class='editLink' href='baseToolCheckGroups.php?code=$groupCode&title=$ctitle&edit=1'>edit</a>";
    $s .= "$edlink <A href='baseToolCheckGroups.php?code=$groupCode&date0=$startDate&&title=$title'>$title</a> <b>$ns</b><br>";
  }
  return $s;
}

//=====================

function showGroupTitleCompetitions( $groupCode, $title ) {
	$startDate = getStartDateParameter();
	$eventSids = getEventSidsByStartDate( $startDate );

  $groupCond = $groupCode ? "groupCode='".MRES($groupCode)."'" : "(groupCode Is Null Or groupCode='')";
  $titleCond = $title     ? "title='".MRES($title)."'" : "(title Is Null Or title='')";
  
  $sql = "SELECT id,title FROM db_events "
		."WHERE pid in($eventSids) AND $groupCond And $titleCond ORDER BY title";
  
  $compRows = MotlDB::queryRows( $sql );
  
  $s = "";
  
  $s .= "<p><b>groupCode = '$groupCode', &nbsp; title = '$title'</b><p>";
  
  $s .= "<p><b style='color:#0000a0;font-size:15px;font-family:arial'>Соревнования с таким кодом и названием:</b><p>";
  
  $prototype = new DbEventProducer();
  
  foreach( $compRows as $row ) {
    $id    = $row->id;
//    $date = $row->date;
    $title = $row->title;
    $edlink = $prototype->makeEditLink( "edit", $id );
    $s .= "$edlink <A href='db.php?id=e$id'>$title<br>";
  }
  return $s;
}

//=====================

function editGroupTitleCompetitions( $groupCode, $title ) {

  $groupCond = $groupCode ? "groupCode='$groupCode'" : "(groupCode Is Null Or groupCode='')";
  $titleCond = $title     ? "title='$title'" : "(title Is Null Or title='')";

  $s = "";
  
  $s .= "<p>";
  
  $s .= "<form action='baseToolCheckGroups.php' method=POST>";
  
  $s .= "<input type='hidden' name='code' value='$groupCode'>";
  $s .= "<input type='hidden' name='title' value='$title'>";
  
  $s .= "<table>\n";
  $s .= "<tr><td>groupCode</td><td> <input style='width:200' type='text' name='newCode' value='$groupCode'> </td></tr>\n";
  $s .= "<tr><td>title</td><td> <input style='width:800' type='text' name='newTitle' value='$title'></td></tr>\n";
  $s .= "</table>\n";
  
  $s .= "<p><input type='submit' name='save' value='Save'>";
  
  $s .= "</form>";
  
  return $s;
}

//=====================

function saveGroupTitleCompetitions( $groupCode, $title ) {

  $groupCond = $groupCode ? "groupCode='".MRES($groupCode)."'" : "(groupCode Is Null Or groupCode='')";
  $titleCond = $title     ? "title='".MRES($title)."'" : "(title Is Null Or title='')";
  
  $newCode  = Motl::getParameter( "newCode" );
  $newTitle = Motl::getParameter( "newTitle" );

  $sql = "UPDATE db_events SET groupCode='".MRES($newCode)."', title='".MRES($newTitle)."' "
        ."WHERE type='competition' AND $groupCond AND $titleCond";
        
  echo "$sql";
  MotlDB::query( $sql );
  
  Motl::redirect( "baseToolCheckGroups.php?code=$groupCode" );
}

//=====================

function listGroups( $ar, $startDate ) {
//print_r($ar);
  $s = "";
  $s .= "<table>";
  for( $k=0; $k<count($ar); $k++ ) {
    $br = $ar[$k];
    $s .= "<tr>";
    for( $i=0; $i<count($br); $i++ ) {
      $code = $br[$i]->gc;
      $n    = $br[$i]->N;
      $ns = " <sub style='color:green'>$n</sub>";
      //$ns = "";
      $codeV = $code ? $code : "&lt;&lt;empty&gt;&gt;";
      $s .= "<td><A target='_new' style='color:#000080' href=baseToolCheckGroups.php?code=$code&date0=$startDate><b>$codeV</b></a>$ns &nbsp; &nbsp; </td>";
    }
    $s .= "</tr>";
  }
  $s .= "</table>";
  return $s;
}

//=====================

function filterAndSort( $rows, $starts ) {
  
  $tmp = array();
  
  foreach( $rows as $row ) {
    $code = $row->gc;
    $acode = explode( ".", $code );
    if( in_array($acode[0],$starts) ) {
      $pos = array_search( $acode[0], $starts );
      $tmp[$pos][] = $row;
    }
  }
  
  foreach( $tmp as $k=>$v ) {
    usort( $tmp[$k], "btcgGroupCmp" );
  }
  
  $out = array();
  for( $k=0; $k<count($starts); $k++ ) {
    if( isset($tmp[$k]) ) {
      $out[] = $tmp[$k];
    }
  }
  return $out;
}

//=====================

function filterNotAndSort( $rows, $starts ) {
  
  $tmp = array();
  
  foreach( $rows as $row ) {
    $code = $row->gc;
    $acode = explode( ".", $code );
    if( ! in_array($acode[0],$starts) ) {
      $tmp[0][] = $row;
    }
  }
  
  foreach( $tmp as $k=>$v ) {
    usort( $tmp[$k], "btcgGroupCmp" );
  }
  
  $out = array();
  for( $k=0; $k<count($starts); $k++ ) {
    if( isset($tmp[$k]) ) {
      $out[] = $tmp[$k];
    }
  }
  return $out;
}

//=====================

function btcgGroupCmp( $a, $b ) {
  $as = explode( ".", $a->gc );
  $bs = explode( ".", $b->gc );
  
  $res = btcgGroupCmpPart($as,$bs,1);
  if( $res ) return $res;
  
  $res = btcgGroupCmpPart($as,$bs,2);
  if( $res ) return $res;
  die( "QQQ" );
}

//---------------------

function btcgGroupCmpPart( $as, $bs, $index ) {
  if( ! isset($as[$index]) )  return -1;  // $as is shorter
  if( ! isset($bs[$index]) )  return  1;  // $bs is shorter
  
  $a = $as[$index];
  $b = $bs[$index];
  
  $av = btcgPartWeight( $a );
  $bv = btcgPartWeight( $b );
  
  if( $av != $bv ) {
    return ($av>$bv) ? 1 : -1;
    //return $av - $bv;
  }
  else {
    return strcmp( $a, $b );
  }
}

//---------------------

function btcgPartWeight( $s ) {
  switch( $s ) {
    case "": return 0;
    case "st": return 0.1;
    case "la": return 0.2;
    case "10": return 0.3;
  }
  
  if( is_int(strpos($s,"_")) ) {
    $a = explode( "_", $s );
    $v = strlen($a[0]) ? 10*(int)$a[0] : (int)$a[1];
    return $v;
  }
  
  return 1000;
}

//=====================
?>
<script>

function onSelDate( sel ) {
	var newDate = sel.options[sel.selectedIndex].value;
	location.replace( "baseToolCheckGroups.php?date0="+newDate );
}

</script>
