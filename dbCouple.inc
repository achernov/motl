<?php
require_once "baseColumn.inc";
require_once "subjectUtils.inc";
//=====================

class DbCoupleProducerFull extends DbCoupleProducer {
  //----------------

  public function produceFull() {
      $id = $this->row->id;
    
    $s = "";
    
    //===== Load Common Data
    $idMan = $this->row->idMan;
    ASSERT_int($idMan);
    $manRow  = MotlDB::queryRow( "SELECT * FROM db_person WHERE id='$idMan'"  ); // ASS $idMan
    $manRow->rowLabel = "Партнер";
    
    $idLady = $this->row->idLady;
    ASSERT_int($idLady);
    $ladyRow = MotlDB::queryRow( "SELECT * FROM db_person WHERE id='$idLady'" ); // ASS $idLady
    $ladyRow->rowLabel = "Партнерша";

    // ASS $idMan, $idLady
    $otherLadyRows = MotlDB::queryRows( 
                          "SELECT DISTINCT db_couples.id, db_person.surname, db_person.name "
                         ."FROM db_couples INNER JOIN db_person ON db_couples.idLady=db_person.id "
                         ."WHERE idMan='$idMan' And idLady<>'$idLady'" );
                         
    // ASS $idMan, $idLady
    $otherManRows = MotlDB::queryRows( 
                          "SELECT DISTINCT db_couples.id, db_person.surname, db_person.name "
                         ."FROM db_couples INNER JOIN db_person ON db_couples.idMan=db_person.id "
                         ."WHERE idLady='$idLady' And idMan<>'$idMan'" );
                         
    $titleMan  = $this->makeText( $manRow,  "[plain|surname] [plain|name]" );
    $titleLady = $this->makeText( $ladyRow, "[plain|surname] [plain|name]" );

    $otherLadies = $this->makeList( $otherLadyRows, 
            "<A href='db.php?id=c[plain|id]'><b>$titleMan</b> - [plain|surname] [plain|name]</a>" );
            
    $otherMen    = $this->makeList( $otherManRows, 
            "<A href='db.php?id=c[plain|id]'>[plain|surname] [plain|name] - <b>$titleLady</b></a>" );
                         
    //===== End Load Common Data
    
    //===== Show Common Data
    $s .= "<h1>База данных: $titleMan - $titleLady</h1>";

    if( Motl::getParameter( "editmode" ) ) {
      $s .= $this->makeEditLink() . "&nbsp;" . $this->makeKillLink(). "<p>";
    }

    $background = $this->row->split ? "bgcolor='#c0c0c0'" : "";
    
    $headerCells = "<td class=DbTableHeader>&nbsp;</td> <td class=DbTableHeader>Фамилия</td> <td class=DbTableHeader>Имя</td> <td class=DbTableHeader>Дата рождения</td>";
    
    $rowTemplateCells = "<td class=DbTableSider>[editbar|edit]<A href='db.php?id=p[plain|id]'>[plain|rowLabel]</a></td>"
                       ."<td class=DbTableCell $background>[plain|surname]&nbsp;</td>"
                       ."<td class=DbTableCell $background>[plain|name]&nbsp;</td>"
                       ."<td class=DbTableCell $background>[plainDateAge|birthDate]&nbsp;</td>";
    
    if( $otherLadies || $otherMen ) {
      $manRow->others  = $otherLadies;
      $ladyRow->others = $otherMen;
      $headerCells .= "<td class=DbTableHeader>В других парах</td> ";
      $rowTemplateCells .= "<td class=DbTableCell $background>[plain|others]&nbsp;</td> ";
    }
    
    $s .= "<table cellspacing='0' border='1'>";
    $s .= "<tr> $headerCells </tr>";
    $man = new DbPersonProducer($manRow);
    $s .= $man->makeText( $manRow,  "<tr> $rowTemplateCells </tr>" );
    $lady = new DbPersonProducer($ladyRow);
    $s .= $lady->makeText( $ladyRow,  "<tr> $rowTemplateCells </tr>" );
    $s .= "</table>";
    //===== End Show Common Data
    
    //===== Show Split
    
    if( $this->row->split ) {
//      $s .= "<p>Пара распалась";
    }
    
    //===== End Show Split

    $s .= "&nbsp;<p>";

    //===== Show Country, City, Club, Teacher
    
    $s .= SubjectUtils::makeCcct( $this->row );
    
    //===== End Show Country, City, Club, Teacher
    
    $subject = new SubjectCouple();
    
    //===== Show Ratings
    /*
    $r = $subject->makeRatings( $id );
    if( $r ) {
      $s .= "<h2 style='color:000080'>Рейтинг сезона</h2>$r";
    }
      */                              
    //===== End Show Ratings
    
    //===== Show Competitions
    
    
    $also = DbCoupleProducerFull::makeAlsoSolo( $idMan, $titleMan, $idLady, $titleLady );
    $seeAlso = $also ? "<font style='font-weight:normal'>(см. также $also)</font>" : null;
    
    $s .= SubjectUtils::makeEvents( "<h2 style='color:000080'>Результаты соревнований: $seeAlso</h2>", 
                                    $subject, $id, "SubjectUtils::resultRenderer" );

    $s .= SubjectUtils::makeEvents( "<h2 style='color:000080'>Зарегистрированы на соревнования:</h2>", 
                                    $subject, $id, "SubjectUtils::registrationRenderer" );
    //===== End Show Competitions
    
    return $s;
  }

  //----------------
  
  private function makeAlsoSolo( $idMan, $titleMan, $idLady, $titleLady ) {
  
    $subjectSolo = new SubjectSolo();
    
    $also = array();
    
    $linkMan  = "<A href='db.php?id=p$idMan'>соло $titleMan</a>";
    $linkLady = "<A href='db.php?id=p$idLady'>соло $titleLady</a>";
    
    if( SubjectUtils::testParticipation( $idMan, $subjectSolo, null ) ) {
      $also[] = $linkMan;
    }
    
    if( SubjectUtils::testParticipation( $idLady, $subjectSolo, null ) ) {
      $also[] = $linkLady;
    }
    
    return count($also) ? implode( ", ", $also ) : null;
  }
  
  //----------------
  
  public static function makeTable( $url, $cond ) {
  
    $s = "";

    //----------

    $sql = "SELECT Couple.id,"
              ."Man.id as mid, Man.surname as msurname, Man.name as mname,"
              ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname, "
              ."(SELECT count(*) FROM db_couple_event WHERE idCouple=Couple.id) AS nRegs "
          ."FROM db_couples AS Couple "
              ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
              ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id";
      //die($sql);
    $rows = MotlDB::queryRows( $sql );
                              
    //----------

    $colId      = new Column( "id",   "id",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]>[plain|id]</a>", "[plain|id]" );
    $colMan     = new Column( "man",  "партнер",    "<nobr><A href=db.php?id=p[plain|mid]>[plain|msurname] [plain|mname]</a></nobr>", "[plain|msurname] [plain|mname]" );
    $colLady    = new Column( "lady", "партнерша",  "<nobr><A href=db.php?id=p[plain|fid]>[plain|fsurname] [plain|fname]</a></nobr>", "[plain|fsurname] [plain|fname]" );
    $colRegs    = new Column( "regs", "N регистраций",  "[plain|nRegs]" );
    
    $template = array( $colId, $colMan, $colLady, $colRegs );
    
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort" );
    $s .= $table->produce( "DbCoupleProducer", $rows, $url );
    
    return $s;
  }
  
  //----------------

  public function getTableName() {
    return "db_couples";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"idMan"	=>	"[text|idMan]",
		"idLady"	=>	"[text|idLady]",
		"idPro"	=>	"[text|idPro]",
		"idAm"	=>	"[text|idAm]",
		"split"	=>	"[text|split]",

		"country"	=>	"[text|country]",
		"city"	=>	"[text|city]",
		"club"	=>	"[text|club]",
		"teacher"	=>	"[text|teacher]",

		"motl"	=>	"[text|motl]"
	   );
  }

  //----------------
}

//=====================
?>
