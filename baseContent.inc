<?php
require_once "_core.inc";
require_once "_dbproducer.inc";
require_once "tabs.inc";
require_once "tables.inc";

require_once "dbAll.inc";
//require_once "dbClub.inc";
//require_once "dbCity.inc";
//require_once "dbCountry.inc";
//require_once "dbHall.inc";
//require_once "dbPerson.inc";
//require_once "dbCouple.inc";
//require_once "dbEvent.inc";

//=====================

/*$url = "http://chernov-pc/motl/base.php?id=men&sort=surname";
echo $url."<br>";
echo urlencode($url)."<br>";
die();*/

class BaseContent extends Producer {

  public function produce( $format = null ) {
    $script = $_SERVER['PHP_SELF'];
    
    $editmode = Motl::getParameter( "editmode" );
    $id = Motl::getParameter( "id" );
    $aid = Motl::splitId( $id );

    $s = "";
    $s .= "<h1>База данных</h1>";

    $s .= "<script src='db.js'></script>";

    //print_r($aid);
    $section = $aid[0];
    
    $tabs = new Tabs();
    $tabs->current = $section;
    $tabs->addTab( "events",   "Соревнования", "base.php?id=events" );
    $tabs->addTab( "couples",  "Пары",         "base.php?id=couples" );
    $tabs->addTab( "solo",     "Солисты",      "base.php?id=solo" );
    if( User::isAdmin() ) {
		$tabs->addTab( "men",      "<font color=green>М</font>",     "base.php?id=men" );
		$tabs->addTab( "ladies",   "<font color=green>Ж</font>",    "base.php?id=ladies" );
		$tabs->addTab( "countries", "<font color=green>Страны</font>",       "base.php?id=countries" );
    }
    $tabs->addTab( "cities",    "Города",       "base.php?id=cities" );
    
    $tabs->addTab( "clubs",    "Клубы",        "base.php?id=clubs" );
    $tabs->addTab( "teachers", "Тренеры",      "base.php?id=teachers" );
    if( User::isAdmin() ) {
		$tabs->addTab( "halls",    "<font color=green>Залы</font>",         "base.php?id=halls" );
		$tabs->addTab( "tools",    "<font color=red>Tools</font>",  "base.php?id=tools" );
	}
    $s .= $tabs->produce();   

    $s .= "<p>";

    switch( $section ) {
      case 'events':
        $s .= $this->makeEventsTable();
        break;

      case 'clubs':
        $s .= $this->makeClubsTable( new BaseColumnClub() );
        break;

      case 'cities':
        $s .= $this->makeCityCountryTable( new BaseColumnCity() );
        break;

      case 'countries':
        $s .= $this->makeCityCountryTable( new BaseColumnCountry() );
        break;

      case 'couples':
        $s .= $this->makeCouplesTable();
        break;


      case 'solo':
        $s .= $this->makeSoloTable();
        break;

      case 'men':
        if( Motl::getParameter( "editmode" ) ) {
          $prototype = new DbPersonProducer();
          $s .= $prototype->makeAddLink( null, array("sex"=>"M") );
        }
        $s .= $this->makeDancersTable( "base.php?id=$id", "sex='M'" );
        break;

      case 'ladies': 
		set_time_limit(60);
        if( Motl::getParameter( "editmode" ) ) {
          $prototype = new DbPersonProducer();
          $s .= $prototype->makeAddLink( null, array("sex"=>"F") );
        }
        $s .= $this->makeDancersTable( "base.php?id=$id", "sex='F'" );
        break;

      case 'teachers':
        if( Motl::getParameter( "editmode" ) ) {
          $prototype = new DbPersonProducer();
          $s .= $prototype->makeAddLink( null, array("isTeacher"=>"1") );
        }
        $s .= $this->makeTeachersTable( "base.php?id=$id" );
        break;

      case 'halls':
        if( Motl::getParameter( "editmode" ) ) {
          $prototype = new DbHallProducer();
          $s .= $prototype->makeAddLink( null );
        }
        $s .= $this->makeHallsTable( "base.php?id=$id" );
        break;

      case 'tools':
        $s .= $this->makeTools();
        break;
        
      default:
        $s .= $this->makeDisclaimer();
        break;
    }
    
    return $s;
  }	
  
  //----------------
  
  function makeDisclaimer() {
    $s = "";
    $s .= "О замеченных в базе неточностях просьба сообщать по адресу <A href='mailto:admin@danceleague.ru'>admin@danceleague.ru</a>";
    return $s;
  }
  
  //----------------
  
  function makeEventsTable() {
    $s = "";

    $dep = new DbEventProducer();
    
    $cond = Motl::getParameter( "editmode" ) ? "(pid Is Null OR pid=0)"
                                             : "(pid Is Null OR pid=0) AND (hidden Is Null OR not hidden) AND (regOpen OR haveScores OR havePlaces )";
                                          
    // COD $cond
    $s .= $dep->produceMultiple( "short", $cond );
    
    //----------
    
    return $s;
  }
  
  //----------------
  
  function makeCityCountryTable( $baseColumn ) {
  
    $ifNotMotl = "[if|motl||style='color:#9090c0;']";
    $letter = $baseColumn->getIdCode();
	
	$colName = new Column( "name",
							$baseColumn->getRusName(),
							"[editbar|edit,del] <A $ifNotMotl href=db.php?id=$letter"."[plain|id]>[plain|name][if|disambig| <i>([plain|disambig])</i>]</a>",
							"[plain|name]" );
							
	return $this->makeCccTable( $baseColumn, array($colName) );
  }
  
  //----------------
  
  function makeClubsTable( $baseColumn ) {
  
    $ifNotMotl = "[if|motl||style='color:#9090c0;']";
    $letter = $baseColumn->getIdCode();
	
	$colName = new Column( "name",
							$baseColumn->getRusName(),
							"[editbar|edit,del] <A $ifNotMotl href=db.php?id=$letter"."[plain|id]>[plain|name][if|disambig| <i>([plain|disambig])</i>]</a>",
							"[plain|name]" );
    $colCity = new Column( "city",    "город",   "[plain|textCity]", Column::NoSort );
    $colBoss = new Column( "main",    "руководитель",  "[plain|textBoss]", Column::NoSort );
							
	return $this->makeCccTable( $baseColumn, array($colName,$colCity,$colBoss) );
  }
  
  //----------------
  
  function makeCccTable( $baseColumn, $nameColumns ) {
	require_once "subjectUtils.inc";
	
    $editmode = Motl::getParameter( "editmode" );
	
  //phpinfo();
    $url = $_SERVER["REQUEST_URI"];
  
    $s = "";

    //----------
    
//    $letter = $baseColumn->getIdCode();
    $field = $baseColumn->getName();
    $table = $baseColumn->getTableName();
    $producerClass = $baseColumn->getProducerClass();

    //----------

	//$motlPeople = ! User::isAdmin();
	$motlPeople = 1;
	$xMotlCond = $motlPeople ? "x.motl" : "1";
	
    $sql = "SELECT t.*, Qc.N AS nCouples, Qs.N AS nSolo "
		."FROM $table as t "
			."LEFT JOIN (SELECT p.id2, count(*) AS N "
				."FROM db_link as p INNER JOIN db_couples AS x ON p.id1=x.id "
				."WHERE p.tag='couple-$field' And $xMotlCond "
				."GROUP BY p.id2) Qc ON t.id=Qc.id2 "
			."LEFT JOIN (SELECT p.id2, count(*) AS N "
				."FROM db_link as p INNER JOIN db_person AS x ON p.id1=x.id "
				."WHERE p.tag='person-$field' And $xMotlCond "
				."GROUP BY p.id2) Qs ON t.id=Qs.id2";
	//die($sql);
				
	if( ! User::isAdmin() ) {
		$sql .= " WHERE motl<>0";
	}
		  
    //$sql = "SELECT t.*, 0 as p, 0 as s FROM $table as t";
    
    //die($sql);
	
    $rows = MotlDB::queryRows( $sql ); // $field SAFE
    
    //----------
    SubjectUtils::addCCCTtoRows( $rows );
    //----------
    
    if( Motl::getParameter( "editmode" ) ) {
      $prototype = new $producerClass();
      $s .= $prototype->makeAddLink( null );
    }
        
    //----------

    $template = array();

      $colId       = new Column( "id",       "id",      "[plain|id]" );
	  
	if( $editmode ) {
		$template[] = $colId;
	}
	
	foreach( $nameColumns as $col ) {
		$template[] = $col;
	}
	
	if( User::isAdmin() ) {
		$template[] = $colNCouples = new Column( "nCouples", "N пар",   "[plain|nCouples]" );
		$template[] = $colNSolo    = new Column( "nSolo",    "N солистов",   "[plain|nSolo]" );
	}

    //----------
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "name" );
    $editmode = Motl::getParameter( "editmode" );
    $idParam = Motl::getParameter("id");
    $url = "base.php?id=$idParam";

    $s .= $table->produce( $producerClass, $rows, $url );
    //----------
    
    return $s;
  }
  
  //----------------
  // SQLPARAM   $cond
  
  public function makeDancersTable( $url, $cond ) {
	require_once "subjectUtils.inc";
	require_once "tables.inc";
  
    $editmode = Motl::getParameter( "editmode" );
  
    $s = "";

    //----------
/*
    $sql = "SELECT *, (SELECT count(*) FROM db_couples WHERE idMan=db_person.id OR idLady=db_person.id) AS nCouples "
          ."FROM db_person WHERE $cond";
*/        
    $sql = "SELECT *, "
//            ."(SELECT count(*) FROM db_couples WHERE idMan=db_person.id AND (split is null OR not split))"
//             ."+(SELECT count(*) FROM db_couples WHERE idLady=db_person.id AND (split is null OR not split)) AS nCouples "
."0 AS nCouples "
          ."FROM db_person WHERE $cond";
		  
    $sql = "SELECT * "
."FROM db_person p LEFT JOIN ( "
	."SELECT id1, id2 AS nCouples "
	."FROM db_link "
	."WHERE tag='person-nCouples' "
.") cntQ ON p.id=cntQ.id1 "
."WHERE $cond";
//die($sql);
    $rows = MotlDB::queryRows( $sql ); // $cond SAFEPARAM
    //----------

    $colSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );
    
    $colId       = new Column( "id",       "id",      "[editbar|edit,copy,del] <A href=db.php?id=p[plain|id]>[plain|id]</a>", "[plain|id]" );
    $colName     = new Column( "name",     "Имя",     "[plain|name]" );
    $colSurname  = new Column( "surname",  "Фамилия", "[plain|surname]", "[plain|surname] [plain|name]" );
    $colBirth    = new Column( "birth",    "Дата рожд.", "[plain|birthDate]" );
    $colNCouples = new Column( "nCouples", "N пар",   "[plain|nCouples]" );
    
    $template = $editmode ? array( $colId, $colSel, $colSurname, $colName, $colBirth, $colNCouples )
                          : array( $colId,          $colSurname, $colName, $colBirth, $colNCouples );

    $s .= "<form action='baseTools.php' target='_blank' method='POST'>\n";
    
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "surname" );
    $s .= $table->produce( "DbPersonProducer", $rows, $url );

    if( $editmode ) {
      $s .= "<p>\n";
      $s .= "<input type='submit' name='deletePersons' value='Удалить'>\n";
      $s .= "<input type='submit' name='mergePersons' value='Объединить'>\n";
    }

    $s .= "</form>\n";

    //----------

    return $s;
  }
  
  //----------------
  
  function makeTeachersTable( $url ){
	require_once "subjectUtils.inc";
	require_once "tables.inc";
  
    $editmode = Motl::getParameter( "editmode" );
  
    $s = "";

    //----------
/*
    $sql = "SELECT *, (SELECT count(*) FROM db_couples WHERE idMan=db_person.id OR idLady=db_person.id) AS nCouples "
          ."FROM db_person WHERE $cond";
*/        
    $sql = "SELECT t.*, "
                 ."(SELECT count(*) FROM db_couples as p "
                  ."WHERE concat(',',p.teacher,',') like concat('%,',t.id,',%')) as nCouples, "
                 ."(SELECT count(*) FROM db_person as s "
                  ."WHERE s.teacher<>'' and concat(',',s.teacher,',') like concat('%,',t.id,',%')) as nSolo "
          ."FROM db_person as t WHERE isTeacher<>''";
		  
    $sql = "SELECT t.*, 
                 (SELECT count(*) FROM db_link as p 
					INNER JOIN db_couples AS x ON p.id1=x.id 
                  WHERE p.tag='couple-teacher' And p.id2=t.id And x.motl 
                  ) as nCouples, 
                 (SELECT count(*) FROM db_link as p 
					INNER JOIN db_person AS x ON p.id1=x.id 
                  WHERE p.tag='person-teacher' And p.id2=t.id And x.motl 
                  ) as nSolo 
			FROM db_person as t 
			WHERE 
				( motl_teacher Or 
				  EXISTS (SELECT * FROM db_link as p 
					WHERE p.tag In('couple-teacher','person-teacher') 
						And p.id2=t.id)
				)";
	if( ! User::isAdmin() ) {
		$sql .= " And motl_teacher ";
	}
		  
    //die($sql);
    $rows = MotlDB::queryRows( $sql );
    //----------
	
    $ifNotMotl = "[if|motl_teacher||style='color:#9090c0;']";

    $colSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );
    
    $colId       = new Column( "id",       "id",      "[editbar|edit,del] <A href=db.php?id=p[plain|id]>[plain|id]</a>", "[plain|id]" );
    $colSurname  = new Column( "surname",  "Фамилия", "<A $ifNotMotl href=db.php?id=p[plain|id]>[plain|surname]</a>", "[plain|surname] [plain|name]" );
    $colName     = new Column( "name",     "Имя",     "[plain|name]" );
    $colNCouples = new Column( "nCouples", "N пар",   "[plain|nCouples]" );
    $colNSolo    = new Column( "nSolo",    "N солистов",   "[plain|nSolo]" );
    
    $template = $editmode ? array( $colSel, $colSurname, $colName, $colNCouples, $colNSolo )
                          : array(          $colSurname, $colName, $colNCouples, $colNSolo );
	if( ! User::isAdmin() ) {
		$template = array( $colSurname, $colName );
	}

    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "surname" );
    $s .= $table->produce( "DbPersonProducer", $rows, $url );

    //----------

    return $s;
  }
  
  //----------------
  
  function makeHallsTable( $url ){
	require_once "tables.inc";
  
    $editmode = Motl::getParameter( "editmode" );
  
    $s = "";

    //----------

    $sql = "SELECT * FROM db_halls";

    $rows = MotlDB::queryRows( $sql );
    //----------

    $colSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );
    
    $colId       = new Column( "id",       "id",      "[editbar|edit,del] <A href=db.php?id=h[plain|id]>[plain|id]</a>", "[plain|id]" );
    $colName     = new Column( "name",     "Название",     "<A href=db.php?id=h[plain|id]>[plain|title]</a>" );
    
    $template = $editmode ? array( $colSel, $colId, $colName )
                          : array(                  $colName );

    $s .= "<form action='baseTools.php' method='POST'>\n";
    
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "surname" );
    $s .= $table->produce( "DbHallProducer", $rows, $url );

    if( $editmode ) {
      $s .= "<input type='submit' name='deletePersons' value='Удалить'>\n";
      $s .= "<input type='submit' name='mergePersons' value='Объединить'>\n";
    }

    $s .= "</form>\n";

    //----------

    return $s;
  }
  
  //----------------
  // SQLPARAM   $sIds

  function makeCouplesTable( $sIds=null ){
  
	require_once "subjectUtils.inc";
	require_once "tables.inc";

    $editmode = Motl::getParameter( "editmode" );

    $s = "";

    //---------- Count couples

	$sql = "SELECT count(*) FROM db_couples";
	if( ! User::isAdmin() ) {
		$sql .= " WHERE motl<>0 and (split is null or not split)";
		$pageSize = 20;
	}
	else {
		$pageSize = 50;
	}
//die($sql);	
	$nCouples = MotlDB::queryValue( $sql );
	$nPages = ceil($nCouples / $pageSize);
	
    $tablePage = Motl::getParameter( "tablePage", 0 );
	$tablePage = min( $tablePage, $nPages-1 );
	$tablePage = max( $tablePage, 0 );
	$pagePos = $pageSize*$tablePage;
	
    //----------
	
	$sort = Motl::getParameter( "sort", "man" );
	
	$order = "Couple.id";
	switch( $sort ) {
		case 'man':  $order = "msurname,mname,fsurname,fname"; break;
		case 'lady': $order = "fsurname,fname,msurname,mname"; break;
		case 'regs': $order = "nRegs"; break;
	}
	
    //----------

    $sql = "SELECT Couple.id,Couple.split,"
              ."Man.id as mid, Man.surname as msurname, Man.name as mname,"
              ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname, "
			  ."Couple.country,Couple.city,Couple.club,Couple.teacher,"
              ."(SELECT count(DISTINCT db_events.pid) FROM db_couple_event INNER JOIN db_events ON db_couple_event.idEvent=db_events.id "
               ."WHERE idCouple=Couple.id and db_events.type='competition') AS nRegs, "
              ."EXISTS (SELECT * FROM db_couple_event AS ce INNER JOIN db_events AS e ON ce.idEvent=e.id WHERE ce.idCouple=Couple.id AND idGallery<>'') AS hasMedia "
          ."FROM db_couples AS Couple "
              ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
              ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id";


    $sql = "SELECT Couple.id,Couple.split,Couple.motl,"
              ."Man.id as mid, Man.surname as msurname, Man.name as mname,"
              ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname, "
			  ."Couple.country,Couple.city,Couple.club,Couple.teacher,"
			  
//              ."0 AS nRegs, "
              ."(SELECT id2 FROM db_link As p FORCE INDEX( Index_id1_tag) WHERE p.tag='couple-nEvents' And p.id1=Couple.id) AS nRegs, "
			  

//			   ."0 AS hasMedia "
              ."EXISTS (SELECT * FROM db_link As p FORCE INDEX( Index_id1_tag) WHERE p.tag='couple-haveMedia' And p.id1=Couple.id) AS hasMedia "

          ."FROM db_couples AS Couple "
              ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
              ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id ";
			  
	if( ! User::isAdmin() ) {
		$sql .= " WHERE Couple.motl<>0 and (split is null or not split)";
	}

	$sql .= "ORDER BY $order "
		   ."LIMIT $pagePos,$pageSize";
              
    /*if( $sIds ) {
      $sql .= " WHERE Couple.id In($sIds)";
    }*/
	
    //die($sql);
    $rows = MotlDB::queryRows( $sql ); // IFSAFE $sIds

    //----------
	
    SubjectUtils::addCCCTtoRows( $rows );
	
    //----------
    $ifSplit = "[if|split|style='text-decoration:line-through underline;']";
    $ifSplit = "[if|split|style='color:#9090c0;']";
    $ifSplit = "[if|split|style='font-style:italic;text-decoration:line-through underline;color:#b0b040;']";
    $ifSplit = "[if|split|style='color:#b0b040;']";
    //----------
    $ifNotMotl = "[if|motl||style='color:#9090c0;']";

    $colSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );

    $camera = "<img lign=middle eight=13 vspace=0 hspace=0 border=0 src='syspictures/camera.png'>";
    $colMedia   = new Column( "media","$camera",         
                              "[if|hasMedia|<A href='#' onclick='return false' title='Для этой пары есть фото/видео'>$camera</a>|]", Column::NoSort );
    
    $colInfo    = new Column( "id",     "&nbsp;",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]><small>подробнее...</small></a>", Column::NoSort );
    $colId      = new Column( "id",   "id",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]>[plain|id]</a>", "[plain|id]" );
	
    $colMan     = new Column( "man",  "партнер",    "<nobr><A $ifNotMotl $ifSplit href=db.php?id=p[plain|mid]>[plain|msurname] [plain|mname]</a></nobr>", "[plain|msurname] [plain|mname]" );
    $colLady    = new Column( "lady", "партнерша",  "<nobr><A $ifNotMotl $ifSplit href=db.php?id=p[plain|fid]>[plain|fsurname] [plain|fname]</a></nobr>", "[plain|fsurname] [plain|fname]" );
    $colRegs    = new Column( "regs", "N <sub>соревн.</sub>",  "[plain|nRegs]" );

    $colCountry = new Column( "country", "страна",  "[plain|textCountry]", Column::NoSort );
    $colCity    = new Column( "city",    "город",   "[plain|textCity]", Column::NoSort );
    $colClub    = new Column( "club",    "клуб",    "[plain|textClub]", Column::NoSort );
    $colTeacher = new Column( "teacher", "тренер",  "[plain|textTeacher]", Column::NoSort );
    
    $template = $editmode ? array( $colSel, $colMedia, $colId, $colMan, $colLady, $colRegs,
								   $colCountry, $colCity, $colClub )
                          : array(          $colMedia, $colInfo, $colMan, $colLady, $colRegs,
								   $colCountry, $colCity, $colClub );
	if( ! User::isAdmin() ) {
		$template = array(  $colMedia, $colInfo, $colMan, $colLady, 
								   $colCountry, $colCity, $colClub );
	}
    
    //$template = array( $colMan, $colLady, $colRegs );

    $s .= "<form action='baseTools.php' method='POST'>\n";

	
	$s .= $this->makeRuler( $nPages, $tablePage, "base.php?id=couples&sort=$sort" );
	
    $table = new Table( $template );
    //$table->sortBy = $sort;
    $s .= $table->produce( "DbCoupleProducer", $rows, "base.php?id=couples&tablePage=$tablePage" );
    
    if( $editmode ) {
      $s .= "<p><input type='submit' name='deleteCouples' value='Удалить'>\n";
    }

    $s .= "</form>\n";
    
    return $s;
  }
  
  //----------------
  
  function makeRuler( $nPages, $curPage, $baseUrl ) {
	$items = array();
	for( $k=0; $k<$nPages; $k++ ) {
		$k1 = $k+1;
		if( $k == $curPage ) {
			$item = "<b>$k1</b>";
		}
		else {
			$url = "$baseUrl&tablePage=$k";
			$item = "<A href='$url'>$k1</a>";
		}
		$items[] = $item;
	}
	$sitems = implode( " ", $items );
	return $sitems;
  }
  
  //----------------
 
  function makeSoloTable(){
	require_once "subjectUtils.inc";
	require_once "tables.inc";
	
    $editmode = Motl::getParameter( "editmode" );

    $s = "";

    //----------

    /*$sql = "SELECT Dancer.id, surname, name, country,city,club,teacher,motl,"
              ."(SELECT count(DISTINCT db_events.pid) FROM db_couple_event INNER JOIN db_events ON db_couple_event.idEvent=db_events.id "
               ."WHERE idSolo=Dancer.id and db_events.type='competition') AS nRegs "
          ."FROM db_person AS Dancer "
          ."WHERE EXISTS (SELECT * FROM db_couple_event WHERE idSolo=Dancer.id)";
	*/
	$sql = "SELECT Dancer.id, surname, name, country,city,club,teacher,motl,"
				."db_link.id2 AS nRegs "
			."FROM db_person AS Dancer "
				."INNER JOIN db_link ON Dancer.id=db_link.id1 "
			."WHERE db_link.tag='person-nEvents'";
			
	if( ! User::isAdmin() ) {
		$sql .= " And Dancer.motl<>0 ";
	}
      //die($sql);
    $rows = MotlDB::queryRows( $sql );
                              
    //----------
	
    SubjectUtils::addCCCTtoRows( $rows );
	
    //----------
	
    $ifNotMotl = "[if|motl||style='color:#9090c0;']";
	
    $colId      = new Column( "id",   "id",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]>[plain|id]</a>", "[plain|id]" );
    $colDancer  = new Column( "dancer", "фамилия, имя",    "<nobr><A $ifNotMotl href=db.php?id=p[plain|id]>[plain|surname] [plain|name]</a></nobr>", "[plain|surname] [plain|name]" );
    $colRegs    = new Column( "regs", "N <sub>соревн.</sub>",  "[plain|nRegs]" );

    $colCountry = new Column( "country", "страна",  "[plain|textCountry]",  "[plain|textCountry0]" );
    $colCity    = new Column( "city",    "город",   "[plain|textCity]",  "[plain|textCity0]" );
    $colClub    = new Column( "club",    "клуб",    "[plain|textClub]",  "[plain|textClub0]" );
    $colTeacher = new Column( "teacher", "тренер",  "[plain|textTeacher]",  "[plain|textTeacher0]" );
    
//    $template = array( $colId, $colDancer, $colRegs );
    $template = array( $colDancer, $colRegs, $colCountry, $colCity, $colClub );
	if( ! User::isAdmin() ) {
		$template = array( $colDancer, $colCountry, $colCity, $colClub );
	}
    
    $table = new Table( $template );
    $table->sortBy = Motl::getParameter( "sort", "dancer" );
    $s .= $table->produce( "DbCoupleProducer", $rows, "base.php?id=solo" );
    
    return $s;
  }
  
  //----------------

  
  function makeTools() {
    $s = "";
    
    $s .= "<h2>Проверки базы танцоров:</h2>";
    $s .= "<A href='baseToolCheckSex.php'>пол</a><br>";
    $s .= "<A href='baseTools.php?checkNames=1'>имена</a><br>";
    $s .= "<A href='baseToolCheckOrphanPeople.php'>\"мертвые души\"</a><br>";
    $s .= "<A href='baseToolCheckOrphanCouples.php'>пары без танцоров</a><br>";
    $s .= "<A href='baseTools.php?checkDupCouples=1'>пары - дубли</a><br>";
    $s .= "<p>";
    $s .= "<A href='baseToolCheckOrphanLinks.php'>мертвые ссылки</a><br>";
    $s .= "<p>";

    
    $s .= "<h2>Нормализация:</h2>";
    
    $s .= "<table class='MainText' style='margin-left:50'>";
    $s .= "<tr><td width=200> <b>В регистрации:</b> </td>" 
             ."<td> <b>В базе:</b> </td> </tr>";
    
    $s .= "<tr><td> <A href='baseToolNormalizeField.php?fieldName=country'>страна</a> </td>" 
             ."<td> <A href='baseToolNormalizeSimilar.php?type=country'>страна</a> </td> </tr>";
    
    $s .= "<tr><td> <A href='baseToolNormalizeField.php?fieldName=city'>город</a> </td>" 
             ."<td> <A href='baseToolNormalizeSimilar.php?type=city'>город</a> </td> </tr>";

    $s .= "<tr><td> <A href='baseToolNormalizeField.php?fieldName=club'>клуб</a> </td>" 
             ."<td> <A href='baseToolNormalizeSimilar.php?type=club'>клуб</a> </td> </tr>";

    $s .= "<tr><td> <A href='baseToolNormalizeField.php?fieldName=teacher'>преподаватель</a> </td>" 
             ."<td> <A href='baseToolNormalizeSimilar.php?type=person'>человек</a> </td> </tr>";

    $s .= "</table>";
    $s .= "<p>";

    $s .= "<h2>Привязка:</h2>";
    $s .= "<A href='baseToolLinkHierarchy.php?show=twin'>иерархия</a><br>";
    $s .= "<A href='baseToolLinkField.php?subject=couple'>пары</a><br>";
    $s .= "<A href='baseToolLinkField.php?subject=solo'>солисты</a><br>";
    $s .= "<A href='baseToolSetAge.php'>ДР</a><br>";
    $s .= "<A href='baseToolSetLinks.php'>SetLinks</a><br>";
    $s .= "<A href='baseToolSetMotl.php'>МОТЛ</a><br>";
    
    $s .= "<h2>Проверка базы турниров:</h2>";
    $s .= "<A href='baseToolCheckDupRegs.php'>Двойная регистрация</a><br>";
    $s .= "<A href='baseToolCheckScores.php'>Оценки</a><br>";
    $s .= "<A href='baseToolCheckGroups.php'>Группы</a><br>";
    $s .= "<A href='baseToolCheckClasses.php'>Классы</a><br>";
    $s .= "<A href='baseToolCheckAge.php'>Возраст</a><br>";

    $s .= "<h2>Прочее:</h2>";
    $s .= "<A href='baseToolCompareBases.php'>сравнить базы</a><br>";
    $s .= "<A href='baseTools.php?mergePersons'>mergePersons</a><br>";
    
    return $s;
  }

  //----------------
}

//=====================
?>
