<?php
//=============================================================================
require_once "baseColumn.inc";
//=============================================================================

class Subject {

  function makeRatings( $id ) {
  /*
    ASSERT_int($id);
  
    $partitionSolo    = new RatingPartitionSolo();
    $partitionAges    = new RatingPartitionAges();
    $partitionClasses = new RatingPartitionClasses();

    $allGroups = array_merge( 
                $partitionSolo->listGroups(),
                $partitionClasses->listGroups(),
                $partitionAges->listGroups() );
  //print_r($allGroups);
  
    $field = $this->getLinkField();
    
    $compCond = RatingPartition::relevantCompetitionCond();
    
    // ASS $id
    // COD $field, $compCond
    $sql = "SELECT DISTINCT groupCode "
          ."FROM db_couple_event INNER JOIN db_events ON db_couple_event.idEvent=db_events.id "
          ."WHERE $field=$id AND groupCode<>'' AND $compCond";
    
    $groups = MotlDB::queryValues( $sql );  
    
    $sortedGroups = array();
    foreach( $allGroups as $group ) {
      if( in_array($group[0],$groups) ) {
        $sortedGroups[] = $group;
      }
    }
    
    $alinks = array();
    foreach( $sortedGroups as $group ) {
      $alinks[] = "<a href='rating.php?ratingGroup=$group[0]&highlight=$id#row$id'>$group[1]</a>";
    }
    $links = implode( ",&nbsp;&nbsp;&nbsp;", $alinks );

    return $links;*/
  }
  
}

//=============================================================================

class SubjectCouple extends Subject {

  //-----------------
  
  function getName() { return "couple"; }
  
  //-----------------
  
  function getLinkField() { return "idCouple"; }
  
  //-----------------
  
  function getLetter() { return "c"; }
  
  //-----------------
  
  function getTableName() { return "db_couples"; }
  
  //-----------------
  
  function getDataQuery() {
	$sql = "SELECT subj.*, CONCAT_WS(' ',man.surname,man.name,'-',lady.surname,lady.name) AS title "
			."FROM db_couples AS subj "
			."INNER JOIN db_person AS man ON subj.idMan=man.id "
			."INNER JOIN db_person AS lady ON subj.idLady=lady.id";
	return $sql;
  }
  
  //-----------------
  
	function loadTitle( $id ) {
		ASSERT_int($id);
		
		$sql = "SELECT CONCAT_WS(' ',Man.surname,Man.name,'-',Lady.surname,Lady.name) "
				."FROM db_couples AS Couple "
					."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
					."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
				."WHERE Couple.id=$id";
				
		return MotlDB::queryValue( $sql );
	}
  
  //-----------------
  
  function makeTableByColumn( $url, $baseColumn, $baseColumnValue, 
									$moreFields=null, $moreColumns=null ) {
	require_once "tables.inc";

    $editmode = Motl::getParameter( "editmode" );
  
    //----------
    
    ASSERT_null($moreFields);
    
    // ASS $moreFields
    $mf = $moreFields ? ", $moreFields" : "";
	
	$cname = $baseColumn->getName();
	
	// COD $cname
    $sql = "SELECT Couple.id,Couple.split,"
				."Couple.country,Couple.city,Couple.club,Couple.teacher,"
              ."Man.id as mid, Man.surname as msurname, Man.name as mname,"
              ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname $mf "
          ."FROM db_couples AS Couple "
              ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
              ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
		  ."WHERE concat(',',Couple.$cname,',') like '%,".MRES($baseColumnValue).",%'";
	if( ! User::isAdmin() ) {
		$sql .= " And (split is null or not split) ";
	}
              
    $rows = MotlDB::queryRows( $sql );  

    if( ! count($rows) ) {
      return null;
    }
    
    //----------
    
    SubjectUtils::addCCCTtoRows( $rows );
    
    //----------
    $ifSplit = "[if|split|style='color:#b0b040;']";
    
    $colInfo    = new Column( "id",     "&nbsp;",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]><small>подробнее...</small></a>", Column::NoSort );
    $colId      = new Column( "id",     "id",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]>[plain|id]</a>", "[plain|id]" );
	
    $colMan     = new Column( "man",    "партнер",    "<nobr><A $ifSplit href=db.php?id=p[plain|mid]>[plain|msurname] [plain|mname]</a></nobr>", "[plain|msurname] [plain|mname]" );
    $colLady    = new Column( "lady",   "партнерша",  "<nobr><A $ifSplit href=db.php?id=p[plain|fid]>[plain|fsurname] [plain|fname]</a></nobr>", "[plain|fsurname] [plain|fname]" );
    $colCountry = new Column( "country", "страна",  "[plain|textCountry]",  "[plain|textCountry0]" );
    $colCity    = new Column( "city",    "город",   "[plain|textCity]",  "[plain|textCity0]" );
    $colClub    = new Column( "club",    "клуб",    "[plain|textClub]",  "[plain|textClub0]" );
    $colTeacher = new Column( "teacher", "тренер",  "[plain|textTeacher]",  "[plain|textTeacher0]" );
	
	$template = array( $colInfo, $colMan, $colLady, 
                       $colCountry, $colCity, $colClub, $colTeacher );
 
/*    
    $template = array( $colId, $colMan, $colLady, 
                       $colCountry, $colCity, $colClub, $colTeacher );
 */ 
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "man" );
    $s = $table->produce( "DbCoupleProducer", $rows, $url );
    
    //----------
    
    return $s;
  }
  
  //-----------------
};

//=============================================================================

class SubjectSolo extends Subject {

  //-----------------
  
  function getName() { return "solo"; }
  
  //-----------------
  
  function getLinkField() { return "idSolo"; }
  
  //-----------------
  
  function getLetter() { return "p"; }
  
  //-----------------
  
  function getTableName() { return "db_person"; }
  
  //-----------------
  
  function getDataQuery() {
	$sql = "SELECT *, CONCAT_WS(' ',surname,name) AS title "
			."FROM db_person AS subj";
	return $sql;
  }
  
  //-----------------
  
	function loadTitle( $id ) {
		ASSERT_int($id);
		
		$sql = "SELECT CONCAT_WS(' ',surname,name) FROM db_person WHERE id=$id";
				
		return MotlDB::queryValue( $sql );
	}
  
  //-----------------
  
  function makeTableByColumn( $url, $baseColumn, $baseColumnValue, 
									$moreFields=null, $moreColumns=null ) {
	require_once "tables.inc";
  
    $editmode = Motl::getParameter( "editmode" );
  
    //----------
    
    ASSERT_null($moreFields);
    
    // ASS $moreFields
    $mf = $moreFields ? ", $moreFields" : "";
    
    $cname = $baseColumn->getName();
	// COD $cname
    $sql = "SELECT id, surname, name, country,city,club,teacher $mf FROM db_person "
			." WHERE concat(',',$cname,',') like '%,".MRES($baseColumnValue).",%'";

    $rows = MotlDB::queryRows( $sql );  

    if( ! count($rows) ) {
      return null;
    }
    
    //----------
    
    SubjectUtils::addCCCTtoRows( $rows );
    
    //----------
    
    $colInfo    = new Column( "id",     "&nbsp;",         "[editbar|edit,del] <A href=db.php?id=p[plain|id]><small>подробнее...</small></a>", Column::NoSort );
    $colId      = new Column( "id",     "id",          "[editbar|edit,del] <A href=db.php?id=p[plain|id]>[plain|id]</a>", "[plain|id]" );
	
    $colDancer  = new Column( "dancer", "фамилия,имя", "<nobr><A href=db.php?id=p[plain|id]>[plain|surname] [plain|name]</a></nobr>", "[plain|surname] [plain|name]" );
    $colCountry = new Column( "country", "страна",  "[plain|textCountry]",  "[plain|textCountry0]" );
    $colCity    = new Column( "city",    "город",   "[plain|textCity]",  "[plain|textCity0]" );
    $colClub    = new Column( "club",    "клуб",    "[plain|textClub]",  "[plain|textClub0]" );
    $colTeacher = new Column( "teacher", "тренер",  "[plain|textTeacher]",  "[plain|textTeacher0]" );

    $template = array( $colDancer, 
                       $colCountry, $colCity, $colClub, $colTeacher );
/*    
    $template = array( $colId, $colDancer, 
                       $colCountry, $colCity, $colClub, $colTeacher );
*/    
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "dancer" );
    $s = $table->produce( "DbCoupleProducer", $rows, $url );
    
    //----------
    
    return $s;
  }
  
  //-----------------
  
};

//=============================================================================

class SubjectUtils {

  
  public static function testParticipation( $id, $subject, $eventCond ) {
  
    ASSERT_int($id);
  
    $linkField = $subject->getLinkField();
    
    // ASS $id
    // COD $linkField
    $sql = "SELECT id FROM db_couple_event WHERE $linkField=$id LIMIT 1";
    
    $result = MotlDB::queryValue( $sql );  
    
    return $result;
  }
  
  //----------------

  public static function makeCcct( $row ) {
    
    $style = 'padding-right:10; padding-bottom:4';

    $s = "";
    $s .= "<table style='font-family:arial; font-size=12;'>";

    $s .= "<tr valign=top> <td style='$style'><b>Страна:</b></td> <td>";
    $s .= SubjectUtils::makeCcctItem( $row, BaseColumn::createColumn( "country" ) );
    $s .= "</td> </tr>";

    $s .= "<tr valign=top> <td style='$style'><b>Город:</b></td> <td>";
    $s .= SubjectUtils::makeCcctItem( $row, BaseColumn::createColumn( "city" ) );
    $s .= "</td> </tr>";

    $s .= "<tr valign=top> <td style='$style'><b>Клуб:</b></td> <td>";
    $s .= SubjectUtils::makeCcctItem( $row, BaseColumn::createColumn( "club" ) );
    $s .= "</td> </tr>";

    $s .= "<tr valign=top> <td style='$style'><b>Преподаватель:</b></td> <td>";
    $s .= SubjectUtils::makeCcctItem( $row, BaseColumn::createColumn( "teacher" ) );
    $s .= "</td> </tr>";

    $s .= "</table>";
    
    return $s;
  }

  //----------------
  
  private static function makeCcctItem( $row, $column ) {
    $colName = $column->getName();
    $ids = explode( ',', $row->$colName );    
    $s = $column->makeListImploded( $ids, "<br>" );
    return $s;
  }
  
  //----------------
  
  public static function makeEvents( $title, $subject, $idSubject, $renderer ) {
  
    ASSERT_int( $idSubject );
  
    $linkField = $subject->getLinkField();
    
    // ASS $idSubject
    // COD $linkField
    $compRows = MotlDB::queryRows( "SELECT *, db_events.id AS idComp "
                   ."FROM db_events INNER JOIN db_couple_event ON db_events.id=db_couple_event.idEvent "
                   ."WHERE $linkField=$idSubject and type='competition' "
                   ."ORDER BY pid DESC, position" );  
                   
    $eventComps = array();
    foreach( $compRows as $comp ) {
      $eventComps[$comp->pid][] = $comp;
    }

    $idEvents = array();
    foreach( $eventComps as $idEvent=>$comps ) {
      $idEvents[] = $idEvent;
    }
    $sidEvents = implode( ",", $idEvents );
    
//  print_r($eventComps);die($sidEvents);
    
    $eventRows = MotlDB::queryRows( "SELECT id,date,title,havePlaces,regOpen,registration FROM db_events WHERE id In($sidEvents) and type='event' ORDER BY date DESC" );  // FLD $sidEvents 
Q( $eventRows );
    $list = "";
    for( $k=0; $k<count($eventRows); $k++ ) {
      $eventRow = $eventRows[$k];
      $compRows = $eventComps[ $eventRow->id ];
      if( $renderer )
        $list .= call_user_func( $renderer, $subject, $idSubject, $eventRow, $compRows );
    }
    
    return $list ? $title . $list : "";
  }

  //----------------
  
  private static function makeGalleryLink( $idMedia ) {
    if( $idMedia ) {
      return "<A title='Щелкните, чтобы посмотреть фото/видео' href='db.php?id=$idMedia'>"
            ."<img style='margin-top:3' border=0 src='syspictures/camera.png'></a>";
    }
    else {
      return "<A title='Для этого события есть фото/видео' href='#' onclick='return false'>"
            ."<img style='margin-top:3' border=0 src='syspictures/camera.png'></a>";
    }
  }
  
  //----------------
  
  private static function makeGalleryLinkPlaceholder() {
    return "<img src='syspictures/pixel.png' width=15 height=1>";
  }
  
  //----------------
  
  public static function resultRenderer( $subject, $idSubject, $eventRow, $compRows ) {
    if( ! $eventRow->havePlaces ) return null;
    
    ASSERT_int($idSubject);
    
    $s = "";
    
    $linkField = $subject->getLinkField();

    $event = new DbEventProducer( $eventRow );

    $s .= $event->makeText( $eventRow, "<h2><A href='db.php?id=e[plain|id],r'>[plainDate|date] &nbsp; [plain|title]</a></h2>" );
    $idEvent = $eventRow->id;

    for( $k=0; $k<count($compRows); $k++ ) {
      $idComp = (int) $compRows[$k]->idComp;
      
      // ASS $idSubject
      // COD $idComp, $linkField
      $sql = "SELECT 1 "
           ."FROM db_events INNER JOIN db_couple_event ON db_events.id=db_couple_event.idEvent "
           ."WHERE (idEvent=$idComp OR pid=$idComp) "
                  ."AND $linkField=$idSubject "
                  ."AND idGallery<>''";
                          
      $galTest = MotlDB::queryValue( $sql, 0 );  
      $camera = $galTest ? SubjectUtils::makeGalleryLink( "e$idEvent,m[plain|idEvent]" ) 
                         : SubjectUtils::makeGalleryLinkPlaceholder();
      //echo "$sql<p>";
      $comp = new DbEventProducer( $compRows[$k] );
      $resultText = $comp->makeResultValue( $compRows[$k] );
      $s .= $comp->makeText( $comp->row, "$camera <A href='db.php?id=e$idEvent,r[plain|idEvent]&hc=$idSubject'>[plain|title]</a> &nbsp; - $resultText<br>" );
    }
    
    return $s;
  }

  //----------------
    
  public static function registrationRenderer( $subject, $idSubject, $eventRow, $compRows ) {
	Q( $eventRow );
    if( $eventRow->havePlaces ) return null;
	
	$editmode = Motl::getIntParameter( "editmode" );
	$regOpen = $eventRow->regOpen || ( $editmode && $eventRow->registration );
	Q( "regOpen=$regOpen" );
    if( ! $regOpen ) return null; // Не совсем корректно, может быть уже закрыто, а турнир еще не прошел

    $s = "";

    $dbp = new DbProducer();

    $s .= $dbp->makeText( $eventRow, "<h2><A href='db.php?id=e[plain|id],c'>[plainDate|date] &nbsp; [plain|title]</a></h2>" );
    $idEvent = $eventRow->id;
    
    $noCamera = SubjectUtils::makeGalleryLinkPlaceholder();
    
    for( $k=0; $k<count($compRows); $k++ ) {
      $s .= $dbp->makeText( $compRows[$k], "$noCamera <A href='db.php?id=e$idEvent,c[plain|idEvent]&hc=$idSubject'>[plain|title]</a><br>" );
    }
    
    return $s;
  }

  //----------------
  
  public static function makePupils( $url, $baseColumn, $baseColumnValue ) {
  
    $apupils = array();
    
    $subjectC = new SubjectCouple();
    $pupilsC = $subjectC->makeTableByColumn( $url, $baseColumn, $baseColumnValue );
    if( $pupilsC ) {
      $apupils[] = "<b><i>Пары:</i></b>" . $pupilsC;
    }
    
    $subjectS = new SubjectSolo();
    $pupilsS = $subjectS->makeTableByColumn( $url, $baseColumn, $baseColumnValue );
    if( $pupilsS ) {
      $apupils[] = "<b><i>Солисты:</i></b>" . $pupilsS;
    }

    if( count($apupils) ) {
      return implode( "<p>", $apupils );
    }
    else {
      return null;
    }
  }
  
  //----------------
  
  public static function addCCCTtoRows( &$rows ) {
    
    $bcCountry = new BaseColumnCountry();
    $bcCity    = new BaseColumnCity();
    $bcClub    = new BaseColumnClub();
    $bcTeacher = new BaseColumnTeacher();
    $bcBoss    = new BaseColumnBoss();
    
	foreach( $rows as $k=>$row ) {
		SubjectUtils::addCCCT_one( $row, $bcCountry,
									"country", "textCountry" );
									
		SubjectUtils::addCCCT_one( $row, $bcCity,    
									"city",    "textCity"    );
									
		SubjectUtils::addCCCT_one( $row, $bcClub,    
									"club",    "textClub"    );
									
		SubjectUtils::addCCCT_one( $row, $bcTeacher, 
									"teacher", "textTeacher" );
									
		SubjectUtils::addCCCT_one( $row, $bcBoss,    
									"boss",    "textBoss"    );
    }
    
  }
  
  //----------------
  
  private static function addCCCT_one( &$row, $column, 
								$propIn, $propOut ) {
	
	$propOut0 = $propOut."0";
	
	if( isset($row->$propIn) ) {
		$ids = explode( ",", $row->$propIn );
		$row->$propOut  = $column->makeListImploded( $ids, ", " );
		$row->$propOut0 = strip_tags( $row->$propOut );
	}
	else {
		$row->$propOut  = '';
		$row->$propOut0 = null;
	}
  }
  
  //----------------
}

//=============================================================================
?>
