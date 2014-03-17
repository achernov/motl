<?php
//=====================
require_once "regType.inc";
//=====================

class RegTypeHandlerTeam extends RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
    return $code=="team";
  }
  
  //===========
  
  public function getCanonicalCode() {
    return "team";
  }
  
  //===========
  
  public function getLabel() {
    return "команды";
  }
  
  //===========
  
  public function makeResultValue( $compRow, $ceRow ) {
    // TO DO
  }
  
  //===========
  
  public function makeForm( $idEvent ) {

    $comps = $this->getSuitableComps( $idEvent );
    $thisComps = $this->getSuitableComps( $idEvent, 1 );
    if( count($thisComps)!=1 ) {
      return "ERROR";
    }
    $thisComp = $thisComps[0];
//print_r($thisComp->id);die();
//    $tiphnd = "onblur='onBlur()' onkeyup='onKeyUp(this)' onkeydown='onKeyDown(event)'";
    $tiphnd = "nblur='onBlur()' onkeyup='onKeyUp(this)'";
//    $tiphnd = "";

    $s = "";

    $s .= "<script src='regForm.js'></script>\n";
    $s .= "<script src='ajax.js'></script>\n";

      $s .= "<form name=RegForm action='regFormReceiver.php' method='POST' onsubmit='return checkFormSolo(this)'>\n";

    $s .= "<table border=0 class='MainText'>";

    $s .= "<input type='hidden' name='id' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='regType' value='".$this->getCanonicalCode()."'>\n";
    $s .= "<input type='hidden' name='submitted' value='1'>\n";
    $s .= "<input type='hidden' name='idComp' value='$thisComp->id'>\n";
              
    $s .= "<tr> <td class='regLabel'> Название </td>"
              ."<td colspan=5> <input class='regInput1' type='text' style='width:600' name='teamName'> </td> </tr>";

    $s .= "<tr> <td> </td><td> <small>Фамилия</small> </td><td> <small>Имя</small> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Капитан </td><td> <input $tiphnd class='regInput1' type='text' name='capSurname'> </td><td> <input class='regInput1' type='text' name='capName'> </td> </tr>";

    //----- Make couples
    
    $format = $thisComp->teamFormat;
    $formatProgs = explode( ";", $format );
    $coupleK = 1;
    $ncouples = 0;
    foreach( $formatProgs as $prog ) {
      list($progName,$progN) = explode( ":", $prog );
      $s .= "<tr> <td colspan=2><h2>$progName</h2></td> </tr>";
      $s .= "<tr> <td></td>"
                ."<td> <small>Фамилия партнера</small> </td><td> <small>Имя партнера</small> </td>"
                ."<td>&nbsp;</td>"
                ."<td> <small>Фамилия партнерши</small> </td><td> <small>Имя партнерши</small> </td> </tr>";
      for( $k=0; $k<$progN; $k++ ) {
        $s .= "<tr> <td>$coupleK</td>"
                  ."<td> <input class='regInput1' type='text' name='msurname$coupleK'> </td>"
                  ."<td> <input class='regInput1' type='text' name='mname$coupleK'> </td>"
                  ."<td>&nbsp;</td>"
                  ."<td> <input class='regInput1' type='text' name='fsurname$coupleK'> </td>"
                  ."<td> <input class='regInput1' type='text' name='fname$coupleK'> </td>"
                  ."</tr>";
        $ncouples ++;
        $coupleK ++;
      }
    }
    
    $s .= "<input type='hidden' name='ncouples' value='$ncouples'>\n";
    //die();
    
    //----- 
    
    /*$s .= "<tr> <td colspan=2><h2>Программы</h2></td> </tr>";

    $s .= "<tr> <td></td><td>";
    $s .= $this->makeChecks( $comps );
    $s .= "</td> </tr>";*/

    $s .= "<tr> <td> </td> <td> &nbsp;<br><input type='submit' value='Зарегистрировать' > </td> </tr>";

    $s .= "</table>";

    $s .= "</form>\n";


    $s .= "Со всеми вопросами и проблемами при пользовании онлайн регистрацией "
         ."обращайтесь к администратору сайта: <A href='mailto:admin@danceleague.ru'>admin@danceleague.ru</a><br>\n";

    return $s;
  }
  
  //===========
  
  public function processUserInput() {
//die("haha");
    if( Motl::getParameter( "submitted", 0 ) ) {
      $script = $_SERVER['PHP_SELF'];
    
      $regIds = $this->registerTeamByRequest();
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
  
  public function registerTeamByRequest() {

  //print_r($_REQUEST);
    $teamName  = Motl::getParameter( "teamName" );
    
    $capSurname   = Motl::getParameter( "capSurname" );
    $capName      = Motl::getParameter( "capName" );
    
    $ncouples     = Motl::getParameter( "ncouples" );

    $idComp       = Motl::getParameter( "idComp" );
    
    $couples = array();
    
    $idCap  = $this->storePerson( $capSurname, $capName );
    
    for( $k=0; $k<$ncouples; $k++ ) {
      $k1 = $k+1;
      $msurname   = Motl::getParameter( "msurname$k1" );
      $mname      = Motl::getParameter( "mname$k1" );
      
      $fsurname   = Motl::getParameter( "fsurname$k1" );
      $fname      = Motl::getParameter( "fname$k1" );
      
      $idMan  = $this->storePerson( $msurname, $mname, "M" );
      $idLady = $this->storePerson( $fsurname, $fname, "F" );
      $idCouple = $this->storeCouple( $idMan, $idLady );
      $couples[] = $idCouple;
    }
    $scouples = implode( ",", $couples );

    $teamFields = array();
    $teamFields["name"] = $teamName;
    $teamFields["idCap"] = $idCap;
    $teamFields["couples"] = $scouples;
    $idTeam = MotlDB::saveRow( "db_teams", "id",null, $teamFields, true );

    $ceFields = array();
    $ceFields["idEvent"] = $idComp;
    $ceFields["idTeam"]  = $idTeam;
    $idReg = MotlDB::saveRow( "db_couple_event", "id",null, $ceFields, true );

//    echo "<p>idNew=$idNew<p>";
//    print_r($teamFields);


    return array($idReg);
  }
  
  //===========

  public function reportRegistration( $sRegIds ) {
  
    ASSERT_ints($sRegIds);
    
    $idTeam   = MotlDB::queryValue( "SELECT idTeam FROM db_couple_event WHERE id In($sRegIds)", "" );  // ASS $sRegIds

    $comps = MotlDB::queryRows( "SELECT e.* FROM db_couple_event as ce INNER JOIN db_events as e ON ce.idEvent=e.id WHERE ce.id In($sRegIds)" );  // ASS $sRegIds
    
    $idEvent = $comps[0]->pid;
    $eventRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id='$idEvent'", "" );  // FLD $idEvent

    $teamRow = MotlDB::queryRow( "SELECT * FROM db_teams WHERE id=$idTeam" );  // FLD $idTeam
    
    $cap = $this->loadPersonText( $teamRow->idCap );

    $s = "";

    $s .= "<h1>Соревнования: $eventRow->title</h1>";

    $dep = new DbEventProducer($eventRow);

    $s .= $dep->makeSwitch( "db.php" );

    $s .= "Команда <b>\"$teamRow->name</b>\"<p>";
    $s .= "Капитан $cap<p>";

    $idcouples = explode( ",", $teamRow->couples );
    $s .= "Пары:<br>";
    for( $k=0; $k<count($idcouples); $k++ ) {
      $k1 = $k+1;
      $cpl = $this->loadCoupleText( $idcouples[$k] );
      $s .= "$k1. $cpl<br>";
    }

    $s .= "<p>зарегистрирована";

    return $s;
  }
  
  //===========
  
  private function loadPersonText( $idPerson ) {
    ASSERT_int($idPerson);
    return MotlDB::queryValue( "SELECT CONCAT(surname,' ',name) FROM db_person WHERE id=$idPerson" );  // ASS $idPerson
  }
  
  //-----------
  
  private function loadCoupleText( $idCouple, $personDelim=" ", $coupleDelim=" - " ) {
  
    ASSERT_int($idCouple);
    
    // ASS $idCouple
    return MotlDB::queryValue( "SELECT CONCAT(m.surname,'".MRES($personDelim)."',m.name,'".MRES($coupleDelim)."',f.surname,'".MRES($personDelim)."',f.name) "
                              ."FROM db_couples as c INNER JOIN db_person as m ON c.idMan=m.id "
                              ."INNER JOIN db_person as f ON c.idLady=f.id WHERE c.id=$idCouple" );  
  }
  
  //===========
  
  private function makeColumns( $enableBaseLink = true ) {
    $obj = new stdClass();

    /*if( $enableBaseLink ) {
      $curl0 = "<A href='db.php?id=p[plain|idSolo]'>";
      $curl1 = "</a>";
  	}
  	else {*/
      $curl0 = "";
  	  $curl1 = "";
  	//}
    
    /*$obj->ecolSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );*/
    $obj->ecolEdit   = new Column( "edit", "edit",               "[editbar|edit,del]" );

    $obj->ecolPlace  = new Column( "place", "баллы(place)",               "[atext|place|style='width:40']" );
    $obj->colPlace   = new Column( "place", "баллы",               "<center>[plain|place]</center>" );

    $obj->ecolNumber = new Column( "number", "номер",               "[atext|number|style='width:40']","[plain|number]" );
    $obj->colNumber  = new Column( "number", "номер",               "[plain|number]" );

    $obj->colName    = new Column( "name",   "команда",          $curl0."[plain|name]".$curl1, "[plain|name]" );

    $obj->colCap     = new Column( "cap",    "капитан",               "<A href='db.php?id=p[plain|idCap]'><nobr>[plain|cap]</nobr></a>" );

    $N = 10;
    for( $k=1; $k<=$N; $k++ ) {
      $colName = "colCouple$k";
      $colCode = "couple$k";
      $obj->$colName = new Column( $colCode, "пара $k",    
                                   "<A href='db.php?id=c[plain|idcouple$k]'>[plain|couple$k]</a>", "[plain|couple$k]" );
    }
    
    return $obj;  }
  
  //===========
  
  public function makeRegList( $compProducer ) {
    $teams = $this->loadTeamsTable( $compProducer );
    
    $ncouples = $this->countTeamsTableCouples( $teams );
//    echo "$ncouples";
//    print_r($teams);
    
    $cols = $this->makeColumns();
    
    $template = array(                     
                  $cols->colNumber,  
                  $cols->colName, 
                  $cols->colCap 
                      );
    for( $k=1; $k<=$ncouples; $k++ ) {
      $colName = "colCouple$k";
      $template[] = $cols->$colName;
    }
                      
    $table= new Table( $template );
    
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );
    $editmode = Motl::getParameter( "editmode", 0 );
    $s = $table->produce( "DbCoupleEventProducer", $teams, "$script?id=$id" );
    
    return $s;
  }
  
  //===========
  
  public function makeResults( $compProducer ) {
    // TO DO !!!
  }
  
  //===========
  
  private function loadTeamsTable( $compProducer ) {
    $idComp = (int) $compProducer->id;
    $teamRows = MotlDB::queryRows( "SELECT db_teams.* "
                                  ."FROM db_teams INNER JOIN db_couple_event "
                                                 ."ON db_couple_event.idTeam=db_teams.id "
                                  ."WHERE db_couple_event.idEvent=$idComp" );  // COD $idComp
                                  
    for( $k=0; $k<count($teamRows); $k++ ) {
      
      $teamRows[$k]->cap = $this->loadPersonText( $teamRows[$k]->idCap );
      
      $idcouples = explode( ",", $teamRows[$k]->couples );
      $teamRows[$k]->ncouples = count($idcouples);
      
      for( $i=0; $i<count($idcouples); $i++ ) {
        $i1 = $i+1;
        $fidname = "idcouple$i1";
        $fname = "couple$i1";
        $teamRows[$k]->$fidname = $idcouples[$i];
        $teamRows[$k]->$fname = $this->loadCoupleText( $idcouples[$i], "&nbsp;" );
      }
    }
    return $teamRows;
  }
  
  //-----------
  
  private function countTeamsTableCouples( $teams ) {
    $ncouples = 0;
    for( $k=0; $k<count($teams); $k++ ) {
      $ncouples = max($ncouples,$teams[$k]->ncouples);
    }
    return $ncouples;
  }
  
  //===========
  
  private function makeTeamsTable( $compProducer, $rows, $templatePair, $edit=false ) {
    // TO DO
  }
  
  //===========
  /*
  public function processListEdit( $compProducer ) {
    // TO DO !
  }
  
  //===========
  
  private function saveListEdit( $compProducer ) {
    // TO DO
  }
  
  //===========
  
  private function finishAddDancer( $compProducer ) {
    // TO DO
  }
  */
  //===========
  
}

//=====================

?>
