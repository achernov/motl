<?php
//=====================
require_once "dbAll.inc";
require_once "subjectUtils.inc";
//=====================

class DbPersonProducerFull extends DbPersonProducer {
  //----------------

  public function produceFull() {
    $id = $this->row->id;
    
    $s = "";
    //Q($this);
    $s .= $this->makeText( $this->row, "<h1>[editbar|edit,copy,del] База данных: [plain|surname] [plain|name]</h1>" );

    //----- Personal Info
    $style = 'padding-right:10;';
    $s .= "<table style='font-family:arial; font-size=12;'>";
    if( Motl::getParameter("editmode") ) {
      $s .= $this->makeText( $this->row, "<tr style='color:green'> <td style='$style'><b>ts_created:</b></td> <td>[plainDate|ts_created]</td> </tr>" );
    }
    $s .= $this->makeText( $this->row, "<tr> <td style='$style'><b>Фамилия:</b></td> <td>[plain|surname]</td> </tr>" );
    $s .= $this->makeText( $this->row, "<tr> <td style='$style'><b>Имя:</b></td> <td>[plain|name]</td> </tr>" );
    $s .= $this->makeText( $this->row, "<tr> <td style='$style'><b>Дата рождения:</b></td> <td>[plainDateAge|birthDate]</td> </tr>" );
    
    if( $this->row->disambig ) {
      
      $comment = $this->row->disambig;
      
      //$tezki = $this->makeTezki();
		$tezki = DbPersonProducerFull::makeTezki();

      if( $tezki ) {
        $comment .= ", не путать с $tezki";
      }
      
      $s .= "<tr> <td style='$style'><b>Примечание:</b></td> <td>$comment</td> </tr>";     
    }
    
    $s .= "</table><p>";
    //----- End Personal Info

    //----- Dancer Info
    
    $subject = new SubjectSolo();

    if( SubjectUtils::testParticipation( $id, $subject, null ) ) {
      $s .= SubjectUtils::makeCcct( $this->row );
    }

    //===== Show Ratings
    
    $r = $subject->makeRatings( $id );
    if( $r ) {
      $s .= "<h2 style='color:000080'>Рейтинг сезона</h2>$r";
    }
                                    
    //===== End Show Ratings
    
    $inCouples = DbPersonProducerFull::makePersonCouples();
    
    
    $res = SubjectUtils::makeEvents( "<h2 style='color:000080'>Результаты соревнований:</h2>", 
                                     $subject, $id, "SubjectUtils::resultRenderer" );
                                     
    $reg = SubjectUtils::makeEvents( "<h2 style='color:000080'>Зарегистрированы на соревнования:</h2>", 
                                     $subject, $id, "SubjectUtils::registrationRenderer" );
                                     
    if( $inCouples || $res || $reg ) {
      $s .= "<h2 class=red>Участие в соревнованиях</h2>";
    }
    $s .= $inCouples;
    
    if( $res || $reg ) {
      $s .= "<h2>Соло:</h2>$res$reg";
    }

//----- End Dancer Info
    
    //----- Teacher Info
    
    $editmode = Motl::getParameter( "editmode" );
    $url = "db.php?id=p$id";
    $column = new BaseColumnTeacher();
    
    $pupils = SubjectUtils::makePupils( $url, $column, $id );
    
    if( $pupils ) {
      $s .= "<h2 class=red>Преподавание</h2>";
      $s .= $pupils;
    }

    //----- End Teacher Info
    
    return $s;
  }

  //----------------
  
  private function makeTezki() {
    $id = (int) $this->row->id;
    
    // COD $id
    $sql = "SELECT other.* FROM db_person AS me, db_person AS other "
          ."WHERE me.id=$id and other.id<>$id and me.name=other.name and  me.surname=other.surname";
    
    $rows = MotlDB::queryRows( $sql );
    $others = array();
    foreach( $rows as $k=>$row ) {
      $others[] = $this->makeText( $row, "<A href='db.php?id=p[plain|id]'>[plain|surname] [plain|name] [if|disambig|<i>([plain|disambig])</i>]</a>" );
    }
    $sothers = implode( ", ", $others );
    return $sothers;
  }
  
  //----------------
  
  private function makePersonCouples() {
    $id = $this->row->id;
    ASSERT_int($id);
    
    // ASS $id
    $sql = "SELECT cpl.id, min(e.date) AS minDate, max(e.date) AS maxDate "
          ."FROM db_couples AS cpl LEFT JOIN (db_couple_event AS ce "
                                 ."INNER JOIN db_events AS c ON ce.idEvent=c.id "
                                 ."INNER JOIN db_events AS e ON c.pid=e.id) ON cpl.id=ce.idCouple "
          ."WHERE idMan=$id or idLady=$id "
          ."GROUP BY cpl.id "
          ."ORDER BY maxDate ";
    //echo $sql;
    $couples = MotlDB::queryRows( $sql );  
    
    if( ! count($couples) ) {
      return null;
    }

    $s = "";
    $s .= "<table cellspacing='0' border='1'>";
    $s .= "<tr>";
    $s .=   "<td class=DbTableHeader>В составе пары</td>";
    $s .=   "<td class=DbTableHeader>Cоревнований в базе</td>";
    $s .=   "<td class=DbTableHeader>Период</td>";
    $s .= "</tr>";
	
	$dummy = new DbCoupleProducer();
	
    for( $k=0; $k<count($couples); $k++ ) {
      $cpId    = $couples[$k]->id;
      $minDate = $couples[$k]->minDate;
      $maxDate = $couples[$k]->maxDate;
      
      // FLD $cpId int
      $csql = "SELECT cpl.id, cpl.split, "
                    ."man.surname AS msurname, man.name AS mname, "
                    ."lady.surname AS fsurname, lady.name AS fname "
                  ."FROM db_couples AS cpl "
                  ."LEFT JOIN db_person AS man  ON cpl.idMan=man.id "
                  ."LEFT JOIN db_person AS lady ON cpl.idLady=lady.id "
                  ."WHERE cpl.id='$cpId'";
      //echo "<p>$csql<p>";
      $cplRow = MotlDB::queryRow( $csql );  
      //print_r($cplRow);

      // FLD $cpId int
      $esql = "SELECT DISTINCT e.id FROM db_couple_event AS ce "
                           ."INNER JOIN db_events AS c ON ce.idEvent=c.id "
                           ."INNER JOIN db_events AS e ON c.pid=e.id "
             ."WHERE ce.idCouple='$cpId' and e.type='event' "; 
//      echo $esql;
      $eventIds = MotlDB::queryValues( $esql );
      $nEvents = count( $eventIds );
      $cplRow->nEvents = $nEvents;
      $cplRow->minDate = $minDate;
      $cplRow->maxDate = $maxDate;

      $rowStyle = $cplRow->split ? "bgcolor='#c0c0c0'" : "";
	  
	if( Motl::getParameter( "editmode" ) ) {
	  $editLink = $dummy->makeEditLink( 'Edit', $cplRow->id ) . " ";
	}
	else {
	  $editLink = "";
	}

	  
      $cplTemplate = "<tr $rowStyle> <td class=DbTableCell>$editLink <A href=db.php?id=c[plain|id]>[plain|msurname] [plain|mname] - [plain|fsurname] [plain|fname]</a></td>"
                         ."<td class=DbTableCell>[plain|nEvents]&nbsp;</td>"
                         ."<td class=DbTableCell>[plainDate|minDate] - [plainDate|maxDate]&nbsp;</td> </tr>";
      $s .= $this->makeText( $cplRow, $cplTemplate );
    }
    $s .= "</table>";
    
    return $s;
  }
  
  //----------------
}

//=====================
?>
