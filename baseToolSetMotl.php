<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "subjectUtils.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=========================================
//print_r( $_REQUEST ); echo "<p>";die();
//=========================================

$subject = new SubjectCouple();

if( isset($_REQUEST['invIds']) ) {
	invertByIds( $subject, $_REQUEST['invIds'] );
	Motl::redirect( $_SERVER['PHP_SELF'] );
}
echo makeForm( $subject );

function invertByIds( $subject, $ids ) {
	$tableName = $subject->getTableName();
	$sids = implode( ',', $ids );
	$sql = "SELECT id, motl, not ifnull(motl,0) "
		."FROM $tableName WHERE id in($sids)";
	$sql = "UPDATE $tableName "
		."SET motl = not ifnull(motl,0) "
		."WHERE id in($sids)";
	//die($sql);
	motlDB::query($sql);
}

function makeForm( $subject ) {
	$script = $_SERVER['PHP_SELF'];
	
	$subjectCode = $subject->getName();
	$tableName = $subject->getTableName();

	$sql = "SELECT DISTINCT subj.id FROM $tableName subj "
		."INNER JOIN db_link lnk ON subj.id=lnk.id1 "
		."INNER JOIN db_club club ON club.id=lnk.id2 "
		."WHERE lnk.tag='$subjectCode-club' AND ifnull(subj.motl,0)<>ifnull(club.motl,0)";
	//die($sql);
	$ids = MotlDB::queryValues( $sql );
	$sids = implode( ',', $ids );

	$s = "";

	$s .= "<form action='$script' methos='GET'>";
	
	$s .= "<table border=1>";

	$rowsAdd = loadByMotl( $subject, $sids, 0 );
	$s .= makeTableRows( $subject, $rowsAdd, "Добавить в МОТЛ" );

//	$rowsDel = loadByMotl( $subject, $sids, 1 );
//	$s .= makeTableRows( $subject, $rowsDel, "Убрать из МОТЛ" );

	$s .= "</table>";
	
	$s .= "<p>";
	
	$s .= "<input type=submit>";
	
	$s .= "</form>";
	
	return $s;
}

function makeTableRows( $subject, $rows, $title ) {
	$letter = $subject->getLetter();
	
	$s = "";
	$s .= "<tr> <td colspan=3 style='padding-top:10px'><big><b>$title</b></big></td> </tr>\r\n";
	foreach( $rows as $row ) {
		//print_r($row);
		$url = "db.php?id=$letter$row->id";
		$s .= "<tr> "
				."<td style='padding-top:5px'>"
					."<input type=checkbox name='invIds[]' "
							."value='$row->id'></td> "
				."<td><a href='$url'>$row->title</a></td> "
				."<td>$row->textClub</td> "
			."</tr>\r\n";
	}
	return $s;
}

function loadByMotl( $subject, $sids, $motlValue ) {
	$sql = $subject->getDataQuery() . " WHERE subj.id in($sids) "
				."AND ifnull(subj.motl,0)=$motlValue "
				."ORDER BY title";
	$rows = MotlDB::queryRows($sql);
	
	SubjectUtils::addCCCTtoRows( $rows );
	
	return $rows;
}

//=========================================
?>
