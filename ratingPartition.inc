<?php
//======================================================================
//==  RatingPartition
//======================================================================
require_once "subjectUtils.inc";

class RatingPartition {

	function __construct( $groupCode ){
		$this->groupCode = $groupCode;
		$this->season = "2012-2013гг.";
		$this->season = "2013-2014гг.";
		//$this->loadData();
	}
	
	//----------------
	
	public function getCode() {
		return "";
	}
	
	//----------------
	
	public function getTitle() {
		return "Кубок сезона МОТЛ $this->season";
	}
	
	//----------------

	protected function getGroupTitle() {
		return null;
	}
	
	//----------------
	
	protected function getRows() {
		return array( "_7"=>"nothing" );
	}
	
	//----------------
	
	protected function getColumns() {
		return array( "n3"=>"nothing"
			);
	}
	
	//----------------
	
	private function getRawAliases() {
		$aliases = array();
		//$aliases["ba.16_.la"][] = "ba._15.la";
		//$aliases["ba.16_.st"][] = "ba._15.st";
		$aliases["c.12_13.la"][] = "c._13.la";
		$aliases["c.12_13.st"][] = "c._13.st";
		//$aliases["c.14_15.la"][] = "c.12_15.la";
		$aliases["c.14_15.la"][] = "c._15.la";
		$aliases["c.16_.la"][] = "c.16-20.la";
		$aliases["d.10_11"][] = "d._11";
		$aliases["d.14_15"][] = "d.14_";
		$aliases["e.12_13"][] = "e.12_";
		$aliases["e.12_13"][] = "es.12_";
		$aliases["e.12_13"][] = "et.12_";
		$aliases["e.12_13"][] = "es.12_13";
		$aliases["e.8_9"][] = "es._9";
		$aliases["e.8_9"][] = "et._9";
		$aliases["e.10_11"][] = "es.10-11";
		$aliases["e.10_11"][] = "es.10_11";
		//$aliases["e.10_11"][] = "es.8_11";
		$aliases["e.10_11"][] = "es.9_11";
		//$aliases["e.10_11"][] = "es._11";
		$aliases["e.14_15"][] = "es.14_15";
		$aliases["n3.8_9"][] = "n3._9";
		//$aliases["n3.8_9"][] = "n3.8_11";
		//$aliases["n4.10_11"][] = "n4.10_13";
		$aliases["n4.10_11"][] = "n4.10_";
		//$aliases["n4.12_13"][] = "n4.12_15";
		$aliases["n4._7"][] = "n4.6_7";
		//$aliases["n4.8_9"][] = "n4.8_11";
		$aliases["n4.8_9"][] = "n4._9";
		$aliases["n5.10_11"][] = "n5.10_";
		//$aliases["n5.8_9"][] = "n5.8_11";
		$aliases["n5.8_9"][] = "n5s.8_9";
		$aliases["you.la"][] = "you1.la";
		$aliases["you.la"][] = "you2.la";
		$aliases["you.st"][] = "you1.st";
		$aliases["you.st"][] = "you2.st";
		return $aliases;
	}
	
	protected function getAliases( $code ) {
		$aliases = $this->getRawAliases();
		if( isset($aliases[$code]) ) {
			return $aliases[$code];
		}
		return array();
	}
	
	//----------------
	
	protected function getUnAlias() {
	}
	
	//----------------
	
	/*
	protected function groupCodeFromRC( $rowCode, $colCode ) {
		$c = $this->groupCodeFromRC0($rowCode,$colCode);
		//echo "$colCode:$rowCode => $c<br>";
		return $c;
	}*/
	
	protected function groupCodeFromRC( $rowCode, $colCode ) {
		$c = explode( ",", $colCode );
		if( count($c) > 1 ) {
			return "$c[0].$rowCode.$c[1]";
		}
		//return "$colCode.$rowCode";
		$parts = array();
		if( strlen($colCode) ) $parts[]=$colCode;
		if( strlen($rowCode) ) $parts[]=$rowCode;
		return implode( ".", $parts );
	}
	
	//----------------
	
	public function testGroup( $group ) {
		$rows = $this->getRows();
		$cols = $this->getColumns();
		
		foreach( $rows as $rowCode=>$rowLabel ) {
			foreach( $cols as $colCode=>$cl ) {
				$code = $this->groupCodeFromRC( $rowCode, $colCode );
				if( $code == $group ) {
					return true;
				}
			}
		}
		
		return false;
	}
	
	//----------------
	
	public function produceAllTops() {
		$codes = $this->getAllGroups();
		//print_r($codes);
		$s = "";
		
		$s .= "<table class=mainText>\r\n";
		
		//$s .= "(" . implode( "<br>", $codes ) . ")";
		$cls = get_class( $this);

		foreach( $codes as $code ) {
			$part = new $cls($code);
			//echo "==$code==<br>";
			$s .= $part->produceTop();
			$s .= "\r\n";
		}
		
		$s .= "</table>\r\n";
		
		//die($s);
		return $s;
	}
	
	//----------------
	
	public function produceTop() {
		$this->loadData();
		if( ! $this->ceLinks ) {
			return null;
		}

		$this->calcRating();
		//print_r( $this->ratedCouples );
		//die();
		//foreach( $this->ratedCouples as $k=>$data ) {
		
		$s = "";
		
		$groupTitle = $this->getGroupTitle();
		
		$s .= "<tr> <td class=cellHdr colspan=4>$groupTitle</td></tr>\r\n";
		
		for( $k=0; $k<3 && $k<count($this->ratedCouples); $k++ ) {
			$couple = $this->ratedCouples[$k];
			$k1 = $k+1;
			//print_r($couple);die();
			$s .= "<tr> <td class=cellNum>$k1</td> "
					."<td>$couple->title</td>"
					."<td>$couple->textClub0</td>"
					."<td>$couple->textTeacher0</td>"
					."</tr>\r\n";
		}
		
		return $s;
	}
	
	//----------------
	
	public function isGroupPresent( $code ) {
		if( in_array( $code, $this->groupsPresent ) ) {
			return true;
		}
		
		$als = $this->getAliases( $code );
		foreach( $als as $al ) {
			if( in_array( $al, $this->groupsPresent ) ) {
				return true;
			}
		}
		return false;
	}
	
	//----------------
	
	public function getAllGroups() {
		$this->loadData();

		$rowsExt = $this->getRows();
		$colsExt = $this->getColumns();

		$codes = array();
		
		foreach( $colsExt as $colCode=>$colLabel ) {
			foreach( $rowsExt as $rowCode=>$rowLabel ) {
				$code = $this->groupCodeFromRC( $rowCode, $colCode );
				
				if( $this->isGroupPresent( $code ) ) {
					$codes[] = $code;
				}
			}
		}
		
		return $codes;
	}
	
	//----------------
	
	public function makeGroupMatrix() {
	
		$this->loadData();

		$rowsExt = $this->getRows();
		//array_unshift( $rowsExt, array(null=>null) );
		$colsExt = $this->getColumns();
		//array_unshift( $colsExt, array(null=>null) );
//		print_r($rowsExt);print_r($colsExt);die();
		
		$s = "<table style='margin-bottom:10' border=1 cellspacing=0>\n";
		
		$s .= "<tr>";
		$s .= "<td class='HeaderCell'>&nbsp;</td>";
		foreach( $colsExt as $colCode=>$colLabel ) {
			$cellText = $colLabel ? $colLabel : "&nbsp";
			$s .= "<td class='HeaderCell'><nobr>$cellText</nobr></td>";
		}
		$s .= "</tr>\n";
		
		
		foreach( $rowsExt as $rowCode=>$rowLabel ) {
			$s .= "<tr>";
			$s .= "<td class='HeaderCell'><nobr>$rowLabel</nobr></td>";
			foreach( $colsExt as $colCode=>$colLabel ) {
				
				$cellText = $this->makeGroupButton( $rowCode, $colCode );
				$s .= "<td style='width:75px'>$cellText</td>";
			}
			$s .= "</tr>\n";
		}
		$s .= "</table>\n";
		
		return $s;
	}
	
	//----------------
	
	private function makeGroupButton( $rowCode, $colCode ) {

		$curCode = $this->groupCodeFromRC( $rowCode, $colCode );
		
		if( ! $this->isGroupPresent( $curCode ) ) {
			return "&nbsp;";
		}
		
		if( $curCode == $this->groupCode ) {
			return "<button style='border-color:#FF564F;background-color:#FF564F;width:100%; height:100%; color:#FF564F'>Показать</button>";
		}
		else {
        return "<input type=button style='width:75px; height:100%' "
                  ."onclick='location.href=\"rating.php?ratingGroup=$curCode\"' "
                  ." value='показать'>";
		}
	}
	
	//----------------
	
	private function loadData() {

		if( isset($this->isLoaded) ) {
			return;
		}
		$this->isLoaded = true;
		
		$minDate = "2013-09-01";
		$maxDate = "2014-09-01";
		$onlyMotl = 1;
		
		$allGroups = array();
		foreach( $this->getRows() as $rowCode=>$rowLabel ) {
			foreach( $this->getColumns() as $colCode=>$cl ) {
				$code = $this->groupCodeFromRC( $rowCode, $colCode );
				$allGroups[] = $code;
				$als = $this->getAliases( $code );
				foreach( $als as $al ) {
					//echo "$code => $al<br>";
					$allGroups[] = $al;
				}
			}
		}
		$sAllGroups = "'" . implode( "','", $allGroups ) . "'";
		//die($sAllGroups);
		$sql = "SELECT DISTINCT comp.groupCode "
			."FROM db_events AS comp INNER JOIN db_couple_event AS ce ON comp.id = ce.idEvent "
				."INNER JOIN db_events AS event ON comp.pid = event.id "
				."INNER JOIN db_couples AS cpl ON cpl.id = ce.idCouple "
			."WHERE comp.groupCode In($sAllGroups) "
				."AND event.date>='$minDate' "
				."AND event.date<='$maxDate' "
				."AND comp.havePlaces "
				."AND comp.hidden is null "
//				."AND cpl.motl"
				.( $onlyMotl ? " AND cpl.motl" : "" )
				;
		//die($sql);
//		echo "$sql<p>";
		
		$this->groupsPresent = MotlDB::queryValues($sql);
		//print_r($this->groupsPresent);
		
		$als = $this->getAliases( $this->groupCode );
		$als[] = $this->groupCode;
		//$als = array($this->groupCode);
		$scodes = "'" . implode( "','", $als ) . "'";
		//die($scodes);
		
		$sql = "SELECT comp.id, comp.pid, comp.groupCode, comp.ncouples, "
					."event.date, event.date1, event.title "
			."FROM db_events AS comp INNER JOIN db_events AS event "
				." ON comp.pid = event.id "
//			."WHERE comp.groupCode='$this->groupCode' "
			."WHERE comp.groupCode In($scodes) "
				."AND comp.type='competition' "
				."AND comp.havePlaces "
				."AND event.date>='$minDate' "
				."AND event.date<='$maxDate' "
				//."AND event.hidden Is Null "
			."ORDER BY event.date, event.date1";
			//die($sql);
		$this->events = MotlDB::queryRowsIndexed("id",$sql);
		//print_r($this->events);
		
		$eids = array();
		foreach( $this->events as $id=>$row ) {
			$eids[] = $id;
			$row->coeff = $this->getCompCoefficient($row);
		}
		$esids = implode( ",", $eids );
		//print_r($this->events);
		$sql = "SELECT idEvent,idCouple,place,groupCode,pid "
				."FROM db_couple_event ce "
					."INNER JOIN db_couples c ON ce.idCouple=c.id "
					."INNER JOIN db_events e ON ce.idEvent=e.id "
				."WHERE idEvent In($esids)"
				.( $onlyMotl ? " AND c.motl" : "" );
		//die($sql);
		$this->ceLinks = MotlDB::queryRows($sql);
		$cids = array();
		foreach( $this->ceLinks as $link ) {
			$cids[] = $link->idCouple;
		}
		$csids = implode( ",", $cids );
		//print_r($cids);
		$sql = "SELECT cpl.id,"
				."CONCAT_WS(' ',m.surname,m.name,'-',f.surname,f.name) as title, cpl.club, cpl.teacher "
			."FROM db_couples cpl INNER JOIN db_person m ON cpl.idMan=m.id "
				."INNER JOIN db_person f ON cpl.idLady=f.id "
			."WHERE cpl.id In($csids)";
		$this->couples = MotlDB::queryRowsIndexed("id",$sql);
		SubjectUtils::addCCCTtoRows( $this->couples );
		
		//print_r($this->couples);die();
//		echo "<br>$sql<br>";
	}
	//E10-11
	//----------------
	
	protected function calcRating() {
		$rating = array();
		$maxRating = 0;
		$this->linkMap = array();
		
		foreach( $this->ceLinks as $link ) {
			$cid = $link->idCouple;
			$eid = $link->idEvent;
			$this->linkMap[$cid][$eid] = $link;
			$couple = $this->couples[$cid];
			$event  = $this->events[$eid];
			
			$term = $this->calcRatingTerm( $link, $couple, $event );
			$link->term = $term;
			
			$link->formula = $this->ratingTermFormula( $link, $couple, $event );
			
			if( ! isset($rating[$cid]) ) {
				$rating[$cid] = 0;
			}
			$rating[$cid] += $term;
			$maxRating = max( $maxRating, $rating[$cid] );
		}
		
		if( $maxRating <= 0 ) {
			$maxRating = 0.0001;
		}
		
		$srt = array();
		foreach( $rating as $id=>$value ) {
			$v = round( 10000*(1-$value/$maxRating) );
			$v = substr( "00000$v", -5 );
			$srt[$id] = $v . $this->couples[$id]->title;
		}
		asort($srt);
//		print_r($srt);
//					$couple->ratingPlace = 1;

		
		$this->ratedCouples = array();
		$placeCnt = 1;
		$ratingPlaces = array();
		foreach( $srt as $id=>$value ) {
			$couple = $this->couples[$id];

			$score = round( $rating[$id], 2 );
			$couple->rating = $score;
			
			if( ! isset($ratingPlaces[$score]) ) {
				$ratingPlaces[$score] = array($placeCnt,$placeCnt);
			}
			else {
				$ratingPlaces[$score][0] 
					= min( $placeCnt, $ratingPlaces[$score][0] );
				$ratingPlaces[$score][1] 
					= max( $placeCnt, $ratingPlaces[$score][1] );
			}
			
			$this->ratedCouples[] = $couple;
			$placeCnt ++;
		}
//		print_r($ratingPlaces);	die();

		foreach( $srt as $id=>$value ) {
			$couple = $this->couples[$id];
			$places = $ratingPlaces[$couple->rating];
			if( $places[0] == $places[1] ) {
				$couple->ratingPlace = $places[0];
			}
			else {
				$couple->ratingPlace = "$places[0]-$places[1]";
			}
		}
	}
	
	//----------------
	
	public function makeRating() {
	
		$this->loadData();
		if( ! $this->ceLinks ) {
			return null;
		}

		$this->calcRating();
		
		$s = "";
		
		$s .= "<script src='tip.js'></script>";
		$s .= "<script src='rating.js'></script>";
		
		$s .= "<script>";
		$s .= $this->makeCoupleJSTable();
		$s .= $this->makeEventJSTable();
		$s .= $this->makeCoupleEventJSTable();
		$s .= "</script>";
		
		$s .= "<div id='ratingTip' 	style='opacity:1;padding:20px;display:none;position:absolute;border:#B3B3B5 1px solid;background-color:#D5E5F4;'>ddd</div>";
		
		$s .= "<table style='margin-bottom:10' border=1 cellspacing=0>";
		$s .= "<tr>";
		$s .= "<td class='HeaderCell'>&nbsp;</td>";
		$s .= "<td class='HeaderCell'>Пара</td>";
		$s .= "<td class='HeaderCell'>Рейтинг</td>";
		foreach( $this->events as $event ) {
			$hnd = "onmouseover=\"onEventMouse(this,'$event->id')\"";
			$s .= "<td class='HeaderCell' $hnd>"
					."<A href='db.php?id=e$event->pid,r$event->id'>"
					."$event->date</a></td>";
		}
		$s .= "</tr>";
		
		$p0 = $p1 = 1;
		for( $k=0; $k<count($this->ratedCouples); $k++ ) {
			$p = $k+1;
		}
		
		foreach( $this->ratedCouples as $couple ) {
			$s .= "<tr>";
			$cplTitle = "<nobr>" 
						. str_replace( " - ", "</nobr> <nobr>- ", $couple->title ) 
						. "</nobr>";
			
			$s .= "<td class='HeaderCell'><center>$couple->ratingPlace</center></td>";
			
			$hnd = "onmouseover=\"onCoupleMouse(this,'$couple->id')\"";
			$s .= "<td class='HeaderCell' $hnd>"
					."<A href='db.php?id=c$couple->id'>"
					."$cplTitle</a></td>";
			$s .= "<td class='HeaderCell'>$couple->rating</td>";
			
			foreach( $this->events as $event ) {
				if( isset($this->linkMap[$couple->id][$event->id]) ) {
					$link = $this->linkMap[$couple->id][$event->id];
					$hnd = " onmouseover=\"onRatingMouse(this,'$couple->id','$event->id')\"";
				}
				else {
					$link = null;
					$hnd = "";
				}
					
				$cellText = $link ? $this->ratingTermText( $link, $couple, $event )
								  : "&nbsp;";
				
				$s .= "<td><div $hnd style='width:70px'><center>$cellText</center></div></td>";
			}
			//print_r( $couple );
			$s .= "</tr>";
		}
		$s .= "</table>";
		return $s;
	}
	
	//----------------
	
	private function normalizePlace( $p ) {
		$a = explode( '-', $p );
		$p1 = (int)trim($a[0]);
		$p2 = isset($a[1]) ? (int)trim($a[1]) : $p1;
		//echo "$p : $p1 : $p2<br>";
		return max($p1,$p2);
	}
	
	//----------------
	
	private function getCompCoefficient( $row ) {
	//print_r($link);die();
		$acode = explode( ".", $row->groupCode );
		
		$values = array(
			"juv1" => 30,
			"juv2" => 50,
			"juv"  => 50,
			"jun1" => 85,
			"jun2" => 155,
			"jun"  => 155,
			"you1" => 180,
			"you2" => 245,
			"you"  => 245,
			"am"   => 255,

			"n"  => 20,
			"n3" => 20,
			"n4" => 20,
			"n5" => 20,
				"n5s" => 20,
			"e"  => 30,
				"es"  => 30,
				"et"  => 30,
			"d"  => 45,
			"c"  => 80,
			"ba" => 200,
		);
		
		$scales = array(
			//2012-2013
			4520 => 1.7, // Кубок Дружбы
//			4520 => 0, // Кубок Дружбы
			//2011-2012
			579  => 1.7, // Кубок Дружбы
			2151 => 1.7, // Кубок Империи
			2169 => 1.7,  // Tica Tica Cup
			2172 => 2    // Первенства и Чемпионат
		);
		$scale = isset($scales[$row->pid]) ? $scales[$row->pid] : 1.0;
	$scale = 1.0;
		
		$code0 = $acode[0];
		
		if( isset($values[$code0])) {
			return $values[$code0]*$scale;
		}
		
print_r($row);die();
echo "Unknown group code: $code0<br>"; return 1;
		die( "Unknown group code: $code0" );
	}
	
	//----------------
	
	protected function calcRatingTerm( $link, $couple, $event ) {
		$p = $this->normalizePlace($link->place);
		$n = $event->ncouples;
		if( $n>1 && $p )
//			$v = ($n-$p)/($n-1);
			$v = (1-$p/$n);
		else 
			$v = 0;
		//echo "$p $n $v<br>";
		return $event->coeff * $v;
	}
	
	//----------------
	
	protected function ratingTermText( $link, $couple, $event ) {
		
		$text = "$link->place / $event->ncouples";
		$v = $this->calcRatingTerm( $link, $couple, $event );

		$url = "db.php?id=e$event->pid,r$event->id&hc=$couple->id";
		//		db.php?id=e2151,r2634&hc=210
		
		return "<A href='$url' itle='$v'>$text</a>";
	}
	
	//----------------
	
	protected function ratingTermFormula( $link, $couple, $event ) {

		$p = $this->normalizePlace($link->place);
		$n = $event->ncouples;
		if( $n>1 && $p ) {
			return  "$event->coeff * ( 1 - $p/$n )";
		}
		return null;
	}
	
	//----------------
	
	protected function makeCoupleJSTable() {
		$a = array();
		foreach( $this->ratedCouples as $k=>$data ) {
			$title = $data->title;
//			$club = escapeJsString($data->raw_club);
//			$teacher = escapeJsString($data->raw_teacher);
			$club = escapeJsString($data->textClub0);
			$teacher = escapeJsString($data->textTeacher0);
			$a[] = "$data->id:{title:\"$title\","
						."club:\"$club\",teacher:\"$teacher\"}";
		}
		
		$sa = implode( ", ", $a );
		$s = "var ratingCoupleJSTable = { $sa };\n";

		return $s;
	}
	
	//----------------
	
	protected function makeEventJSTable() {
		//print_r( $this->events ); die();
		
		$a = array();
		foreach( $this->events as $id=>$data ) {
			$a[] = "$id:{date:\"$data->date\",ncouples:$data->ncouples,coeff:$data->coeff,title:\"$data->title\"}";
		}
		
		$sa = implode( ", ", $a );
		$s = "var ratingEventJSTable = { $sa };\n";
		//die($s);
		return $s;
	}
	
	//----------------
	
	protected function makeCoupleEventJSTable() {
		//print_r( $this->linkMap ); die();
		
		$a = array();
		foreach( $this->linkMap as $idCouple=>$events ) {
			foreach( $events as $idEvent=>$data ) {
				$id = "{$idCouple}_{$idEvent}";
			//print_r($data);die();
				$place = $data->place;
				$term = round( $data->term, 2 );
				$formula = $data->formula;
				$a[] = "'$id':{place:\"$data->place\",term:'$term = $formula'}";
			}
		}
//calcRatingTerm
		$sa = implode( ", ", $a );
		$s = "var ratingCoupleEventJSTable = { $sa };\n";
		
		return $s;
	}
	
	//----------------
};

//======================================================================
/*
public function testGroup( $group )
public function makeGroupMatrix()
public function makeRating()

protected function getRows()
protected function getColumns()

protected function groupCodeFromRC( $rowCode, $colCode )
protected function calcRatingTerm( $link, $couple, $event )
protected function ratingTermText( $link, $couple, $event )
*/
//======================================================================

class RatingPartitionAges extends RatingPartition {
	//----------------
	
	public function getCode() {
		return "rating";
	}
	
	//----------------
	
	public function getTitle() {
		return "Рейтинговый кубок сезона МОТЛ $this->season";
	}
	
	//----------------

	protected function getGroupTitle() {
		$parts = explode( ".", $this->groupCode );
		
		$ages = array(
					"juv1"=>"Ювеналы 1",
					"juv2"=>"Ювеналы 2",
					"jun1"=>"Юниоры 1",
					"jun2"=>"Юниоры 2",
					"you"=>"Молодежь",
					"am"=>"Взрослые" );
		
		$s = "";

		$age = $parts[0];
		array_shift( $parts );
		if( isset($ages[$age]) ) {
			$age = $ages[$age];
		}
		$s .= $age;
		
		if( count($parts) ) {
			$s .= ", " . ucfirst($parts[0]);
		}
		
		return $s;
	}
	
	//----------------
	
	protected function getRows() {
		return array( 
				"st"=>"St", 
				"la"=>"La", 
				""=>"St+La" 
			);
	}
	
	//----------------
	
	protected function getColumns() {
		return array(
//				"juv"=>"Ювеналы",
				"juv1"=>"Ювеналы 1",
				"juv2"=>"Ювеналы 2",
//				"juv,juv1,juv2"=>"Ювеналы",
//				"jun"=>"Юниоры",
//				"jun,jun1,jun2"=>"Юниоры",
				"jun1"=>"Юниоры 1",
				"jun2"=>"Юниоры 2",
				"you"=>"Молодежь",
//				"you,you1,you2"=>"Молодежь",
				//"you1"=>"Молодежь 1",
				//"you2"=>"Молодежь 2",
				"am"=>"Взрослые"
			);
	}
};

//======================================================================

class RatingPartitionClasses extends RatingPartition {
	//----------------
	
	public function getCode() {
		return "class";
	}
	
	//----------------
	
	public function getTitle() {
		return "Классификационный кубок сезона МОТЛ $this->season";
	}
	
	//----------------

	protected function getGroupTitle() {
		$parts = explode( ".", $this->groupCode );
		
		$s = "";

		$cls = strtoupper( $parts[0] );
		array_shift( $parts );
		if( $cls == "ba" ) $cls = "B+A";
		$s .= $cls;
		
		if( is_int(strpos($parts[0],"_")) ) {
			$age = $parts[0];
			array_shift( $parts );
			
			$a = substr($age,0,1);
			$b = substr($age,-1,1);
			
			if( $a == '_' ) {
				$age = substr( $age, 1 ) . " и мл.";
			}
			else
			if( $b == '_' ) {
				$age = substr( $age, 0, -1 ) . " и ст.";
			}
			else {
				$age = str_replace( "_", "-", $age );
			}
			
			$s .= " $age";
		}
		
		if( count($parts) ) {
			$s .= ", " . ucfirst($parts[0]);
		}
		
		return $s;
	}
	
	//----------------
	
	protected function getRows() {
		return array( 
					"_7"=>"7 и мл.", 
					"8_9"=>"8-9", 
//					"_9"=>"9 и мл.", 
					"10_11"=>"10-11", 
//					"_11"=>"11 и мл.", 
					"12_13"=>"12-13", 
					"14_15"=>"14-15", 
					"16_"=>"16 и ст." );
	}
	
	//----------------
	
	protected function getColumns() {
		return array( 
			"n3"=>"N3", "n4"=>"N4", "n5"=>"N5", "e"=>"E", 
			"d"=>"D", "d,st"=>"D,St", "d,la"=>"D,La",
			/*"c"=>"C",*/ "c,st"=>"C,St", "c,la"=>"C,La",
					  "ba,st"=>"B+A,St", "ba,la"=>"B+A,La",
			);
	}
};

//======================================================================
?>
