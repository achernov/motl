<?php
//=====================
require_once "dbEvent.inc";
//=====================

class Registrator {

  //===========
  
  public function makeSoloForm( $idEvent ) {
  
	if( ! $this->isRegOpen($idEvent) ) {
		return "Сейчас нет онлайн-регистрации на этот турнир";
	}

	$handler = new RegTypeHandlerSolo();
    $comps = $this->getSuitableComps( $handler, $idEvent );

//    $tiphnd = "onblur='onBlur()' onkeyup='onKeyUp(this)' onkeydown='onKeyDown(event)'";
    $tiphnd = "nblur='onBlur()' onkeyup='onKeyUp(this)'";
//    $tiphnd = "";

    $s = "";

    $s .= "<script src='regForm.js'></script>\n";
    $s .= "<script src='ajax.js'></script>\n";

      $s .= "<form name=RegForm action='regFormReceiver.php' method='POST' onsubmit='return checkFormSolo(this)'>\n";

    $s .= "<input type='hidden' name='id' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='regType' value='".$handler->getCanonicalCode()."'>\n";
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
 
  public function storePerson( $surname, $name, $sex=null ) {

    $id = MotlDB::queryValue( "SELECT id FROM db_person WHERE name='".MRES($name)."' and surname='".MRES($surname)."'", null );  

    $fields = array();
    if( ! $id ) { // Don't overwrite if exists - not to spoil case
      $fields[ "name" ]    = trim($name);
      $fields[ "surname" ] = trim($surname);
    }
    if( $sex ) { $fields[ "sex" ]    = trim($sex); }
    //if( $birthdate ) { $fields[ "birthdate" ]    = trim($birthdate); }

    if( count($fields) ) {
      $id = MotlDB::saveRow( "db_person", "id",$id, $fields );
    }
    
    return $id;
  }

  //----------------

  public function storePersonEx( $surname, $name ) {

    $ids = MotlDB::queryValues( "SELECT id FROM db_person WHERE name='".MRES($name)."' and surname='".MRES($surname)."'", null );
    if( count($ids) ) {
      return $ids;
    }
    //print_r($ids);die();

    $fields = array(
                "name"    => trim($name),
                "surname" => trim($surname)
              );

    $id = MotlDB::saveRow( "db_person", "id",null, $fields );
    
    return array($id);
  }
  
  //===========

  public function storeCouple( $idMan, $idLady ) {
    ASSERT_int($idMan);
    ASSERT_int($idLady);
    
    // ASSS $idMan, $idLady
    $id = MotlDB::queryValue( "SELECT id FROM db_couples WHERE idMan='$idMan' and idLady='$idLady'", null );  

    if( ! $id ) {
      $id = MotlDB::saveRow( "db_couples", "id",null, array("idMan"=>$idMan, "idLady"=>$idLady) );
    }

    return $id;
  }

  //-----------
  // SQLPARAM $idsMan,$idsLady  - int[]
  public function findCouple( $idsMan, $idsLady ) {
        
    $sidsMan  = implode( ",", $idsMan );
    $sidsLady = implode( ",", $idsLady );
    
    ASSERT_ints($sidsMan);
    ASSERT_ints($sidsLady);
    
    // ASS $idMan, $idLady
    $ids = MotlDB::queryValues( "SELECT id FROM db_couples WHERE idMan In($sidsMan) and idLady In($sidsLady)", null );  
    return $ids;
  }

  //===========

  public function getCheckIds() {
    $ids = array();
    foreach( $_REQUEST as $key=>$value ) {
      if( (substr($key,0,5)=="prog_") && $value ) {

        $ids[] = substr( $key, 5 );
        
      }
    }
    return $ids;
  }
  
  //===========
  
  public function makeChecks( $comps ) {
  
    $s = "";
  
    $achecks = array();
    
    $table = "<table class='MainText'>";
    for( $k=0; $k<count($comps); $k++ ) {
      
      $ctitle = $comps[$k]->title;
      
      if( $comps[$k]->type == "separator" ) {
		$sepClass = "separator".$comps[$k]->ratingLevel;
        //$table .= "<tr> <td> </td> <td colspan=2> <b><i>$ctitle</i></b> </td> </tr>";
        $table .= "<tr> <td> </td> <td colspan=2> <span class='$sepClass'>$ctitle</span> </td> </tr>";
      }
      else {
        $cid = $comps[$k]->id;
        $table .= "<tr> <td> </td> <td colspan=2> <input type='checkbox' name='prog_$cid'> $ctitle </td> </tr>";
        $achecks[] = "prog_$cid";
      }
      
    }
    $table .= "</table>";
    
	if( count($achecks) == 1 ) {
		$s .= "<input type='hidden' name='$achecks[0]' value='on'>";
	}
	else {
		$s .= $table;
	}
	
    $checks = "'" . implode( "','", $achecks ) . "'";
    $s .= "<script>var checks=Array($checks);</script>";
    
    return $s;
  }
  
  //===========
    
  protected function isRegOpen( $idEvent ) {
	if( User::isAdmin() ) { return true; }
	return MotlDB::queryValue( "SELECT regOpen FROM db_events WHERE id='".MRES($idEvent)."'" );
  }
  
  //===========

  public function getSuitableComps( $handler, $idEvent, $strict=false ) {
    ASSERT_int($idEvent);
  
    //--- Load
    $comps = MotlDB::queryRows( "SELECT * FROM db_events WHERE pid='$idEvent' and type In('competition','separator') ORDER BY position,id" );  // ASS $idEvent

//	echo "<pre>";
	//print_r( $comps );
	
    //--- Filter
	
	$currentTopLevel = '0';
	
	$itemsBuffer = array();
	
	for( $k=count($comps)-1; $k>=0; $k-- ) {
		
		$item = $comps[$k];
		$title = $item->title;
		//print_r( $item );
		
		if( $item->type=='competition' ) {
			if( $handler->codeIsMine($item->regType,"regform") ) {
//echo "G $title<br>";
			$itemsBuffer[] = $item;
				$currentTopLevel = 'Z'; // allow any separator
			}
			else {
//echo "B $title<br>";
//				$currentTopLevel = '0'; // block any separator
			}
		}
		else
		if( $item->type=='separator' ) {
			
			$itemLevel = $item->ratingLevel ? $item->ratingLevel : 'X'; // rather low, but not 'Z'
			
			if( $itemLevel < $currentTopLevel ) {
//echo "S $itemLevel $title<br>";
				$itemsBuffer[] = $item;
				$currentTopLevel = $itemLevel; // No more separators of the same level
			}
			else {
//echo "q $currentTopLevel $title<br>";
			}
		}
	}
	
    //--- Invert buffer
	$itemsBuffer = array_reverse( $itemsBuffer );
    //--- 
//print_r( $itemsBuffer );
	//die();
	return $itemsBuffer;
  }
  
  //===========
  
  public function makeCoupleForm( $idEvent ) {
  
	if( ! $this->isRegOpen($idEvent) ) {
		return "Сейчас нет онлайн-регистрации на этот турнир";
	}

	$handler = new RegTypeHandlerCouple();
	
    $comps = $this->getSuitableComps( $handler, $idEvent );

//    $tiphnd = "onblur='onBlur()' onkeyup='onKeyUp(this)' onkeydown='onKeyDown(event)'";
    $tiphnd = "nblur='onBlur()' onkeyup='onKeyUp(this)'";
//    $tiphnd = "";

    $s = "";

    $s .= "<script src='regForm.js'></script>\n";
    $s .= "<script src='ajax.js'></script>\n";

    $s .= "<form name=RegForm action='regFormReceiver.php' method='POST' onsubmit='return checkFormCouple(this)'>\n";

    $s .= "<input type='hidden' name='id' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='regType' value='".$handler->getCanonicalCode()."'>\n";
    $s .= "<input type='hidden' name='submitted' value='1'>\n";

    $s .= "<table border=0 class='MainText'>";
    $s .= "<tr> <td> </td><td> <small>Фамилия</small> </td><td> <small>Имя</small> </td><td> <small>Дата рождения (дд.мм.гггг)</small> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Партнер </td><td> <input $tiphnd class='regInput1' type='text' name='msurname'> </td><td> <input class='regInput1' type='text' name='mname'> </td><td> <input class='regInput1' type='text' name='mbirthdate'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Партнерша </td><td> <input class='regInput1' type='text' name='fsurname'> </td><td> <input class='regInput1' type='text' name='fname'> </td><td> <input class='regInput1' type='text' name='fbirthdate'> </td>  </tr>";
    
    $s .= "<tr> <td class='regLabel'> Класс </td><td colspan=2> <select id='cls' name='class'><option><option>N<option>E<option>D<option>C<option>B<option>A<option>S<option>M</select> </td> "
         ."<td rowspan=8 valign=top>"
            ."<div id='found' style='width:350;padding:20;'></div>"
         ."</td>"
         ."</tr>";
 
    $s .= "<tr> <td>&nbsp;</td> </tr>";

    $s .= "<tr> <td class='regLabel'> Клуб   </td><td colspan=2> <input class='regInput2' type='text' name='club'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Руководитель</td><td colspan=2> <input class='regInput2' type='text' name='boss'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Тренер </td><td colspan=2> <input class='regInput2' type='text' name='teacher'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Город  </td><td colspan=2> <input class='regInput2' type='text' name='city'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Страна </td><td colspan=2> <input class='regInput2' type='text' name='country'> </td> </tr>";

    $s .= "<tr> <td colspan=3><h2>Программы</h2></td> </tr>";

    $s .= "<tr> <td></td><td colspan=2>";
    $s .= $this->makeChecks( $comps );
    $s .= "</td> </tr>";

    $s .= "<tr> <td> </td> <td colspan=2> &nbsp;<br><input type='submit' value='Зарегистрировать' > </td> </tr>";

    $s .= "</table>";

    $s .= "</form>\n";


    $s .= "Со всеми вопросами и проблемами при пользовании онлайн регистрацией "
         ."обращайтесь к администратору сайта: <A href='mailto:admin@danceleague.ru'>admin@danceleague.ru</a><br>\n";

    return $s;
  }
  
  //===========
  
  public function processUserInputCouple( $handler ) {
  
    if( Motl::getParameter( "submitted", 0 ) ) {
      $script = $_SERVER['PHP_SELF'];
    
      $regIds = $this->registerCoupleByRequest();
      $sids = implode( ",", $regIds );

      $type = $handler->getCanonicalCode();
      Motl::redirect( "$script?regType=$type&regIds=$sids" );
    }
    else
    if( $sids = Motl::getParameter( "regIds" ) ) {
      
      return $this->reportRegistration( $sids );
      
    }
    
  }
  
  //===========

  public function processUserInputSolo( $handler ) {

    if( Motl::getParameter( "submitted", 0 ) ) {
      $script = $_SERVER['PHP_SELF'];
    
      $regIds = $this->registerSoloByRequest();
      $sids = implode( ",", $regIds );

      $type = $handler->getCanonicalCode();
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
						 "mbirthdate"=>$birthdate,
                         "raw_club"=>$club, "raw_boss"=>$boss, "raw_teacher"=>$teacher, 
                         "raw_city"=>$city, "raw_country"=>$country ), 
                  true );

      $regIds[] = $id;
    }

    DataUtil::updateEventBase();

    return $regIds;
  }
  
  //===========
    
  public function registerCoupleByRequest() {

    $msurname   = Motl::getParameter( "msurname" );
    $mname      = Motl::getParameter( "mname" );
    $mbirthdate = Motl::getParameter( "mbirthdate" );
    
    $fsurname   = Motl::getParameter( "fsurname" );
    $fname      = Motl::getParameter( "fname" );
    $fbirthdate = Motl::getParameter( "fbirthdate" );
    
    $class =    Motl::getParameter( "class" );
    $club =     Motl::getParameter( "club" );
    $boss =     Motl::getParameter( "boss" );
	$teacher =  Motl::getParameter( "teacher" );
    $city =     Motl::getParameter( "city" );
    $country =  Motl::getParameter( "country" );

    $compIds = $this->getCheckIds();

    $idMan = Motl::getParameter( "idMan" );
    $idsMan  = $idMan ? array($idMan) : $this->storePersonEx( $msurname, $mname, "M" );
    
    $idLady = Motl::getParameter( "idLady" );
    $idsLady = $idLady ? array($idLady) : $this->storePersonEx( $fsurname, $fname, "F" );
    
    //echo "<pre>";
//    $idsMan = array(1132);
//    $idsMan[] = 2145;
    //print_r( $idsMan );
    //print_r( $idsLady );
    
    $idsCouple = $this->findCouple( $idsMan, $idsLady );
    
    $nCouples = count($idsCouple);
    $nMen = count($idsMan);
    $nLadies = count($idsLady);
    
    switch( $nCouples ) {
    
      case 1:
        $idCouple = $idsCouple[0];
        break;
        
      case 0:
        {
          if( $nMen==1 && $nLadies==1 ) { // No couples, NO ambiguity
            $idCouple = $this->storeCouple( $idsMan[0], $idsLady[0] );
            break;
          }
          else { // No couples, AMBIGUITY
            $this->renderDisambiguation( $idsMan, $idsLady );
            die();
          }
        }
        
      default: // Many couples
        $this->renderDisambiguation( $idsMan, $idsLady );
        die();
    }
    //die("QQQQQ");

    $regIds = array();

    for( $k=0; $k<count($compIds); $k++ ) {

      $idEvent = $compIds[$k];

      $id = MotlDB::saveRow( "db_couple_event", "id",null, 
                  array( "idEvent"=>$idEvent, "idCouple"=>$idCouple,
                         "mbirthdate"=>$mbirthdate,
                         "fbirthdate"=>$fbirthdate,
                         "class"=>$class, "raw_club"=>$club, "raw_boss"=>$boss, "raw_teacher"=>$teacher, 
                         "raw_city"=>$city, "raw_country"=>$country ), 
                  true );

      $regIds[] = $id;
    }

    DataUtil::updateEventBase();

    return $regIds;
  }
  
  //===========
  
  private function renderDisambiguation( $idsMan, $idsLady ) {
    
    $content = $this->makeDisambiguation( $idsMan, $idsLady );
    
    $layout = new MotlLayout(); 
    $layout->mainPane = $content;
    $layout->write();
  }

  //===========

  private function makeDisambiguation( $idsMan, $idsLady ) {
    
    $s = "";
    $s .= "<h1>Регистрация - требуется уточнение:</h1>";
    
    $awhom = array();
    if( count($idsMan)  > 1 ) { $awhom[] = "несколько партнеров";  }
    if( count($idsLady) > 1 ) { $awhom[] = "несколько партнерш";  }
    $whom = implode( " и ", $awhom );
    
    $s .= "В базе данных есть $whom с такими именами и фамилиями.<p>";
    $s .= "Пожалуйста, уточните, кого именно следует зарегистрировать:<p>";
    
    $selMen    = $this->makePersonDisambiguation( $idsMan,  "name='idMan'" );
    $selLadies = $this->makePersonDisambiguation( $idsLady, "name='idLady'" );
    
    $s .= "<form action='regFormReceiver.php' method='POST' >";
    
    $s .= $this->copyParameter( "id" );
    $s .= $this->copyParameter( "regType" );
    $s .= $this->copyParameter( "submitted" );
    $s .= $this->copyParameter( "msurname" );
    $s .= $this->copyParameter( "mname" );
    $s .= $this->copyParameter( "mbirthdate" );
    $s .= $this->copyParameter( "fsurname" );
    $s .= $this->copyParameter( "fname" );
    $s .= $this->copyParameter( "fbirthdate" );
    $s .= $this->copyParameter( "class" );
    $s .= $this->copyParameter( "club" );
    $s .= $this->copyParameter( "teacher" );
    $s .= $this->copyParameter( "city" );
    $s .= $this->copyParameter( "country" );
    
    $s .= $this->copyProgParameters();

    $s .= "<table class='mainText'>";
    $s .= "<tr> <td>Партнер:</td> <td>$selMen</td> </tr>";
    $s .= "<tr> <td>Партнерша:</td> <td>$selLadies</td> </tr>";
    $s .= "<tr> <td colspan='2'><input type='submit' value='Завершить регистрацию'></td> </tr>";
    $s .= "</table>";
    $s .= "</form>";
    //die();
    return $s;
  }
  
  //-----------
  
  private function copyParameter( $name ) {
    $value = Motl::getParameter( $name, "" );
    return "<input type='hidden' name='$name' value='$value'>\n";
  }
  
  //-----------
  
  private function copyProgParameters() {
    $s = "";
    foreach( $_REQUEST as $key=>$value ) {
      if( (substr($key,0,5)=="prog_") && $value ) {
        $s .= $this->copyParameter( $key );
      }
    }
    return $s;
  }
  
  //-----------
  
  private function makePersonDisambiguation( $ids, $selProps ) {
    $sids  = implode( ",", $ids );
    ASSERT_ints($sids);
    
    $rows = MotlDB::queryRows( "SELECT * FROM db_person WHERE id In($sids)" ); // ASS $sids
    
    $s = "<select $selProps>";
    $model = new DbPersonProducer();
    foreach( $rows as $k=>$row ) {
      $s .= $model->makeText( $row, "<option value='[plain|id]'>[plain|surname] [plain|name] [if|disambig|<i>([plain|disambig])</i>]" );
    }
    $s .= "</select>";
    
    return $s;
  }
  
  //===========

  public function reportRegistration( $sRegIds ) {

    ASSERT_ints($sRegIds);
    $subjRow   = MotlDB::queryRow( "SELECT idCouple,idSolo FROM db_couple_event WHERE id In($sRegIds)", "" );  // ASS $sRegIds

    $comps = MotlDB::queryRows( "SELECT e.* FROM db_couple_event as ce INNER JOIN db_events as e ON ce.idEvent=e.id WHERE ce.id In($sRegIds)" );  // ASS $sRegIds
    
    $idEvent = $comps[0]->pid;
    $eventRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id='$idEvent'", "" );  // FLD $idEvent

    $coupleRow = MotlDB::queryRow( "SELECT m.surname as msurname, m.name as mname, f.surname as fsurname, f.name as fname FROM db_couples as c INNER JOIN db_person as m ON c.idMan=m.id INNER JOIN db_person as f ON c.idLady=f.id WHERE c.id=$subjRow->idCouple" ); 
    $dancerRow = MotlDB::queryRow( "SELECT surname, name FROM db_person WHERE id=$subjRow->idSolo" );  

    $s = "";

    $s .= "<h1>Соревнования: $eventRow->title</h1>";

    $dep = new DbEventProducerFull($eventRow);

    $s .= $dep->makeSwitch( "db.php" );

	if( $coupleRow ) {
		$s .= "Пара <b>$coupleRow->msurname $coupleRow->mname "
					."- $coupleRow->fsurname $coupleRow->fname</b> "
			 ."зарегистрирована по следующим программам:<br>";
	}
	else {
		$s .= "<b>$dancerRow->surname $dancerRow->name</b> "
			."зарегистрирован(а) по следующим программам:<br>";
	}

    $s .= "<div style='margin-left:25;margin-top:5'>";
    for( $k=0; $k<count($comps); $k++ ) {
      $row = $comps[$k];

      $s .= "<A href='db.php?id=e$idEvent,c$row->id'>$row->title</a><br>";
    }
    $s .= "</div>";

    return $s;
  }
  
  //===========

}

//=====================
?>