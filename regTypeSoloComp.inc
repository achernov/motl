<?php
//=====================
require_once "regType.inc";
//=====================

class RegTypeHandlerSoloComp extends RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
	if( $operation == "regform" ) {
		return false; // Форма общая с solo, тот handler ее и строит
	}
    return $code=="solocomp";
  }
  
  //===========
  
  public function getCanonicalCode() {
    return "solocomp";
  }
  
  //===========
  
  public function getLabel() {
    return "кубок солистов";
  }
  
  //===========
  
  public function makeResultValue( $compRow, $ceRow ) {

    $nCouples = $compRow->ncouples;
    $place    = $ceRow->place;
    $num = $compRow->number ? ", &nbsp; №".$compRow->number : "";
    return "$place из $nCouples$num";
  }
  
  //===========
  
  public function makeForm( $idEvent ) {

    $comps = $this->getSuitableComps( $idEvent );

//    $tiphnd = "onblur='onBlur()' onkeyup='onKeyUp(this)' onkeydown='onKeyDown(event)'";
    $tiphnd = "nblur='onBlur()' onkeyup='onKeyUp(this)'";
//    $tiphnd = "";

    $s = "";

    $s .= "<script src='regForm.js'></script>\n";
    $s .= "<script src='ajax.js'></script>\n";

      $s .= "<form name=RegForm action='regFormReceiver.php' method='POST' onsubmit='return checkFormSolo(this)'>\n";

    $s .= "<input type='hidden' name='id' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='regType' value='".$this->getCanonicalCode()."'>\n";
    $s .= "<input type='hidden' name='submitted' value='1'>\n";

    $s .= "<table border=0 class='MainText'>";
    $s .= "<tr> <td class='regLabel'> Фамилия </td><td> <input $tiphnd class='regInput2' type='text' name='surname'> </td> "
         ."<td rowspan=11 valign=top>"
            ."<div id='found' style='width:350;padding:20;'></div>"
         ."</td>"
         ."</tr>";
    $s .= "<tr> <td class='regLabel'> Имя </td><td> <input $tiphnd class='regInput2' type='text' name='name'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Дата рождения <br><small>(дд.мм.гггг) </td><td> <input $tiphnd class='regInput2' type='text' name='birthdate'> </td> </tr>";
 
    $s .= "<tr> <td>&nbsp;</td> </tr>";

    $s .= "<tr> <td class='regLabel'> Клуб   </td> <td> <input $tiphnd class='regInput2' type='text' name='club'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Тренер </td> <td> <input $tiphnd class='regInput2' type='text' name='teacher'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Город  </td> <td> <input $tiphnd class='regInput2' type='text' name='city'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Страна </td> <td> <input $tiphnd class='regInput2' type='text' name='country'> </td> </tr>";

    $s .= "<tr> <td colspan=2><h2>Программы</h2></td> </tr>";

    $s .= "<tr> <td></td><td>";
    $s .= $this->makeChecks( $comps );
    $s .= "</td> </tr>";

    $s .= "<tr> <td> </td> <td> &nbsp;<br><input type='submit' value='Зарегистрировать' > </td> </tr>";

    $s .= "</table>";

    $s .= "</form>\n";


    $s .= "Со всеми вопросами и проблемами при пользовании онлайн регистрацией "
         ."обращайтесь к администратору сайта: <A href='mailto:admin@danceleague.ru'>admin@danceleague.ru</a><br>\n";

    return $s;
  }
  
  //===========
  
  public function processUserInput() {

    if( Motl::getParameter( "submitted", 0 ) ) {
      $script = $_SERVER['PHP_SELF'];
    
      $regIds = $this->registerSoloByRequest();
      $sids = implode( ",", $regIds );

      $type = $this->getCanonicalCode();
      Motl::redirect( "$script?regType=$type&regIds=$sids" );
    }
    else
    if( $sids = Motl::getParameter( "regIds" ) ) {
      
      return $this->reportRegistration( $sids );
      
    }
    
  }
  
  //===========
  
  public function registerSoloByRequest() {
	require_once "registrator.inc";

    $surname   = Motl::getParameter( "surname" );
    $name      = Motl::getParameter( "name" );
    $birthdate = Motl::getParameter( "birthdate" );
    
    $club =     Motl::getParameter( "club" );
    $teacher =  Motl::getParameter( "teacher" );
    $city =     Motl::getParameter( "city" );
    $country =  Motl::getParameter( "country" );

    $compIds = $this->getCheckIds();

    $idDancer  = Registrator::storePerson( $surname, $name );

    $regIds = array();

    for( $k=0; $k<count($compIds); $k++ ) {

      $idEvent = $compIds[$k];

      $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                  array( "idEvent"=>$idEvent, "idSolo"=>$idDancer,
                         "class"=>$class, "raw_club"=>$club, "raw_teacher"=>$teacher, 
                         "raw_city"=>$city, "raw_country"=>$country ), 
                  true );

      $regIds[] = $id;
    }

    DataUtil::updateEventBase();

    return $regIds;
  }
  
  //===========

  public function reportRegistration( $sRegIds ) {
  
    ASSERT_ints($sRegIds);
  
    $idDancer   = MotlDB::queryValue( "SELECT idSolo FROM db_couple_event WHERE id In($sRegIds)", "" );  // ASS $sRegIds

    $comps = MotlDB::queryRows( "SELECT e.* FROM db_couple_event as ce INNER JOIN db_events as e ON ce.idEvent=e.id WHERE ce.id In($sRegIds)" );  // ASS $sRegIds
    
    $idEvent = $comps[0]->pid;
    $eventRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id='$idEvent'", "" );  // FLD $idEvent

    $coupleRow = MotlDB::queryRow( "SELECT surname, name FROM db_person WHERE id=$idDancer" );  // FLD $idDancer

    $s = "";

    $s .= "<h1>Соревнования: $eventRow->title</h1>";

    $dep = new DbEventProducer($eventRow);

    $s .= $dep->makeSwitch( "db.php" );

    $s .= "<b>$coupleRow->surname $coupleRow->name</b> "
         ."зарегистрирован(а) по следующим программам:<br>";

    $s .= "<div style='margin-left:25;margin-top:5'>";
    for( $k=0; $k<count($comps); $k++ ) {
      $row = $comps[$k];

      $s .= "<A href='db.php?id=e$idEvent,c$row->id'>$row->title</a><br>";
    }
    $s .= "</div>";

    return $s;
  }
  
  //===========
  
  public function makeRegList( $compProducer ) {
    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";
    
    if( $editmode ) $s .= $this->makeEditFormStart();
    $s .= $this->makeCouplesTable( $compProducer, 'c', $editmode );
    if( $editmode ) $s .= $this->makeEditFormEnd( $compProducer );
    
    return $s;
}
  
  //===========
  
  public function makeResults( $compProducer ) {
  
    $editmode = Motl::getParameter( "editmode", 0 );
    
    $tableKind = null;
    
    switch( $compProducer->row->type ) {
      case 'competition': $tableKind='p'; break;
      case 'round':       $tableKind='s'; break;
    }
    
    $s = "";
    
    if( $editmode ) $s .= $this->makeEditFormStart();
    $s .= $this->makeCouplesTable( $compProducer, $tableKind, $editmode );
    if( $editmode ) $s .= $this->makeEditFormEnd( $compProducer );
    
    return $s;
  }
  
  //===========
  
  private function makeEditFormStart() {
    return "<form action='dbProcessEventCouples.php' method='POST'>\n";
  }
  
  //===========
  
  private function makeEditFormEnd( $compProducer ) {
  
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

    $s .= $compProducer->makeText( null, "[text|dancer|style='width:250']" );
    $idParam = Motl::getParameter("id");
    $thisId = $compProducer->row->id;    
    $s .= "<input type='hidden' name='idParam' value='$idParam'>\n";
    $s .= "<input type='hidden' name='idEvent' value='$thisId'>\n";
    $s .= "<input type='submit' name='addDancer' value='Добавить участника'>\n";

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
      $curl0 = "<A href='db.php?id=p[plain|idSolo]'>";
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

    $obj->ecolNumber = new Column( "number", "номер",               "[atext|number|style='width:40']", "plain|number]" );
    $obj->colNumber  = new Column( "number", "номер",               "[plain|number]" );

    $obj->colDancer  = new Column( "dancer", "участник",          $curl0."<nobr>[plain|surname] [plain|name]</nobr>".$curl1, "[plain|surname] [plain|name]" );
//    $obj->colDancer  = new Column( "dancer", "участник",          $curl0."<nobr>[plain|surname] [plain|name]</nobr>".$curl1, "[plain|surname] [plain|name]" );
    
    $obj->colMan     = new Column( "man", "партнер",           $curl0."<nobr>[plain|msurname] [plain|mname]</nobr>".$curl1, "[plain|msurname] [plain|mname]" );
    $obj->colLady    = new Column( "lady", "партнерша",       "<nobr>[plain|fsurname] [plain|fname]</nobr>" );
    
    $obj->colManBirth   = new Column( "mbirth", "дата р.",        "[plainDateAge|mbirth]" );
    $obj->colLadyBirth  = new Column( "fbirth", "дата р.",        "[plainDateAge|fbirth]" );

    $obj->ecolClass  = new Column( "class", "класс",        "[atext|class]" );
    $obj->colClass   = new Column( "class", "класс",        "[plain|class]" );

    $obj->ecolCountry = new Column( "where", "страна",       "[atext|raw_country]" );
    $obj->colCountry  = new Column( "where", "страна",       "[plain|raw_country]" );

    $obj->ecolCity    = new Column( "where", "город",        "[atext|raw_city]" );
    $obj->colCity     = new Column( "where", "город",        "[plain|raw_city]" );

    $obj->ecolClub    = new Column( "club", "клуб",                 "[atext|raw_club]" );
    $obj->colClub     = new Column( "club", "клуб",                 "[plain|raw_club]" );

    $obj->ecolTeacher = new Column( "teacher", "тренер",             "[atext|raw_teacher]" );
    $obj->colTeacher  = new Column( "teacher", "тренер",             "[plain|raw_teacher]" );
    
    return $obj;
  }
  
  //===========
  
  private function makeCouplesTable( $compProducer, $tableKind, $edit=false ) {
    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";

    $thisId = (int) $compProducer->row->id;

    $idComp = (int)(  ($compProducer->row->type=='competition') ? $compProducer->row->id : $compProducer->row->pid  );

    $idEvent = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$idComp'", 0 );  // COD $idComp

    //----------

      if( $tableKind=='p' || $tableKind=='s' ) {
      
        $rounds = MotlDB::queryRows( "SELECT * FROM db_events "
                                    ."WHERE pid='$idComp' and type='round' and (hidden is null or not hidden) ORDER BY round" );  // COD $idComp

        $roundLabels = array( "финал - оценки", "кресты 1/2", "кресты 1/4", "кресты 1/8", "кресты 1/16", "кресты 1/32", "кресты 1/64" );

        $tabs = new Tabs();
        $tabs->addTab( "$idComp", "места", "db.php?id=e$idEvent,r$idComp" );

        for( $k=0; $k<count($rounds); $k++ ) {
          $rLevel = $rounds[$k]->round;
          $rid    = $rounds[$k]->id;
          $label = $roundLabels[$rLevel];
          $label = $rounds[$k]->title;
          $tabs->addTab( "$rid", $label, "db.php?id=e$idEvent,r$rid" );
        }

        $tabs->current = $thisId;

        $s .= $tabs->produce();   
      }  

      $judges = $this->makeJudges( $compProducer );
      if( $judges ) {
        $s .= "Судьи: <i>". $judges . "</i><br>";
      }

      if( $edit ) {
        $s .= $compProducer->makeEditLink();
        $s .= "&nbsp;";
        $s .= $compProducer->makeKillLink();
      }

    //----------

    $rows = MotlDB::queryRows( "SELECT Event.*, Dancer.id as idDancer, surname, name "
                              ."FROM db_couple_event AS Event INNER JOIN db_person AS Dancer ON Event.idSolo=Dancer.id "
                              ."WHERE idEvent='$thisId'" );  // COD $thisId

    //----------

    $defaultSort = "";

    $crossesHere =  ( $tableKind=='s' && $compProducer->row->round );

    $defaultSort = $crossesHere ? "crosses" : "place"; 

    $sort = Motl::getParameter( "sort", $defaultSort );

    //----------

// id,idEvent,idCouple,number,country,city,club,teacher,mid,msurname,mname,fid,fsurname,fname
//    $cols = $this->makeColumns( 1 && User::isAdmin() );
    $cols = $this->makeColumns( 1 );

    // Couples show/edit
    $templates["c"][0] =  array(            
                            $cols->colNumber,  
                          $cols->colDancer,
//                            $cols->colMan, 
//                            $cols->colLady, 
                        $cols->colCountry,  
                        $cols->colCity,  
                            $cols->colClub, 
                            $cols->colTeacher 
                          );
    $templates["c"][1] =  array( 
                            $cols->ecolSel,
                            $cols->ecolEdit, 
                            $cols->ecolNumber, 
                          $cols->colDancer,
/*                            $cols->colMan, 
                      $cols->colManBirth,
                            $cols->colLady, 
                      $cols->colLadyBirth,*/
//                      $cols->ecolClass,  
                        $cols->colCountry,  
                        $cols->colCity,  
                            $cols->colClub, 
                            $cols->colTeacher 
                          );
    // Places show/edit
    $templates["p"][0] =  array(             
                            $cols->colPlace, 
                            $cols->colNumber, 
                          $cols->colDancer,
//                            $cols->colMan, 
//                            $cols->colLady, 
                        $cols->colCountry,  
                        $cols->colCity,  
                            $cols->colClub, 
                            $cols->colTeacher 
                          );
    $templates["p"][1] =  array( 
                            $cols->ecolSel,
                            $cols->ecolEdit, 
                            $cols->ecolPlace, 
                            $cols->ecolNumber, 
                          $cols->colDancer,
//                            $cols->colMan, 
//                            $cols->colLady, 
//                      $cols->colBirth,
//                      $cols->ecolClass,  
                        $cols->ecolCountry,  
                        $cols->ecolCity,  
                            $cols->ecolClub, 
                            $cols->ecolTeacher 
                          );
    // Scores show/edit
    $templates["s"][0] =  array(            
                            $cols->colPlace, 
                            $cols->colNumber, 
                          $cols->colDancer,
//                            $cols->colMan, 
//                            $cols->colLady 
                          );
    $templates["s"][1] =  array( 
                            $cols->ecolSel,
                            $cols->ecolEdit, 
                            $cols->ecolPlace, 
                          $cols->colDancer,
//                            $cols->colMan, 
//                            $cols->colLady, 
                            $cols->ecolNumber,
                          );

    //----- Load dances
    
    $dances = $compProducer->getDances();
    $nDances = count($dances);

    //----- Append dances

    $spreadScores = "spreadScores(this,[plain|#],$nDances)";
    $templates["s"][1][] = new Column( "spread", "A","[atext|spread|style='width:50' onclick='$spreadScores' onblur='$spreadScores']", Column::NoSort );

    for( $k=0; $k<$nDances; $k++ ) {
      $d = $k+1;
      $ds = "123456789a";
      $d = $ds[$k];
      $fname = "d$d";
      $dname = $dances[$k];
      $templates["s"][0][] = new Column( "d$d", $dname,"[plain|$fname|id='scores_[plain|id]_$k' style='font-family:\"Courier New\";']" );
      
      $spreadColumn = "spreadColumn(this,\"$d\")";
//      $dspread = "<input type='text' style='width:50' onclick='$spreadColumn' nblur='$spreadColumn'>";
      $dspread = "<textarea style='width:50' rows=1 onclick='$spreadColumn;return false' onblur='$spreadColumn'></textarea>";
      
      $templates["s"][1][] = new Column( "d$d", $dname.$dspread,"[atext|$fname|style='width:150' onclick='normalizeScores(this)' onblur='normalizeScores(this)']" );
    }

    if( $crossesHere ) {
      $templates["s"][0][] = new Column( "crosses", "кресты", "[plain|crosses]", "crossCmp" );
    }

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

    //----- Select template

    $template = $templates[$tableKind][$edit?1:0];

    //----- Do hightlight
    
    $hc = Motl::getParameter( "hc", null );
    $hrow = null;
    
    if( ! is_null($hc) ) {
      for( $k=0; $k<count($rows); $k++ ) {
        if( $rows[$k]->idSolo == $hc )
          $hrow = $rows[$k];
      }
    }
    
    //----- Make table

    $table= new Table( $template );
    if( $template ) {
      $script = $_SERVER['PHP_SELF'];
      $id = Motl::getParameter( "id" );

      $table->sortBy = $sort;

      $table->highlightRow = $hrow;

      $s .= $table->produce( "DbCoupleEventProducer", $rows, "$script?id=$id" );
	  $s .= "<div id='scoreTip' style='display:none;position:absolute;border:#B3B3B5 1px solid;background-color:#D5E5F4;'>ddd</div>";
    }

    //---------- Make JS gimmicks
	
	$regIds = array();
	foreach( $rows as $row ) {
		$regIds[] = $row->id;
	}
	$ajudges = $this->loadJudges( $compProducer );

	$s .= $this->makeScoreTipJS( $regIds, $ajudges, $nDances );
	
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
    //print_r( $_REQUEST );
	$idEvent = Motl::getParameter("idEvent");
    $title = MotlDB::queryValue( "SELECT title FROM db_events WHERE id=$idEvent" );
	echo "<b style='color:#0000c0'>$title</b><p>";

    if( isset($_REQUEST["save"]) ) {
      $this->saveListEdit( $compProducer );
    }
    else
    if( isset($_REQUEST["addDancer"]) ) {
      $this->finishAddDancer( $compProducer );
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
      $obj->saveToDB();
    }
    
    Motl::redirectToRetUrl();
  }
  
  //===========
  
  private function finishAddDancer( $compProducer ) {
	require_once "registrator.inc";
	
    $idComp = Motl::getParameter( "idEvent" );
    $dbf_dancer = Motl::getParameter( "dbf_dancer" );;
//    echo "<b>finishAddDancer</b><p>";

    $fn = explode( " ", trim($dbf_dancer) );
    
    $idDancer  = Registrator::storePerson( $fn[0], $fn[1] );
    echo "$idComp, $dbf_dancer, $idDancer";

    $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                array( "idEvent"=>$idComp, "idSolo"=>$idDancer ), 
                true );
                
    Motl::redirectToRetUrl();
  }
  
  //===========
  
  private function makeRound( $compProducer ) {
    //print_r( $_REQUEST );//die("makeRound...");

    $ids     = $_REQUEST['dbf_id'];
    
    $selids = array();

    for( $k=0; $k<count($ids); $k++ ) {
      if( isset($_REQUEST["dbf_".$k."_check"]) ) {
        $selids[] = $ids[$k];
      }
    }

    $nids = count($selids);
    $sids = implode( ",", $selids );
    ASSERT($sids);

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

    $idEvent = $eventIds[0];

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

    $doClearSiblings = 0;
    
    if( $doClearSiblings ) {
    // FLD $newRow[pid], 
    // ASS $newRow[round]
      $delIds = MotlDB::queryValues( "SELECT id FROM db_events WHERE pid=$newRow[pid] and round=$newRow[round]" );  
      
      $dids = implode( ",", $delIds );

      if( strlen($dids) ) {
        MotlDB::query( "DELETE FROM db_events WHERE id In($dids)" );  // FLD $dids
        MotlDB::query( "DELETE FROM db_couple_event WHERE idEvent In($dids)" );  // FLD $dids
      }
    }

    $idNewEvent = MotlDB::saveRow( "db_events", "id",null, $newRow );

    // COD,FLD $idNewEvent
    // ASS $sids
    MotlDB::query( "INSERT INTO db_couple_event(idEvent,idSolo,number) SELECT $idNewEvent,idSolo,number FROM db_couple_event WHERE id In($sids)" );

//  echo "SELECT idCouple FROM db_couple_event WHERE id In($sids)";
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
    $s .= "<table><tr>";
    if( ! strlen(trim($round) ) ) { // Протокол
		
		if( PROTOCOL_TYPE == PROTOCOL_TARUDKO ) {
			$s .= "<input type='hidden' name='protocolType' value='".PROTOCOL_TYPE."'>\n";
			$s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
			//$s .= $this->makeFillArea( $compProducer, "place",   "place",     50 );
			$s .= $this->makeFillArea( $compProducer, "dancer",     "Участник",   200 );
			//$s .= $this->makeFillArea( $compProducer, "country", "Страна",    100 );
			$s .= $this->makeFillArea( $compProducer, "city",    "Город",     150 );
			$s .= $this->makeFillArea( $compProducer, "club",    "Клуб",      300 );
			$s .= $this->makeFillArea( $compProducer, "teacher", "Тренеры",   300 );
		}
		else
		if( PROTOCOL_TYPE == PROTOCOL_ALEKSEEV ) {
			$s .= "<input type='hidden' name='protocolType' value='".PROTOCOL_TYPE."'>\n";
			$s .= $this->makeFillArea( $compProducer, "place",   "очки",     50 );
			$s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
			$s .= $this->makeFillArea( $compProducer, "man_lady","Участник",   200 );
			//$s .= $this->makeFillArea( $compProducer, "birthdates", "Дата рожд.",    100 );
			//$s .= $this->makeFillArea( $compProducer, "class",    "Класс",    100 );
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
                  array( "dancer",  "Участник" ),
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
                
	if( Motl::getParameter("protocolType") == PROTOCOL_ALEKSEEV ) {
		// НЕ БЫВАЕТ!!!
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
//print_r( $_REQUEST );die();
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
		$adancer  = $this->getArrayParameter( "dancer" );
		$acountry = $this->getArrayParameter( "country" );
		$acity    = $this->getArrayParameter( "city" );
		$aclub    = $this->getArrayParameter( "club" );
		$ateacher = $this->getArrayParameter( "teacher" );
		
		$nrows = $this->arraysDataHeight( array($anumber,$aplace,$adancer,$acountry,$acity,$aclub,$ateacher) );
		
		$ceIdsToKeep = array();

		for( $k=0; $k<$nrows; $k++ ) {
		  $number  = $this->arrayItem( $anumber, $k );
		  $place   = $this->arrayItem( $aplace, $k );
		  $dancer  = $this->arrayItem( $adancer, $k );
		  $country = $this->arrayItem( $acountry, $k );
		  $city    = $this->arrayItem( $acity, $k );
		  $club    = $this->arrayItem( $aclub, $k );
		  $teacher = $this->arrayItem( $ateacher, $k );
		  
		  list($surname,$name) = $this->parsePerson( $dancer );
		  $idSolo = Registrator::storePerson( $surname, $name );
		  
		  $ceRow = null;

		  if( $idSolo ) {
			// ASS $idEvent
			// COD $idSolo
			$ceRow = MotlDB::queryRow( "SELECT * FROM db_couple_event WHERE idEvent=$idEvent AND idSolo=$idSolo" );  
//print_r($ceRow);
//die("idSolo=$idSolo");
		  }

		  if( $ceRow) {
			$ce = new DbCoupleEventProducer( $ceRow );
		  }
		  else {
			$ce = new DbCoupleEventProducer();
		  }

		  $ce->row->idEvent  = $idEvent;
		  $ce->row->idSolo   = $idSolo;
		  
		  if( $number  ) { $ce->row->number = $number; }
		  if( $place   ) { $ce->row->place = $place; }
		  if( $country ) { $ce->row->raw_country = $country; }
		  if( $city    ) { $ce->row->raw_city = $city; }
		  if( $club    ) { $ce->row->raw_club = $club; }
		  if( $teacher ) { $ce->row->raw_teacher = $teacher; }
//print_r($ce);
		  $ce->saveToDB();
		  $ceIdsToKeep[] = $ce->row->id;
		}

		$sidstoKeep = implode( ",", $ceIdsToKeep );
//echo $sidstoKeep;
		$sql = "DELETE FROM db_couple_event "
			."WHERE idEvent=$idEvent AND id Not In($sidstoKeep)";
		//die($sql);
		MotlDB::query($sql);

		$pid = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id=$idEvent" );
		echo "<A href='db.php?id=e$pid,r$idEvent'>назад</a><br>";
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

    $roundRow = array( "pid"=>$idEvent, 
					"type"=>"round", 
					"regType"=>"solocomp", 
					"round"=>$round, 
					"title"=>$roundNames[$round], 
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
                          "idSolo"=>$oldCeRow->idSolo, 
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
	echo "<A href='db.php?id=e$pid,r$idEvent'>назад</a><p>";
}
  
  //===========
}

//=====================

?>
