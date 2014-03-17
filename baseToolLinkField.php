<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseColumn.inc";
require_once "subjectUtils.inc";
require_once "dbAll.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

$baseDate = "2012-01-01";

switch( Motl::getParameter("subject") ) {
	case 'solo':   $subject = new SubjectSolo(); break;
	case 'couple': $subject = new SubjectCouple(); break;
}


if( Motl::getParameter("action") == "select" ) {
	echo makeSelectForm( $subject );
}
else
if( Motl::getParameter("action") == "doselect" ) {
	
	doSelect( $subject );
	
	$script = $_SERVER['PHP_SELF'];
	$subjCode = Motl::getParameter("subject");
	$id = Motl::getParameter("id");
	$field = Motl::getParameter("field");
	Motl::redirect( "$script?action=select&subject=$subjCode&id=$id&field=$field" );
	//echo makeSelectForm( $subject );
}
else{
	echo makeGlobalForm( $subject );
}

//=====================

function doSelect( $subject ) {
	//print_r( $_REQUEST );
	$cmdS = Motl::getParameter("cmdS");
	$cmdC = Motl::getParameter("cmdC");
	$cmdSC = Motl::getParameter("cmdSC");
	$id  = Motl::getParameter("id");
	$sel = Motl::getParameter("sel");
	$field = Motl::getParameter("field");
	$subjectCode = $subject->getName();
	$tableName = $subject->getTableName();
	
	
	if( $sel < 0 ) {
	
		$sql = "SELECT $field FROM $tableName WHERE id=$id";
		$value = MotlDB::queryValue($sql);
		
		$sql = "SELECT Now()";
		$date = MotlDB::queryValue($sql);
		
	}
	else 
	if( $sel > 0 ) {
	
		$sql = "SELECT $field FROM db_couple_event WHERE id=$sel";
		$value = MotlDB::queryValue($sql);
		
		$sql = "SELECT event.date "
			."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
			."WHERE ce.id='$sel'";
		$date = MotlDB::queryValue($sql);
	}
	else {
		echo "Nothing selected<br>";
		return;
	}
	
	if( $cmdS || $cmdSC ) {
		//echo "SET id=$id, $field=$value<p>";
		$sql = "UPDATE $tableName SET $field='$value' WHERE id=$id";
		MotlDB::query($sql);
	}
	
	if( $cmdC || $cmdSC ) {
//		echo "CONFIRM id=$id, $field=$value, date=$date<p>";
		$sql = "INSERT INTO confirmation(property,value,date) "
			."VALUES('$subjectCode.$id.$field','$value','$date')";
		MotlDB::query($sql);
	}
	
	
	
	//echo "<p>value=$value<p>";
	//echo "<p>cmdS=$cmdS<br>cmdC=$cmdC<br>cmdSC=$cmdSC<br>id=$id<br>sel=$sel<p>";
}

//=====================

function makeSelectForm( $subject ) {
global $baseDate;
	$script = $_SERVER['PHP_SELF'];

	$id = Motl::getParameter("id");
	$field = Motl::getParameter("field");
	$column = BaseColumn::createColumn( $field );

	$subjectCode = $subject->getName();
	$tableName = $subject->getTableName();
	$idField = $subject->getLinkField();
	$letter = $subject->getLetter();
	
	$fieldName = $column->getName();
	$fieldLabel = $column->getRusName();
	$htmlName = $column->getHtmlFieldName();
	
	$sql = $subject->getDataQuery() . " WHERE subj.id=$id";
	$rows = MotlDB::queryRows( $sql );
	$subjRow = $rows[0];
	SubjectUtils::addCCCTtoRows( $rows );
	//print_r($subjRow);
	
	$subjTitle = $subjRow->title;
	$subjUrl = "db.php?id=$letter$id";
	
	$s = "";
	
	$s .= "<A href='$script?subject=$subjectCode'>назад</a><p>";
	
	$s .= "<form action='$script'>";
	$s .= "<input type='hidden' name='action' value='doselect'>";
	$s .= "<input type='hidden' name='subject' value='$subjectCode'>";
	$s .= "<input type='hidden' name='id' value='$id'>";
	$s .= "<input type='hidden' name='field' value='$field'>";
	
	$s .= "<big><a href='$subjUrl'>$subjTitle</a></big><p>";
	$s .= "<input type='radio' name='sel' value='-1'><b>$fieldLabel:</b> ". $subjRow->$htmlName;
	$s .= "<hr width=300 align=left>";
	$s .= "<input type='submit' name='cmdS'  value='set'> &nbsp; ";
	$s .= "<input type='submit' name='cmdC'  value='confirm'> &nbsp; ";
	$s .= "<input type='submit' name='cmdSC' value='set &amp; confirm'> &nbsp; ";
	$s .= "<hr width=300 align=left>";
	
	$sqlConf = "SELECT 'confirm' AS type, id, 0 AS cid, 0 AS eid, "
			."value AS $fieldName, date "
			."FROM confirmation WHERE property='$subjectCode.$id.$fieldName'";
//die($sql);	
	
	$sqlRef = "SELECT 'reg' AS type, ce.id,comp.id AS cid, event.id AS eid, "
				."$fieldName, event.date "
			."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
			."WHERE $idField='$id' AND event.type='event' ";

	$sql = "SELECT type,id,cid,eid,$fieldName,date, "
				."(date>='$baseDate') AS isModern "
			."FROM ($sqlConf UNION $sqlRef) Q "
//			."WHERE $idField='$id' AND event.type='event' "
//				." AND event.date>='$baseDate'"
			."ORDER BY date DESC"
			."";
	//die($sql);
	$rows = MotlDB::queryRows( $sql );
	SubjectUtils::addCCCTtoRows( $rows );
	
	$tbl = "";
	$tbl .= "<table cellspacing=0 cellpadding=4>";
	foreach( $rows as $row ) {
		$varTitle = $row->$htmlName;
		$type =  $row->type;
		$date =  $row->date;
		$ceid =  $row->id;
		$eid =  $row->eid;
		$cid =  $row->cid;
		$url = "db.php?id=e$eid,r$cid&hc=$id";
		$style = $row->isModern ? "font-weight:bold" : "font-style:italic";
		if( $type == 'confirm' ) {
			$style .= ';background-color:#c0ffc0';
		}
		$tbl .= "<tr style='$style'>";
		$tbl .= "<td><a href='$url'>$date</a></td>";
		if( $type != 'confirm' ) {
			$obj = new DbCoupleEventProducer();
			$editLink = $obj->makeEditLink("edit",$ceid);
			$tbl .= "<td><input type='radio' name='sel' value='$ceid'></td>";
		}
		else {
			$tbl .= "<td>&nbsp;</td>";
			$editLink = "";
		}
		$tbl .= "<td>$editLink</td>";
		$tbl .= "<td>$varTitle</td>";
		$tbl .= "</tr>";
	}
	$tbl .= "</table>";
//print_r($rows);

	$s .= $tbl;
	
	$s .= "</form>";

	die($s);
}

//=====================

function makeGlobalForm( $subject ) {
	
	$subjectCode = $subject->getName();
	$tableName = $subject->getTableName();
	$idField = $subject->getLinkField();

	$dataQuery = $subject->getDataQuery();
	$sql = $dataQuery . " ORDER BY title";//." LIMIT 10";
//die($sql);
	$subjRows = MotlDB::queryRows( $sql );
	SubjectUtils::addCCCTtoRows( $subjRows );
//	print_r( $subjRows );
	
	$bcCountry = new BaseColumnCountry();
	$bcCity = new BaseColumnCity();
	$bcClub = new BaseColumnClub();
	$bcTeacher = new BaseColumnTeacher();
	$columns = array( $bcCountry, $bcCity, $bcClub, $bcTeacher );

	echo "<A href='base.php?id=tools'>назад</a><p>";

	echo "<table border=0>";
	$code = $subject->getLetter();
	$nRows = 0;
	foreach( $subjRows as $row ) {
	
		$subjText = "";
		$useSubj = false;
		
		$title = "<A href='db.php?id=$code$row->id'>$row->title</a>";
		
		$subjText .= "<tr>";
		$subjText .= "<td colspan=4><p>&nbsp;</p><b>$title</b></td>";
		$subjText .= "</tr>";
		
		$subjText .= "<tr valign=top>";
		foreach( $columns as $col ) {
			$value = getFieldValue( $row, $col );
			if( testVariants( $row, $col, $subject ) ) {
				$useSubj = true;
				$value .= "<br>" . makeVariantsLink( $row, $col, $subject );
			}
			$cell = "<td width=300>$value</td>";

			$subjText .= $cell;
		}
		$subjText .= "</tr>";
		
		if( $useSubj ) {
			$nRows ++;
			echo $subjText;
		}
	}
	echo "</table>";
	
	echo "Всего: $nRows";
}

//=====================

function getFieldValue( $row, $column ) {
	$tfname = $column->getHtmlFieldName();
	return $row->$tfname;
}

//=====================

function testVariants( $row, $column, $subject ) {
global $baseDate;
	$id = $row->id;
	$fieldName = $column->getName();
	$tableName = $subject->getTableName();
	$idField = $subject->getLinkField();
	$subjectCode = $subject->getName();
	$value = $row->$fieldName;
	//$value='';
//$id=907;$fieldName='teacher';
	
	$sql = "SELECT date "
			."FROM confirmation "
			."WHERE property='$subjectCode.$id.$fieldName' "
			."ORDER BY date DESC "
			."LIMIT 1";
//die($sql);
	$confirmDate = MotlDB::queryValue($sql);

	$startDate = $confirmDate ? $confirmDate : $baseDate;
	
	if( strlen(trim($value)) ) {
		$sql = "SELECT count(DISTINCT $fieldName) "
				."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
				."WHERE $idField=$id AND $fieldName<>'$value' "
				." AND event.date>'$startDate'";
		//		die($sql);
		$n = MotlDB::queryValue($sql);
		//echo "$sql<br>$n<hr>";
		return $n>0;
	}
	else {
		$sql = "SELECT count(DISTINCT $fieldName) "
				."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
				."WHERE $idField=$id AND $fieldName<>'' "
				." AND event.date>'$startDate'";
		$n = MotlDB::queryValue($sql);
		//echo "$sql<br>$n<hr>";
		if( $n > 1 ) {
			return true;
		}
		else 
		if( $n == 1 ) {
			$sql = "SELECT $fieldName "
				."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
				."WHERE $idField=$id AND $fieldName<>'' "
				." AND event.date>'$startDate'"
				."LIMIT 1";
			$value = MotlDB::queryValue($sql);
			
			$sql = "UPDATE $tableName SET $fieldName='$value' WHERE id=$id";
			//echo "$sql<br>";
			MotlDB::query($sql);
			return false;
		}
	}
}

//=====================

function makeVariantsLink( $row, $column, $subject ) {
	$script = $_SERVER['PHP_SELF'];
	$id = $row->id;
	$name = $column->getName();
	$subjectCode = $subject->getName();
	$style = "color:red;";
	$url = "$script?action=select&subject=$subjectCode&id=$id&field=$name";
	return "<a style='$style' href='$url'>варианты</a>";
}

//=====================

function makeVariants( $id, $idField, $prop, $tprop ) {
	$tprop0 = $tprop."0";
	
	$sql = "SELECT event.date, ce.$prop "
			."FROM db_couple_event AS ce "
			."INNER JOIN db_events AS comp ON ce.idEvent=comp.id "
			."INNER JOIN db_events AS event ON comp.pid=event.id "
			."WHERE ce.$idField=$id AND event.type='event' "
			."ORDER BY date DESC";
			
	$rows = MotlDB::queryRows( $sql );
	
	SubjectUtils::addCCCTtoRows( $rows );
	
	//print_r($rows);
	$s = "";
	foreach( $rows as $row ) {
		$date = $row->date;
		$text = $row->$tprop0;
		$s .= "<option>$date - $text";
	}
	return $s;
}

//=====================
?>
<script>

function doClick( field, theId, ids, names ) {
	ctrlNames = document.all["Names"+theId+field]
	ctrlIds   = document.all["Ids"+theId+field]
	
	ctrlNames.value = names
	ctrlIds.value = ids
	/*
	alert( field )
	alert( theId )
	alert( ids )
	alert( names )*/
}

</script>
