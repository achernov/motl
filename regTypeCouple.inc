<?php
//=====================
require_once "regType.inc";
//die("regTypeCouple.inc");
//=====================

class RegTypeHandlerCouple extends RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
    return $code==null || $code=="" || $code=="couple";
  }
  
  //===========
  
  public function getCanonicalCode() {
    return "couple";
  }
  
  //===========
  
  public function getLabel() {
    return "пары";
  }
  
  //===========
  
  public function makeResultValue( $compRow, $ceRow ) {
    $nCouples = $compRow->ncouples;
    $place    = $ceRow->place;
    $num = $compRow->number ? ", &nbsp; №".$compRow->number : "";
    
	if( User::isAdmin() ) {
		$class = $compRow->class ? ", &nbsp; класс - $compRow->class" : "";
	}
	else {
		$class = "";
	}
	
    return "$place из $nCouples$num$class";
  }
  
  //===========
  
  public function makeForm( $idEvent ) {
  
	require_once "registrator.inc";
	
	$registrator = new Registrator();

    return $registrator->makeCoupleForm( $idEvent );
  }
  
  //===========
  
  public function processUserInput() {
  
	require_once "registrator.inc";
	
	$registrator = new Registrator();
  
    return $registrator->processUserInputCouple($this);
    
  }
  
  //===========
  
  public function makeRegList( $compProducer ) {
    $editmode = Motl::getParameter( "editmode", 0 );

    $rows = $this->loadDancersTable( $compProducer );

    return $this->makeDancersTable( $compProducer, $rows, MODE_REGLIST, $editmode );
}
  
  //===========
  
  private function loadDancersTable( $compProducer ) {
  
    $thisId = (int) $compProducer->row->id;
    
    $rows = MotlDB::queryRows( "SELECT Event.*,"
                                  ."Man.id as mid, Man.surname as msurname, Man.name as mname, Man.birthdate as mbirth,"
                                  ."Lady.id as fid, Lady.surname as fsurname, Lady.name as fname, Lady.birthdate as fbirth "
                              ."FROM ((db_couple_event AS Event INNER JOIN db_couples AS Couple ON Event.idCouple=Couple.id) "
                                  ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id) "
                                  ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
                              ."WHERE idEvent='$thisId'" );  
//	print_r($rows);
    return $rows;
  }
  
  //===========

  private function makeTemplate( $compProducer, $cols, $mode, $edit ) {

	$e = $edit ? "1" : "0";
  
	switch( $mode.$e ) {
		case MODE_REGLIST."0":
			return array(                     
                        $cols->colNumber,  
                        $cols->colClass,  
                        $cols->colMan, 
                        $cols->colLady, 
//                      $cols->colCountry,  
                        $cols->colCity,  
                        $cols->colClub, 
						$cols->colBoss,
                        $cols->colTeacher 
                      );
					  
		case MODE_REGLIST."1":
			return array( 
						$cols->ecolSel,
						$cols->ecolEdit, 
						$cols->ecolNumber, 
						$cols->colMan, 
						$cols->colManBirth,
						$cols->colLady, 
						$cols->colLadyBirth,
						$cols->ecolClass,  
						$cols->colCountry,  
						$cols->colCity,  
						$cols->colClub, 
						$cols->colTeacher 
                      );
					  
		case MODE_RESULT_COMMON."0":
			return array(                     
						$cols->colPlace, 
						$cols->colNumber, 
						$cols->colClass,  
//						$cols->colPoints,
						$cols->colMan, 
						$cols->colLady, 
//	                        $cols->colCountry,  
						$cols->colCity,  
						$cols->colClub, 
						$cols->colBoss,
						$cols->colTeacher 
                      );
					  
		case MODE_RESULT_COMMON."1":
			return array( 
						$cols->ecolSel,
						$cols->ecolEdit, 
						$cols->ecolPlace, 
						$cols->ecolNumber, 
						$cols->ecolClass,  
						$cols->colMan, 
						$cols->colLady, 
                        $cols->ecolCountry,  
                        $cols->ecolCity,  
						$cols->ecolClub, 
						$cols->ecolTeacher 
                      );
					  
		case MODE_RESULT_SCORES."0":
			$tpl = array(                     
						$cols->colPlace, 
						$cols->colNumber, 
						$cols->colMan, 
						$cols->colLady 
                      );
			$this->appendDances( $compProducer, $tpl, $mode, $edit );
			return $tpl;
					  
		case MODE_RESULT_SCORES."1":
			$tpl = array( 
						$cols->ecolSel,
						$cols->ecolEdit, 
						$cols->ecolPlace, 
						$cols->colMan, 
						$cols->colLady, 
						$cols->ecolNumber,
                      );
			$this->appendDances( $compProducer, $tpl, $mode, $edit );
			return $tpl;
	}
	
	die( "ERROR in makeTemplate()" );
  }
  
  //===========
  
  private function appendDances( $compProducer, &$template, $mode, $edit ) {
  
		//----- Setup dances

		$dances = $compProducer->getDances();
		$nDances = count($dances);
		
		//----- Append dances

		if( $edit ) {
			$spreadScores = "spreadScores(this,[plain|#],$nDances)";
			$template[] = new Column( "spread", "A","[atext|spread|style='width:50' onclick='$spreadScores' onblur='$spreadScores']", Column::NoSort );
 		}

		for( $k=0; $k<$nDances; $k++ ) {
		  $ds = "123456789a";
		  $d = $ds[$k];
		  $fname = "d$d";
		  $dname = $dances[$k];
		  
		  $spreadColumn = "spreadColumn(this,\"$d\")";
		  $dspread = "<textarea style='width:50' rows=1 onclick='$spreadColumn;return false' onblur='$spreadColumn'></textarea>";
		  
		  if( $edit ) {
			$template[] = new Column( "d$d", $dname.$dspread,"[atext|$fname|style='width:150' onclick='normalizeScores(this)' onblur='normalizeScores(this)']" );
		  }
		  else {
			$template[] = new Column( "d$d", $dname,"[plain|$fname|id='scores_[plain|id]_$k' style='font-family:\"Courier New\";']" );
		  }
		}

		$crossesHere =  ( $mode==MODE_RESULT_SCORES && $compProducer->row->round );
		
		if( $crossesHere && !$edit) {
			$template[] = new Column( "crosses", "кресты", "[plain|crosses]", "crossCmp" );
		}
	}
	
  //===========
  
  public function makeResults( $compProducer ) {
  
    $editmode = Motl::getParameter( "editmode", 0 );
    
    //----- Choose View Mode
	
    switch( $compProducer->row->type ) {
	
      case 'competition': 
		$mode = MODE_RESULT_COMMON;
		break;
		
      case 'round':       
		$mode = MODE_RESULT_SCORES;
		break;
    }
    
    $s = "";
    
    //----- Make tabs

    $thisId = (int) $compProducer->row->id;
	$s .= $this->makeModeSelector( $compProducer, $editmode, $thisId );

    //----- Make Judges
	
	  $judges = $this->makeJudges( $compProducer );
	  if( $judges ) {
		$s .= "Судьи: <i>". $judges . "</i><br>";
	  }

    //----- Make Table
	
	$dances = $compProducer->getDances();
	$nDances = count($dances);
	
    if( $editmode ) {
      $s .= $this->makeEditFormStart();
    }
	
	$rows = $this->loadDancersTable( $compProducer );
	
    $s .= $this->makeDancersTable( $compProducer, $rows, $mode, $editmode );
	
    if( $editmode ) {
		$s .= $this->makeEditFormEnd( $compProducer, $rows, $dances );
	}
	
    //---------- Make JS gimmicks
	
	$regIds = array();
	foreach( $rows as $row ) {
		$regIds[] = $row->id;
	}
	$ajudges = $this->loadJudges( $compProducer );

	$s .= $this->makeScoreTipJS( $regIds, $ajudges, $nDances );
	
    
    return $s;
  }
  
  //===========
  
  private function makeEditFormStart() {
    return "<form action='dbProcessEventCouples.php' method='POST'>\n";
  }
  
  //===========
  
  private function makeEditFormEnd( $compProducer, $rows, $dances ) {
  
    $s = "";
    
    $s .= "<input type='submit' name='save' value='Save' style='margin-right:100'>\n";

    $s .= $compProducer->makeText( null, "[select|round|final:0;1/2:1;1/4:2;1/8:3;1/16:4;1/32:5;1/64:6]" );
    $s .= "<input type='submit' name='makeRound' value='Make round'>";
    $s .= "<input type='submit' name='makeFill' value='Fill round'>";
    
    $s .= "&nbsp; &nbsp; &nbsp; &nbsp; \n";

    $s .= "<input type='submit' name='deleteSelected' value='Delete selected' onclick='return confirmKills()'>\n";

    $s .= "<p>&nbsp;<p>\n";

    $returl = Motl::makeRetUrlPost();
    $s .= "<input type='hidden' name='returl' value='$returl'>\n";

    $s .= $compProducer->makeText( null, "Партнер [text|man] Партнерша [text|lady]" );
    $idParam = Motl::getParameter("id");
    $thisId = $compProducer->row->id;    
    $s .= "<input type='hidden' name='idParam' value='$idParam'>\n";
    $s .= "<input type='hidden' name='idEvent' value='$thisId'>\n";
    $s .= "<input type='submit' name='addCouple' value='Добавить пару'>\n";

    $s .= "</form>\n";
    
    return $s;
  }
  
  //===========
  
  private function makeColumns( $enableBaseLink = false ) {
  
	if( getLocalSetting('DISABLE_BASE_LINK') ) {
		$enableBaseLink = false;
	}
	
    $obj = new stdClass();
    
    if( $enableBaseLink ) {
      $curl0 = "<A href='db.php?id=c[plain|idCouple]'>";
      $curl1 = "</a>";
  	}
  	else {
      $curl0 = "";
  	  $curl1 = "";
  	}
  
    $obj->ecolSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );
    $obj->ecolEdit   = new Column( "edit", "edit",               "[editbar|edit,del]" );

    $obj->ecolPlace  = new Column( "place", "место",               "[atext|place|style='width:40']", "placeCmp" );
    $obj->colPlace   = new Column( "place", "место",               "[plain|place]", "placeCmp" );

    $obj->colPoints  = new Column( "points", "<center>очки,<br>вклейка</center>",          
							"<small><A href='coupleResult.php?idCE=[plain|id]'>результат пары</a></small>", Column::NoSort );
	
    $obj->ecolNumber = new Column( "number", "номер",               "[atext|number|style='width:40']", "plain|number]" );
    $obj->colNumber  = new Column( "number", "номер",               "[plain|number]" );

    $obj->colMan     = new Column( "man", "партнер",           $curl0."<nobr>[plain|msurname] [plain|mname]</nobr>".$curl1, "[plain|msurname] [plain|mname]" );
    $obj->colLady    = new Column( "lady", "партнерша",       "<nobr>[plain|fsurname] [plain|fname]</nobr>" );
    
    $obj->colManBirth   = new Column( "mbirth", "дата р.",        "[plainDateAge|mbirth]" );
    $obj->colLadyBirth  = new Column( "fbirth", "дата р.",        "[plainDateAge|fbirth]" );

    $obj->ecolClass  = new Column( "class", "класс",        "[atext|class|style='width:60']" );
    $obj->colClass   = new Column( "class", "класс",        "<center>[plainClass|class]" );

    $obj->ecolCountry = new Column( "where", "страна",       "[atext|raw_country]" );
    $obj->colCountry  = new Column( "where", "страна",       "[plain|raw_country]" );

    $obj->ecolCity    = new Column( "where", "город",        "[atext|raw_city]" );
    $obj->colCity     = new Column( "where", "город",        "<nobr>[plain|raw_city]</nobr>" );

//	$test = "<br>Клюшин Николай";
	$test = "";
	
    $obj->ecolClub    = new Column( "club", "клуб",                 "[atext|raw_club]" );
    $obj->colClub     = new Column( "club", "клуб",                 "<nobr>[plain|raw_club]</nobr>" );

    $obj->colBoss     = new Column( "boss", "руководитель",                 "<nobr>[plain|raw_boss]</nobr>" );

    $obj->ecolTeacher = new Column( "teacher", "тренер",             "[atext|raw_teacher]" );
    $obj->colTeacher  = new Column( "teacher", "тренер",             "<nobr>[plain|raw_teacher]</nobr>" );
    
    return $obj;
  }
  
  //===========
  
	public function makeModeSelector( $compProducer, $edit, $current ) {
		$idComp = (int)( ($compProducer->row->type=='competition') ? $compProducer->row->id : $compProducer->row->pid );

		$idEvent = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$idComp'", 0 );  // COD $idComp

        $rounds = MotlDB::queryRows( "SELECT * FROM db_events "
                                    ."WHERE pid='$idComp' and type='round' and (hidden is null or not hidden) ORDER BY round" );  

        $roundLabels = array( "финал", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64" );
        //$roundLabels = array( "финал - оценки", "кресты 1/2", "кресты 1/4", "кресты 1/8", "кресты 1/16", "кресты 1/32", "кресты 1/64" );

        $tabs = new Tabs();

        $more = null;
        if( $edit ) {
          $elink = $compProducer->makeEditLink( "edit", $idComp );
          $klink = $compProducer->makeKillLink( "del", $idComp );
          $more = "$elink $klink ";
          $more = "$elink ";
        }
        $tabs->addTab( "$idComp", "места", "db.php?id=e$idEvent,r$idComp", null, $more );

        for( $k=0; $k<count($rounds); $k++ ) {
          $rLevel = $rounds[$k]->round;
          $rid    = $rounds[$k]->id;
		  
          $label = $rounds[$k]->title ? $rounds[$k]->title : $roundLabels[$rLevel];
		  $label = ($rLevel>0) ? "кресты $label" : "$label - оценки";
		  
          $more = null;
          if( $edit ) {
            $elink = $compProducer->makeEditLink( "edit", $rid );
            $klink = $compProducer->makeKillLink( "del",  $rid );
            $more = "$elink $klink ";
          }
          $tabs->addTab( "$rid", $label, "db.php?id=e$idEvent,r$rid", null, $more );
        }

        $tabs->current = $current;

        return $tabs->produce();   
	}
  
  //===========
  
  private function makeDancersTable( $compProducer, $rows, $mode, $edit=false ) {

    $s = "";

    //----------
	
	if( ! $edit ) {
		$this->substCodedCCCT( $rows );
	}
	
    //----------

    $defaultSort = "";

    $crossesHere =  ( $mode==MODE_RESULT_SCORES && $compProducer->row->round );
    $defaultSort = $crossesHere ? "crosses" : "place"; 

    $sort = Motl::getParameter( "sort", $defaultSort );

    //----- Load dances
    
    $dances = $compProducer->getDances();
    $nDances = count($dances);

    //----- Make template

    $cols = $this->makeColumns( 1 );
    $template = $this->makeTemplate( $compProducer, $cols, $mode, $edit );

    //---------- Count crosses

    for( $r=0; $r<count($rows); $r++ ) {
      $row = $rows[$r];
      $n = 0;
      for( $k=0; $k<$nDances; $k++ ) {
        $ds = "123456789a";
        $d = $ds[$k];
        $fname = "d$d";
        $n += $this->countCrosses( $row->$fname );
        //echo "$fname $d<br>";
      }
      //echo "<p>";
      $row->crosses = $n;
    }

    //----- Do hightlight
    
    $hc = Motl::getParameter( "hc", null );
    $hrow = null;
    
    if( ! is_null($hc) ) {
      for( $k=0; $k<count($rows); $k++ ) {
        if( $rows[$k]->idCouple == $hc )
          $hrow = $rows[$k];
      }
    }
    
    //----- Make table

    $table = new Table( $template );
	$templator = new TableTemplator();
	if( $compProducer->row->type == 'competition' ) {
		$templator->groupCode = $compProducer->row->groupCode;
	}
	$table->templator = $templator;
    if( $template ) {
      $script = $_SERVER['PHP_SELF'];
      $id = Motl::getParameter( "id" );

      $table->sortBy = $sort;

      $table->highlightRow = $hrow;

      $s .= $table->produce( "DbCoupleEventProducer", $rows, "$script?id=$id" );
	  $s .= "<div id='scoreTip' style='display:none;position:absolute;border:#B3B3B5 1px solid;background-color:#D5E5F4;'>ddd</div>";
    }

    //----------
    return $s;
  }
  
  //----------------

  private function countCrosses( $s ) {
    $s = trim( $s );
    $s1 = $s;
    $s1 = str_replace( "1", "", $s1 );
    $s1 = str_replace( "x", "", $s1 );
    return strlen($s) - strlen($s1);
  }

  //===========
  
  public function processListEdit( $compProducer ) {
    //print_r( $_REQUEST );die();
	$idEvent = Motl::getParameter("idEvent");
    $title = MotlDB::queryValue( "SELECT title FROM db_events WHERE id=$idEvent" );
	echo "<b style='color:#0000c0'>$title</b><p>";

    if( isset($_REQUEST["save"]) ) {
      $this->saveListEdit( $compProducer );
    }
    else
    if( isset($_REQUEST["addCouple"]) ) {
      $this->finishAddCouple( $compProducer );
    }  
    else
    if( isset($_REQUEST["makeRound"]) ) {
      $this->makeRound( $compProducer );
    }  
    else
    if( isset($_REQUEST["makeFill"]) ) {
      $this->makeFill( $compProducer );
    }  
    else
    if( isset($_REQUEST["processFill"]) ) {
      $this->processFill( $compProducer );
    }  
    else
    if( isset($_REQUEST["endFill"]) ) {
      $this->endFill( $compProducer );
      DataUtil::updateEventBase();
    }  
    else
    if( isset($_REQUEST["deleteSelected"]) ) {
      $this->deleteSelected( $compProducer );
    }  
    
  }
  
  //===========
  
  private function saveListEdit( $compProducer ) {
    //echo "<b>saveListEdit</b><p>";
   // print_r( $_REQUEST );
   // print_r( $compProducer );
    
    $ids = $_REQUEST['dbf_id'];

    for( $k=0; $k<count($ids); $k++ ) {
      $obj = new DbCoupleEventProducer();
      $obj->loadFromQuery( $k );
    //print_r( $obj );echo "<br>";
      $obj->savetoDB();
    }
    
    DataUtil::updateEventBase();
    Motl::redirectToRetUrl();
  }
  
  //===========
  
  private function finishAddCouple( $compProducer ) {
    $idEvent = Motl::getParameter( "idEvent", "" );
    $man     = Motl::getParameter( "dbf_man", "" );
    $lady    = Motl::getParameter( "dbf_lady", "" );

    //$fn = explode( " ", trim($dbf_dancer) );
    
	/*
    $idMan  = registerPerson($man);
    $idLady = registerPerson($lady);
    $idCouple = registerCouple( $idMan, $idLady );

    $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                array("idEvent"=>$idEvent, "idCouple"=>$idCouple),
                true );
     */
	 
	$ce = $this->openCoupleEventByNames( $idEvent, $man, $lady );
	$ce->saveToDB();
	 
//echo( "addCouple $idEvent $idCouple $idMan $idLady = $id" );
    DataUtil::updateEventBase();
    Motl::redirectToRetUrl();
  }
  
  //===========
  
  private function makeRound( $compProducer ) {
    //print_r( $_REQUEST );

    $ids     = $_REQUEST['dbf_id'];
    
    $selids = array();

    for( $k=0; $k<count($ids); $k++ ) {
      if( isset($_REQUEST["dbf_".$k."_check"]) ) {
        $selids[] = $ids[$k];
      }
    }

    $nids = count($selids);
    $sids = implode( ",", $selids );
    
    ASSERT_ints($sids);

    $eventIds = MotlDB::queryValues( "SELECT DISTINCT idEvent FROM db_couple_event WHERE id In($sids)" );  // ASS $sids
    if( count($eventIds) <1 ) {
      motlDie( "ERROR: no couples selected" );
    }
    if( count($eventIds) > 1 ) {
      motlDie( "ERROR: not the same event" );
    }

    $round = Motl::getIntParameter( "dbf_round" );
    
    if( ! strlen($round) ) {
      motlDie( "ERROR: round not selected" );
    }

    $idEvent = (int) $eventIds[0];

    $eventRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=$idEvent" );  // FLD $idEvent

    //print_r( $eventRow );

    $roundNames = array( "final", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64" );

    $newRow = array( "type"=>"round", "round"=>$round, "ncouples"=>$nids, "title"=>$roundNames[$round] );

    switch( $eventRow->type ) {
      case 'competition':
        $newRow["pid"] = $eventRow->id;
        break;

      case 'round':
        $newRow["pid"] = $eventRow->pid;
        break;

      default:
        motlDie( "Wrong event type" );
    }
    
    $idMain = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id=".$newRow["pid"] );  // FLD $newRow["pid"]
    
    /*echo "<hr>";
    print_r( $newRow );
    echo "<hr>";*/

    // FLD $newRow[pid], 
    // ASS $newRow[round]
    $delIds = MotlDB::queryValues( "SELECT id FROM db_events WHERE pid=$newRow[pid] and round=$newRow[round]" );  
    
    $dids = implode( ",", $delIds );
//die("here $dids");
    if( strlen($dids) ) {
      MotlDB::query( "DELETE FROM db_events WHERE id In($dids)" );  // FLD $dids
      MotlDB::query( "DELETE FROM db_couple_event WHERE idEvent In($dids)" );  // FLD $dids
    }

    $idNewEvent = MotlDB::saveRow( "db_events", "id",null, $newRow );

    // COD,FLD $idNewEvent
    // ASS $sids
    MotlDB::query( "INSERT INTO db_couple_event(idEvent,idCouple,number) SELECT $idNewEvent,idCouple,number FROM db_couple_event WHERE id In($sids)" );  

    DataUtil::updateEventBase();
    
    Motl::redirect( "db.php?id=e$idMain,r$idNewEvent" );
  }
  
  //===========
  
  private function makeFill( $compProducer ) {
    //print_r( $_REQUEST );
    //print_r( $compProducer );
    $round = Motl::getParameter( "dbf_round" );
    $idEvent = $compProducer->row->id;
   
    $s = "";

//    $s .= "<A href='db.php?id=e$idEvent'>вернуться</a>";
    
    $s .= "<form action='dbProcessEventCouples.php' method='POST'>\n";
    $s .= "<input type='hidden' name='idEvent' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='dbf_round' value='$round'>\n";
    $s .= "<table><tr valign=top>";
    if( ! strlen(trim($round) ) ) { // Протокол
		
		if( PROTOCOL_TYPE == PROTOCOL_TARUDKO ) {
			$s .= "<input type='hidden' name='protocolType' value='".PROTOCOL_TYPE."'>\n";
			//$s .= $this->makeFillArea( $compProducer, "place",   "place",     50 );
			$s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
			$s .= $this->makeFillArea( $compProducer, "man",     "Партнер",   200 );
			$s .= $this->makeFillArea( $compProducer, "lady",    "Партнерша", 200 );
			//$s .= $this->makeFillArea( $compProducer, "country", "Страна",    100 );
			$s .= $this->makeFillArea( $compProducer, "city",    "Город",     150 );
			$s .= $this->makeFillArea( $compProducer, "club",    "Клуб",      300 );
			$s .= $this->makeFillArea( $compProducer, "teacher", "Тренеры",   300 );
		}
		else
		if( PROTOCOL_TYPE == PROTOCOL_ALEKSEEV ) {
			$s .= "<input type='hidden' name='protocolType' value='".PROTOCOL_TYPE."'>\n";
			$s .= $this->makeFillArea( $compProducer, "place",   "place",     50 );
			$s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
			$s .= $this->makeFillArea( $compProducer, "man_lady","Партнер/Партнерша",   200 );
			$s .= $this->makeFillArea( $compProducer, "birthdates", "Даты рожд.",    100 );
			$s .= $this->makeFillArea( $compProducer, "class",    "Класс",    100 );
			$s .= $this->makeFillArea( $compProducer, "club_city",    "Клуб/Город",     150 );
			$s .= $this->makeFillArea( $compProducer, "teacher", "Тренеры",   300 );
		}
		else {
			die( "Wrong protocol type" );
		}
    }
    else {
		$dances = $compProducer->getDancesString();
      $adances = explode( ",", $dances );
      $s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
      $s .= $this->makeFillArea( $compProducer, "place",   "place",     50 );
      
	  if( $round > 0 ) {
		$s .= $this->makeFillArea( $compProducer, "dx", "Все танцы",   100,100 );
	  }
	  
      $ndances = count($adances);
      for( $k=0; $k<$ndances; $k++ ) {
        $d = $k+1;
        $s .= $this->makeFillArea( $compProducer, "d$d", $adances[$k],   300 );
      }
      $s .= "<input type='hidden' name='ndances' value='$ndances'>\n";
      $s .= "<input type='hidden' name='dances' value='$dances'>\n";
    }
    $s .= "</tr></table>\n";
    $s .= "<input type='submit' name='processFill' value='Дальше&gt;&gt;'>\n";
    $s .= "</form>\n";
    
    echo $s;
  }
  
  //===========
  
  private function processFill( $compProducer ) {
    //print_r( $_REQUEST );
    
    $idEvent = Motl::getParameter( "idEvent" );
    
    $round = Motl::getParameter( "dbf_round" );
    
    if( ! strlen(trim($round) ) ) { // Протокол
      $columns = array( 
                  array( "number",  "№" ),
                  array( "place",   "place" ),
                  array( "man",     "Партнер" ),
                  array( "lady",    "Партнерша" ),
                  array( "country", "Страна" ),
                  array( "city",    "Город" ),
                  array( "club",    "Клуб" ),
                  array( "teacher", "Тренеры" ) );
      $ndances = "";
      $dances  = "";
    }
    else {
      $columns = array( 
                  array( "number",  "№" ),
                  array( "place",   "place" ) );
                  
      $ndances = Motl::getParameter( "ndances" );
      $dances  = Motl::getParameter( "dances" );
      $adances = explode( ",", $dances );
      
      for( $k=0; $k<$ndances; $k++ ) {
        $d = $k+1;
        $columns[] = array( "d$d", $adances[$k] );
      }
    }
                
	if( Motl::getParameter("dx") ) {
		$this->expandDances(Motl::getParameter("ndances"));
	}
                
	if( Motl::getParameter("protocolType") == PROTOCOL_ALEKSEEV ) {
		$this->splitParameter( "number",  "number",  null );
		$this->splitParameter( "place",   "place",   null );
		$this->splitParameter( "teacher", "teacher", null );
		$this->splitParameter( "man_lady", "man", "lady" );
		$this->splitParameter( "birthdates", "mbirthdate", "fbirthdate" );
		$this->splitJoinedParameter( "club_city", "club", "city" );
	}
	
    $this->showFillTable( $columns );
    
    echo "<form action='dbProcessEventCouples.php' method='POST'>\n";
    echo "<input type='hidden' name='idEvent' value='$idEvent'>\n";
    echo "<input type='hidden' name='dbf_round' value='$round'>\n";
    echo "<input type='hidden' name='ndances' value='$ndances'>\n";
    echo "<input type='hidden' name='dances' value='$dances'>\n";
    for( $k=0; $k<count($columns); $k++ ) {
      $name  = $columns[$k][0];
      $value = Motl::getParameter( $name );
      echo "<input type='hidden' name='$name' value='$value'>\n";
    }
    echo "<input type='submit' name='endFill' value='Дальше&gt;&gt;'>\n";
    echo "</form>\n";
  }

  //-----------
  
  private function splitParameter( $name, $name1,$name2 ) {
	$values = $this->getArrayParameter( $name );
	
	$values1 = array();
	$values2 = array();
	
	for( $k=0; $k+1<count($values); $k+=2 ) {
		$values1[] = $values[$k];
		$values2[] = $values[$k+1];
	}
	$this->setParameter( $name1, implode("\r\n",$values1) );
	$this->setParameter( $name2, implode("\r\n",$values2) );
  }
  
  //-----------
  
  private function splitJoinedParameter( $name, $name1,$name2 ) {
	$values = $this->getArrayParameter( $name );
	
	$values1 = array();
	$values2 = array();
	
	for( $k=0; $k+1<count($values); $k+=2 ) {
		$combi = $values[$k]." ".$values[$k+1];
		
		$pos = mb_strpos( $combi, chr(194), 0, "utf-8" );
		if( is_bool($pos) ) { // not found
			$a = trim( $combi );
			$b = "";
		}
		else {
			$a = trim( mb_substr($combi,0,$pos,"utf-8") );
			$b = trim( mb_substr($combi,$pos,1000,"utf-8") );
		}
		
		$values1[] = $a;
		$values2[] = $b;
	}
	$this->setParameter( $name1, implode("\r\n",$values1) );
	$this->setParameter( $name2, implode("\r\n",$values2) );
  }
  
  //-----------
  
  private function showFillTable( $columns ) {
    $ncolumns = count($columns);
    
    $values = array();
    
    for( $k=0; $k<$ncolumns; $k++ ) {
      $values[$k] = $this->getArrayParameter( $columns[$k][0] );
    }
    
    $nrows = $this->arraysDataHeight( $values );
    
    echo "<table cellspacing=0 border=1>";
    echo "<tr>";
    for( $i=0; $i<$ncolumns; $i++ ) {
      echo "<td><center><b>" . $columns[$i][1] . "</b></center></td>";
    }
    echo "</tr>";
    for( $k=0; $k<$nrows; $k++ ) {
      echo "<tr>";
      for( $i=0; $i<$ncolumns; $i++ ) {
        echo "<td>" . $this->arrayItem($values[$i],$k) . "&nbsp;</td>";
      }
      echo "</tr>";
    }
    echo "</table>\n";
  }
  
  //===========
  
  private function endFill( $compProducer ) {

    $round = Motl::getParameter( "dbf_round" );
    
    if( ! strlen(trim($round) ) ) { // Протокол
      $this->endFill_proto( $compProducer );
    }
    else { // Round
      $this->endFill_round( $compProducer );
    }
                     
	$idEvent = Motl::getIntParameter( "idEvent" );
  	echo "<p>";
	echo "<A href='dbProcessEventCouples.php?idEvent=$idEvent&makeFill=yes&dbf_round=0'>финал</a><br>";
	echo "<A href='dbProcessEventCouples.php?idEvent=$idEvent&makeFill=yes&dbf_round=1'>1/2</a><br>";
	echo "<A href='dbProcessEventCouples.php?idEvent=$idEvent&makeFill=yes&dbf_round=2'>1/4</a><br>";
	echo "<A href='dbProcessEventCouples.php?idEvent=$idEvent&makeFill=yes&dbf_round=3'>1/8</a><br>";
}
  
  //===========
  
  private function endFill_proto( $compProducer ) {
	require_once "registrator.inc";
    //print_r( $_REQUEST );
    
    $idEvent = Motl::getIntParameter( "idEvent" );
    $round = Motl::getParameter( "dbf_round" );
    
    $anumber = $this->getArrayParameter( "number" );
    $aplace  = $this->getArrayParameter( "place" );
    $aman     = $this->getArrayParameter( "man" );
    $alady    = $this->getArrayParameter( "lady" );
    $acountry = $this->getArrayParameter( "country" );
    $acity    = $this->getArrayParameter( "city" );
    $aclub    = $this->getArrayParameter( "club" );
    $ateacher = $this->getArrayParameter( "teacher" );
    
    $nrows = $this->arraysDataHeight( array($anumber,$aplace,$aman,$alady,$acountry,$acity,$aclub,$ateacher) );
    
//    echo $nrows;die();
	$ceIdsToKeep = array();
	
    for( $k=0; $k<$nrows; $k++ ) {
      $number  = $this->arrayItem( $anumber, $k );
      $place   = $this->arrayItem( $aplace, $k );
      $man     = $this->arrayItem( $aman, $k );
      $lady    = $this->arrayItem( $alady, $k );
      $country = $this->arrayItem( $acountry, $k );
      $city    = $this->arrayItem( $acity, $k );
      $club    = $this->arrayItem( $aclub, $k );
      $teacher = $this->arrayItem( $ateacher, $k );

	//===========
	$ce = $this->openCoupleEventByNames( $idEvent, $man, $lady );
	//===========
      
      if( $number  ) { $ce->row->number = $number; }
      if( $place   ) { $ce->row->place = $place; }
      if( $country ) { $ce->row->raw_country = $country; }
      if( $city    ) { $ce->row->raw_city = $city; }
      if( $club    ) { $ce->row->raw_club = $club; }
      if( $teacher ) { $ce->row->raw_teacher = $teacher; }

      $ce->saveToDB();
	  $ceIdsToKeep[] = $ce->row->id;
    }
	
	$sidstoKeep = implode( ",", $ceIdsToKeep );
	
	$sql = "DELETE FROM db_couple_event "
		."WHERE idEvent=$idEvent AND id Not In($sidstoKeep)";
	MotlDB::query($sql);
	
	$pid = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id=$idEvent" );
	echo "<A href='db.php?id=e$pid,r$idEvent'>назад</a><br>";
  }
  
  //===========
  
	private function openCoupleEventByNames( $idEvent, $man, $lady ) {
		require_once "registrator.inc";
	
		list($msurname,$mname) = $this->parsePerson( $man );
		$idMan  = Registrator::storePerson( $msurname, $mname, "M" );
		
		list($fsurname,$fname) = $this->parsePerson( $lady );
		$idLady = Registrator::storePerson( $fsurname, $fname, "F" );
		
		$idCouple = Registrator::storeCouple( $idMan, $idLady );

		$ceRow = null;
		if( $idCouple ) {
			// ASS $idEvent
			// COD $idCouple
			$ceRow = MotlDB::queryRow( "SELECT * FROM db_couple_event "
										."WHERE idEvent=$idEvent AND idCouple=$idCouple" );  
		}

		if( $ceRow) {
			$ce = new DbCoupleEventProducer( $ceRow );
		}
		else {
			$ce = new DbCoupleEventProducer();
		}

		$ce->row->idEvent  = $idEvent;
		$ce->row->idCouple = $idCouple;
		
		return $ce;
	}
  
  //===========
  
  private function endFill_round( $compProducer ) {
    //print_r( $_REQUEST );
    
    $idEvent = Motl::getIntParameter( "idEvent" );
    $round = Motl::getParameter( "dbf_round" );
    
    $number = $this->getArrayParameter( "number" );
    $place  = $this->getArrayParameter( "place" );
    
    $ndances = Motl::getParameter( "ndances" );
    $dances  = Motl::getParameter( "dances" );
    $adances = explode( ",", $dances );
    $values = array();
    $aRealDs = array();
    $aRealDances = array();

    for( $k=0; $k<$ndances; $k++ ) {
      $d = $k+1;
      $value0 = Motl::getParameter( "d$d", false );
      $value = $this->getArrayParameter( "d$d", false );
      $values[$k] = $value;
      
      if( strlen(trim($value0)) ) {
        $aRealDs[] = $k;
        $aRealDances[] = $adances[$k];
      }
      //Motl::getParameter( "d$d" );
      //$columns[] = array( "d$d", $adances[$k] );
    }
    //print_r( $values );

    $realDances = implode( ",", $aRealDances );
    
    //-----
    $nrows = $this->arraysDataHeight( array($number,$place) );
    //echo "<p>MAKE ROUND: $round &lt;- $idEvent<br>";
    
    $roundNames = array( "final", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64" );

    $roundRow = array( "pid"=>$idEvent, "type"=>"round", "round"=>$round, "title"=>$roundNames[$round], 
                     "dances"=>$realDances, "ncouples"=>$nrows );

    //print_r($roundRow);
    $idNewEvent = -1;
    $idNewEvent = MotlDB::saveRow( "db_events", "id",null, $roundRow );

    $ncouples = 0;
    for( $k=0; $k<$nrows; $k++ ) {
      // ASS $idEvent
      $oldCeRow = MotlDB::queryRow( "SELECT * FROM db_couple_event WHERE idEvent='$idEvent' AND number='".MRES($number[$k])."'" );  
      if( ! $oldCeRow ) {
        continue;
      }
      
      $ncouples ++;

	  $kNumber = $this->arrayItem($number,$k);
	  $kPlace = $this->arrayItem($place,$k);
	  
      $ceRow = array( "idEvent"=>$idNewEvent,
                          "idCouple"=>$oldCeRow->idCouple, 
                          "number"=>$kNumber, 
                          "place"=>$kPlace );

      for( $i=0; $i<count($aRealDs); $i++ ) {
        $d = $i+1;
        $fn = ($d<10) ? "d$d" : "da";
        $ceRow[$fn] = $this->formatScores( $round, $this->arrayItem($values[$aRealDs[$i]],$k) );
      }
      MotlDB::saveRow( "db_couple_event", "id",null, $ceRow );
      //print_r($ceRow);
	  
	  if( $kPlace ) {
		$oldCeRow->place = $kPlace;
		MotlDB::saveRow( "db_couple_event", "id",$oldCeRow->id, $oldCeRow );
		//print_r($oldCeRow);
	  }
    }
    
    $roundRow = array( "pid"=>$idEvent, "type"=>"round", "round"=>$round, "title"=>$roundNames[$round], 
                     "dances"=>$realDances, "ncouples"=>$ncouples );
    MotlDB::saveRow( "db_events", "id",$idNewEvent, $roundRow );
	
	$pid = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id=$idEvent" );
	echo "<A href='db.php?id=e$pid,r$idEvent'>назад</a><br>";
  }
  
  //===========
}

//=====================

class TableTemplator extends DbProducerTemplator {
  
  public $groupCode;
  
  function processTag_plainClass( $tagParts ) {
    $value = $this->getFValue($tagParts);
	
	if( !is_bool( strpos($value,":") ) ) { // If contains  ':'
		$ar = explode( ":", $value );
//		$value = "$ar[0]|$ar[1]|$ar[2]";
		if( strlen($ar[0]) ) {
			$value = $ar[0];
		}
		else
		if( $ar[1] == $ar[2] ) {
			$value = $ar[1];
		}
		else {
			$b = array();
			if( strlen($ar[1]) ) $b[] = $this->formatProg("St",$ar[1]);
			if( strlen($ar[2]) ) $b[] = $this->formatProg("La",$ar[2]);
			$value = implode( ", ", $b );
		}
	}
	
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }
  
  private function formatProg( $name, $class ) {
	//return "$name - $class";
	return "$class($name)";
  }
}

//=====================
?>