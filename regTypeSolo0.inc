<?php
//=====================
require_once "regType.inc";
//=====================

class RegTypeHandlerSolo extends RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
    return ($code=="solo") 
		|| ($operation=="regform"&&$code=="solocomp"); // Форма общая с solocomp
  }
  
  //===========
  
  public function getCanonicalCode() {
    return "solo";
  }
  
  //===========
  
  public function getLabel() {
    return "солисты";
  }
  
  //===========
  
  public function makeResultValue( $compRow, $ceRow ) {

    $score  = $this->calcScore( $ceRow );
    //$score  = $ceRow->place;
    
    $diplomDegree = $this->calcDiplom( $score );
    
    $diplomText = $this->diplomText( $diplomDegree );
    
    return "$score баллов, диплом $diplomText";
  }
  
  //===========
  
  public function makeForm( $idEvent ) {
  
	if( ! $this->isRegOpen($idEvent) ) {
		return "Сейчас нет онлайн-регистрации на этот турнир";
	}

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

    $s .= "<tr> <td class='regLabel'> Клуб   </td>     <td> <input $tiphnd class='regInput2' type='text' name='club'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Руководитель</td><td> <input $tiphnd class='regInput2' type='text' name='boss'> </td> </tr>";
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

    $surname   = Motl::getParameter( "surname" );
    $name      = Motl::getParameter( "name" );
    $birthdate = Motl::getParameter( "birthdate" );
    
    $club =     Motl::getParameter( "club" );
    $boss =     Motl::getParameter( "boss" );
    $teacher =  Motl::getParameter( "teacher" );
    $city =     Motl::getParameter( "city" );
    $country =  Motl::getParameter( "country" );

    $compIds = $this->getCheckIds();

    $idDancer  = $this->storePerson( $surname, $name );

    $regIds = array();

    for( $k=0; $k<count($compIds); $k++ ) {

      $idEvent = $compIds[$k];

      $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                  array( "idEvent"=>$idEvent, "idSolo"=>$idDancer,
						 "mbirthdate"=>$mbirthdate,
                         "class"=>$class, "raw_club"=>$club, "raw_boss"=>$boss, "raw_teacher"=>$teacher, 
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

    $obj->ecolPlace  = new Column( "place", "баллы(place)",               "[atext|place|style='width:40']" );
    $obj->colPlace   = new Column( "place", "баллы",               "<center>[plain|place]</center>" );

    $obj->ecolNumber = new Column( "number", "номер",               "[atext|number|style='width:40']","[plain|number]" );
    $obj->colNumber  = new Column( "number", "номер",               "[plain|number]" );

    $obj->colDancer  = new Column( "dancer", "участник",          $curl0."<nobr>[plain|surname] [plain|name]</nobr>".$curl1, "[plain|surname] [plain|name]" );

    $obj->ecolCountry = new Column( "where", "страна",       "[atext|raw_country]" );
    $obj->colCountry  = new Column( "where", "страна",       "[plain|raw_country]" );

    $obj->ecolCity    = new Column( "where", "город",        "[atext|raw_city]" );
    $obj->colCity     = new Column( "where", "город",        "[plain|raw_city]" );

    $obj->ecolClub    = new Column( "club", "клуб",                 "[atext|raw_club]" );
    $obj->colClub     = new Column( "club", "клуб",                 "[plain|raw_club]" );

    $obj->colBoss     = new Column( "boss", "руководитель",                 "<nobr>[plain|raw_boss]</nobr>" );
	
    $obj->ecolTeacher = new Column( "teacher", "тренер",             "[atext|raw_teacher]" );
    $obj->colTeacher  = new Column( "teacher", "тренер",             "[plain|raw_teacher]" );

    $obj->colAvg     = new Column( "avg", "баллы",               "<center>[plain|avg]</center>" );
    $obj->colDiplom  = new Column( "diplom", "диплом",               "<center>[plain|diplom]</center>" );

    return $obj;
  }
  
  //===========
  
  public function makeRegList( $compProducer ) {

    $editmode = Motl::getParameter( "editmode", 0 );

    $rows = $this->loadDancersTable( $compProducer );

    return $this->makeDancersTable( $compProducer, $rows, MODE_REGLIST, $editmode );
  }
  
  //===========
  
  public function makeResults( $compProducer ) {

    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";

    //----- Prepare templates

    $cols = $this->makeColumns( 1 && User::isAdmin() );
    
    $templateInfo = array(
                      array(                     
                        $cols->colNumber,  
                        $cols->colDancer, 
                        $cols->colDiplom, 
//                        $cols->colAvg, 
                        //$cols->colCountry,  
                        $cols->colCity,  
                        $cols->colClub,  
                        $cols->colBoss, 
                        $cols->colTeacher  
                      ),
                      array( 
                        $cols->ecolSel,
                        $cols->ecolEdit, 
                        $cols->ecolNumber, 
                        $cols->colDancer,                      
                        //$cols->ecolCountry, 
                        $cols->ecolCity, 
                        $cols->ecolClub, 
                        $cols->ecolTeacher 
                      )
                    );

    $templateScores = array(
                        array(                     
                          $cols->colNumber,  
                          $cols->colDancer, 
                          $cols->colDiplom, 
//                          $cols->colPlace,
                          $cols->colAvg,
                        ),
                        array( 
                          $cols->ecolSel,
                          $cols->ecolEdit, 
                          $cols->ecolNumber, 
                          $cols->colDancer, 
                          $cols->colDiplom, 
                          $cols->colAvg,
                          $cols->ecolPlace
                        )
                     );
                     
    //----- Setup dances

    $dances = $compProducer->getDances();
    $nDances = count($dances);
    
    //----- Append dances

    $templateScores[1][] = new Column( "spread", "A","[atext|spread|style='width:50' onclick='spreadScores(this,[plain|#],$nDances)' onblur='spreadScores(this,[plain|#],$nDances)']", Column::NoSort );

    for( $k=0; $k<$nDances; $k++ ) {
      $ds = "123456789a";
      $d = $ds[$k];
      $fname = "d$d";
      $dname = $dances[$k];
      
      $spreadColumn = "spreadColumn(this,\"$d\")";
      $dspread = "<textarea style='width:50' rows=1 onclick='$spreadColumn' onblur='$spreadColumn'></textarea>";
      
      $templateScores[0][] = new Column( "d".($k+1), $dname,"[plain|$fname|id='scores_[plain|id]_$k' style='font-family:\"Courier New\";']" );
      $templateScores[1][] = new Column( "d".($k+1), $dname.$dspread,"[atext|$fname|style='width:150' onclick='normalizeScores(this)' onblur='normalizeScores(this)']" );
    }
                    
    //----- Choose View Mode
    
    $aid = Motl::splitId( Motl::getParameter("id") );
    $viewMode = isset($aid[2]) ? $aid[2] : "";
    
    if( $viewMode == "scores" ) {
      $template = $templateScores;
	  $mode = MODE_RESULT_SCORES;
    }
    else {
      $template = $templateInfo;
	  $mode = MODE_RESULT_COMMON;
    }
    
    //----- Make tabs
    
    $idComp = (int)( ($compProducer->row->type=='competition') ? $compProducer->row->id : $compProducer->row->pid );

    $idEvent = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$idComp'", 0 );  // COD $idComp
    
    $tabs = new Tabs();
    $tabs->addTab( "",       "протокол", "db.php?id=e$idEvent,r$idComp" );
    $tabs->addTab( "scores", "оценки",   "db.php?id=e$idEvent,r$idComp,scores" );
    $tabs->current = $viewMode;
    $s .= $tabs->produce();
    
    //----- Make Judges
    
    $judges = $this->makeJudges( $compProducer );
    if( $judges ) {
      $s .= "Судьи: <i>". $judges . "</i><br>";
    }
    
    //----- Make Table
    
    if( $editmode ) {
      $s .= $this->makeEditFormStart();
    }

    $rows = $this->loadDancersTable( $compProducer );
    
    $s .= $this->makeDancersTable( $compProducer, $rows, $mode, $editmode );

    if( $editmode ) {
      $nRows = count($rows);

      $s .= "<input type='button' value='Calc Avg' onclick='calcSoloAvg($nRows,$nDances)'>\n";
      
      $s .= $this->makeEditFormEnd( $compProducer );
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
  
  private function makeEditFormEnd( $compProducer ) {
  
    $s = "";
    
    $s .= "<input type='submit' name='save' value='Save' style='margin-right:100'>\n";

    
    $returl = Motl::makeRetUrlPost();
    $idComp = $compProducer->row->id;
    $s .= "<input type='hidden' name='returl' value='$returl'>\n";
    $s .= "<input type='hidden' name='idEvent' value='$idComp'>\n";

    $s .= "&nbsp; &nbsp; &nbsp; &nbsp; \n";

    $s .= "<input type='submit' name='makeFill' value='Fill'>";
	
    $s .= "&nbsp; &nbsp; &nbsp; &nbsp; \n";

    $s .= "<input type='submit' name='deleteSelected' value='Delete selected' onclick='return confirmKills()'>\n";

    $s .= "<p>";
    $s .= $compProducer->makeText( null, "[text|dancer|style='width:250']" );
    $s .= "<input type='submit' name='addDancer' value='Добавить участника'>\n";

    $s .= "</form>\n";
    
    return $s;
  }
  
  //===========
  
  private function calcScore( $row ) {
  
    $n = 0;
    $sum = 0;
    
    for( $d=1; $d<=6; $d++ ) {
      $fn = "d$d";
      $scores = $row->$fn;
      if( $scores ) {
        $sum += $this->strAvg( $scores );
        $n ++;
      }
    }
    
    $avg = $n ? round( $sum/$n, 1 ) : null;
    
    return $row->place ? $row->place : $avg;
  }
  
  //===========
  
  public function calcDiplom( $score ) {
    if( $score >= 4.5 ) {
      return 1;
    }
    else
    if( $score >= 3.5 ) {
      return 2;
    }
    else 
    if( $score ) {
      return 3;
    }
    return null;
  }
  
  //===========
  
  private function diplomText( $degree ) {
  
    switch( $degree ) {
      case 1: return "1 ст.";
        
      case 2: return "2 ст.";
        
      case 3: return "3 ст.";
        
      default: return  "&nbsp;";
    }
    
  }
  
  //===========
  
  private function loadDancersTable( $compProducer ) {
  
    $thisId = (int) $compProducer->row->id;
    
    $rows = MotlDB::queryRows( "SELECT Event.*, Dancer.id as idDancer, surname, name "
                              ."FROM db_couple_event AS Event INNER JOIN db_person AS Dancer ON Event.idSolo=Dancer.id "
                              ."WHERE idEvent='$thisId'" );  // COD $thisId
                              
    return $rows;
  }
  
  //===========
  
  private function makeTemplate( $cols, $mode, $edit ) {
	
	switch( $mode ) {
		case MODE_REGLIST."0":
			return $edit ?
                      array( 
                        $cols->ecolSel,
                        $cols->ecolEdit, 
                        $cols->ecolNumber, 
                        $cols->colDancer, 
                        //$cols->colCountry,  
                        $cols->colCity,  
                        $cols->colClub, 
                        $cols->colTeacher 
                      )
					  :
                      array(                     
                        $cols->colNumber,  
                        $cols->colDancer, 
                        //$cols->colCountry,  
                        $cols->colCity,  
                        $cols->colClub, 
                        $cols->colBoss, 
                        $cols->colTeacher 
                      );
					  
		case MODE_RESULT_COMMON:
			return $edit ?
                      array( 
                        $cols->ecolSel,
                        $cols->ecolEdit, 
                        $cols->ecolNumber, 
                        $cols->colDancer,                      
                        //$cols->ecolCountry, 
                        $cols->ecolCity, 
                        $cols->ecolClub, 
                        $cols->ecolTeacher 
                      )
					  :
                      array(                     
                        $cols->colNumber,  
                        $cols->colDancer, 
                        $cols->colDiplom, 
//                        $cols->colAvg, 
                        //$cols->colCountry,  
                        $cols->colCity,  
                        $cols->colClub,  
                        $cols->colBoss, 
                        $cols->colTeacher  
                      );
					  
		case MODE_RESULT_SCORES:
			return $edit ?
                      array( 
                          $cols->ecolSel,
                          $cols->ecolEdit, 
                          $cols->ecolNumber, 
                          $cols->colDancer, 
                          $cols->colDiplom, 
                          $cols->colAvg,
                          $cols->ecolPlace
                      )
					  :
                      array(                     
                          $cols->colNumber,  
                          $cols->colDancer, 
                          $cols->colDiplom, 
//                          $cols->colPlace,
                          $cols->colAvg,
                      );
	}
	
	die( "ERROR in makeTemplate()" );
  }
  
  //===========
  
  private function makeDancersTable( $compProducer, $rows, $mode, $edit=false ) {
    $s = "";

    //----------

	if( ! $edit ) {
		$this->substCodedCCCT( $rows );
	}
	
    //----------

    if( $edit ) {
      $s .= $compProducer->makeEditLink();
      $s .= "&nbsp;";
      $s .= $compProducer->makeKillLink();
    }

    //----------

    $sort = Motl::getParameter( "sort", "surname" );

    //----- Make template

    $cols = $this->makeColumns( 1 );
    $template = $this->makeTemplate( $cols, $mode, $edit );

    //----- Calc avg score
    
    for( $k=0; $k<count($rows); $k++ ) {
      $row = $rows[$k];

      $avg = $this->calcScore( $row );
      $row->avg = $avg ? sprintf("%.1f",$avg) : "&nbsp;";
      $diplomDegree = $this->calcDiplom($avg);
      
      //$diplomDegree = $this->calcDiplom($row->place);
      
      $row->diplom = $this->diplomText( $diplomDegree );
     
    }
    
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

    //----------
    return $s;
  }
  
  //===========
  
  private function strAvg( $s ) {
    $sum = 0;
    for( $k=0; $k<strlen($s); $k++ ) {
      $sum += $s[$k];
    }
    return $sum/strlen($s);
  }
  
  //===========
  
  public function processListEdit( $compProducer ) {
	//echo"<pre>"; print_r( $_REQUEST ); die("qqq");
	$idEvent = Motl::getParameter("idEvent");
    $title = MotlDB::queryValue( "SELECT title FROM db_events WHERE id=$idEvent" );
	echo "<b style='color:#0000c0'>$title</b><p>";
	
    if( isset($_REQUEST["save"]) ) {
      $this->saveListEdit( $compProducer );
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
    }  
    else
    if( isset($_REQUEST["addDancer"]) ) {
      $this->finishAddDancer( $compProducer );
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
    
    Motl::redirectToRetUrl();
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
    if( ! isset($_REQUEST["dbf_spread"]) ) { // Протокол
		
		if( PROTOCOL_TYPE == PROTOCOL_TARUDKO ) {
			$s .= "<input type='hidden' name='protocolType' value='".PROTOCOL_TYPE."'>\n";
			$s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
			//$s .= $this->makeFillArea( $compProducer, "place",   "place",     50 );
			$s .= $this->makeFillArea( $compProducer, "dancer",     "Участник",   200 );
			$s .= $this->makeFillArea( $compProducer, "country", "Страна",    100 );
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
      $ndances = count($adances);
      $s .= $this->makeFillArea( $compProducer, "number",  "№",         50 );
      $s .= $this->makeFillArea( $compProducer, "place",   "очки",     50 );
      
	  $s .= $this->makeFillArea( $compProducer, "dx", "Все танцы",   100,100 );
	  
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
    //echo "<pre>"; print_r( $_REQUEST ); die();
    
    $idEvent = Motl::getParameter( "idEvent" );
    
    $round = Motl::getParameter( "dbf_round" );
    
    if( ! isset($_REQUEST["d1"]) ) { // Протокол
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
	
	if( Motl::getParameter("dx") ) {
		$this->expandDances(Motl::getParameter("ndances"));
	}
                
	if( Motl::getParameter("protocolType") == PROTOCOL_ALEKSEEV ) {
		$this->splitParameter( "number",  "number",  null );
		$this->splitParameter( "place",   "place",   null );
		$this->splitParameter( "teacher", "teacher", null );
		$this->splitParameter( "man_lady", "dancer", "dummy" );
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
		if( strlen(trim($values[$k+1])) ) {
			$values1[] = $values[$k];
			$values2[] = $values[$k+1];
		}
		else {
			$combi = $values[$k]." ".$values[$k+1];
//			echo "&lt;&lt;".$values[$k]."==".$values[$k+1]."&gt;&gt;<hr>";
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
//print_r( $_REQUEST );die();
    $round = Motl::getParameter( "dbf_round" );
    
    if( ! isset($_REQUEST["d1"]) ) { // Протокол
		$this->endFill_proto( $compProducer );
    }
    else { // Round
		$this->endFill_round( $compProducer );
    }
                     
  }
  
  //===========
  
	private function endFill_proto( $compProducer ) {
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
		  $idSolo = $this->storePerson( $surname, $name );
		  
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
		echo "<A href='db.php?id=e$pid,r$idEvent'>назад</a><p>";

		echo "<A href='dbProcessEventCouples.php?idEvent=$idEvent&makeFill=yes&dbf_spread=dummy'>оценки</a><br>";
	}
  
  //===========
  
	private function endFill_round( $compProducer ) {
//		print_r( $_REQUEST );
		echo "<pre>";
		
		$idEvent = Motl::getIntParameter( "idEvent" );

		$number = $this->getArrayParameter( "number" );
		$place  = $this->getArrayParameter( "place" );

		$ndances = Motl::getParameter( "ndances" );
		$dances  = Motl::getParameter( "dances" );
		$adances = explode( ",", $dances );
		
		//print_r( $adances );
		$nrows = $this->arraysDataHeight( array($number,$place) );

		$values = array();
		for( $k=0; $k<$ndances; $k++ ) {
			$d = $k+1;
			//$value0 = Motl::getParameter( "d$d", false );
			$value = $this->getArrayParameter( "d$d", false );
			$values[$k] = $value;
		}
		
		$ncouples = 0;
		for( $k=0; $k<$nrows; $k++ ) {
		  // ASS $idEvent
		  $idCe = MotlDB::queryValue( "SELECT id FROM db_couple_event WHERE idEvent='$idEvent' AND number='".MRES($number[$k])."'" );  
		  if( ! $idCe ) {
			continue;
		  }
		  
		  $ncouples ++;

		  $coupleRow = array();

		  $coupleRow['place'] = $this->arrayItem($place,$k);
		  
		  for( $i=0; $i<$ndances; $i++ ) {
			$d = $i+1;
			$fn = ($d<10) ? "d$d" : "da";
			$coupleRow[$fn] = $this->formatScores( 0, $this->arrayItem($values[$i],$k) );
		  }
		  MotlDB::saveRow( "db_couple_event", "id",$idCe, $coupleRow );
		  //print_r($coupleRow);
		}
		
		$pid = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id=$idEvent" );
		echo "<A href='db.php?id=e$pid,r$idEvent,scores'>назад</a><br>";
	}
	
  //===========
  
  private function finishAddDancer( $compProducer ) {
    $idComp = Motl::getParameter( "idEvent" );
    $dbf_dancer = Motl::getParameter( "dbf_dancer" );;
//    echo "<b>finishAddDancer</b><p>";

    $fn = explode( " ", trim($dbf_dancer) );
    
    $idDancer  = $this->storePerson( $fn[0], $fn[1] );
    echo "$idComp, $dbf_dancer, $idDancer";

    $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                array( "idEvent"=>$idComp, "idSolo"=>$idDancer ), 
                true );
                
    Motl::redirectToRetUrl();
  }
  
  //===========
  
}

//=====================
?>