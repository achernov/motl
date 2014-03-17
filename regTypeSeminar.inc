<?php
//=====================
require_once "regType.inc";
//=====================

class RegTypeHandlerSeminar extends RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
    return $code=="seminar";
  }
  
  //===========
  
  public function getCanonicalCode() {
    return "seminar";
  }
  
  //===========
  
  public function getLabel() {
    return "участники семинара";
  }
  
  //===========
  
  public function makeForm( $idEvent ) {
  
    $comps = $this->getSuitableComps( $idEvent );

    $tiphnd = "";

    $s = "";

    $s .= "<script src='regForm.js'></script>\n";

    $s .= "<form action='regFormReceiver.php' method='POST' onsubmit='return checkFormSeminar(this)'>\n";

    $s .= "<input type='hidden' name='id' value='$idEvent'>\n";
    $s .= "<input type='hidden' name='regType' value='".$this->getCanonicalCode()."'>\n";
    $s .= "<input type='hidden' name='submitted' value='1'>\n";

    $s .= "<table border=0 class='MainText'>";
    $s .= "<tr> <td> </td><td> <small></small> </td><td> <small></small> </td><td> <small></small> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Фамилия </td><td> <input $tiphnd class='regInput2' type='text' name='surname'> </td>";
    $s .= "<tr> <td class='regLabel'> Имя </td><td> <input $tiphnd class='regInput2' type='text' name='name'> </td>";
    //$s .= "<tr> <td class='regLabel'> Город  </td><td colspan=2> <input $tiphnd class='regInput2' type='text' name='city'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Клуб   </td><td colspan=2> <input $tiphnd class='regInput2' type='text' name='club'> </td> </tr>";
    //$s .= "<tr> <td class='regLabel'> Статус </td><td colspan=2> <input $tiphnd class='regInput2' type='text' name='status'> </td> </tr>";
    $s .= "<tr> <td class='regLabel'> Организация </td><td colspan=2> <input $tiphnd class='regInput2' type='text' name='organization'> </td> </tr>";

	$checks = $this->makeChecks( $comps );
	if( $this->checksVisible($checks) ) {
		$s .= "<tr> <td colspan=3><h2>Программы</h2></td> </tr>";
		$s .= "<tr> <td></td><td colspan=2>";
		$s .= $checks;
		$s .= "</td> </tr>";
	}
	else {
		$s .= $checks;
	}

    $s .= "<tr> <td> </td> <td colspan=2> &nbsp;<br><input type='submit' value='Зарегистрировать' > </td> </tr>";

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
    
      $regIds = $this->registerSeminarByRequest();
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

  public function registerSeminarByRequest() {

    $surname   = Motl::getParameter( "surname" );
    $name      = Motl::getParameter( "name" );
    $city =     Motl::getParameter( "city" );
    $club =     Motl::getParameter( "club" );
    $organization =     Motl::getParameter( "organization" );
    $status =   Motl::getParameter( "status" );

    $compIds = $this->getCheckIds();

//    $idPerson  = $this->storePerson( $surname, $name );

    $regIds = array();

    for( $k=0; $k<count($compIds); $k++ ) {

      $idEvent = $compIds[$k];

      $id = MotlDB::saveRow( "regperson", "id",null, 
                  array( "idEvent"=>$idEvent, 
							"surname"=>$surname,
							"name"=>$name,
							"city"=>$city,
							"club"=>$club,
							"organization"=>$organization,
							"status"=>$status,
						 ), 
                  true 
			);

      $regIds[] = $id;
    }

//    DataUtil::updateEventBase();

    return $regIds;
  }
  
  //===========
  
  public function reportRegistration( $sRegIds ) {
  
    ASSERT_ints($sRegIds);
  
    $idComps = MotlDB::queryValues( "SELECT idEvent FROM regperson WHERE id In($sRegIds)", "" );  // ASS $sRegIds
	$sidComps = implode( ",", $idComps );

    $comps = MotlDB::queryRows( "SELECT * FROM db_events WHERE id In($sidComps)" );
    
    $idEvent = $comps[0]->pid;
    $eventRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id='$idEvent'", "" );

    $coupleRow = MotlDB::queryRow( "SELECT DISTINCT surname, name FROM regperson WHERE id In($sRegIds)" );

    $s = "";

    $s .= "<h1>Соревнования: $eventRow->title</h1>";

    $dep = new DbEventProducer($eventRow);

    $s .= $dep->makeSwitch( "db.php" );

    $s .= "<b>$coupleRow->surname $coupleRow->name</b> "
         ."зарегистрирован(а)";
//         ."зарегистрирован(а) на следующие мероприятия:<br>";
/*
    $s .= "<div style='margin-left:25;margin-top:5'>";
    for( $k=0; $k<count($comps); $k++ ) {
      $row = $comps[$k];

      $s .= "<A href='db.php?id=e$idEvent,c$row->id'>$row->title</a><br>";
    }
    $s .= "</div>";
*/
    return $s;
  }
  
  //===========

  public function makeRegList( $compProducer ) {
	//print_r($compProducer);die('q');
	$id = $compProducer->row->id;
    $rows = MotlDB::queryRows( "SELECT * FROM regperson WHERE idEvent=$id" );
	//print_r($rows);
	
	
	$editmode = Motl::getParameter( "editmode", 0 );
	
	$template = array();
	if( $editmode ) {
//		$template[] = new Column( "edit", "edit",     "[editbar|edit,del]" );
	}
	$template[] = new Column( "surname", "Фамилия",     "[plain|surname]" );
	$template[] = new Column( "name",    "Имя",         "[plain|name]" );
	$template[] = new Column( "club",    "Клуб",        "[plain|club]" );
	$template[] = new Column( "org",     "Организация", "[plain|organization]" );
	
	$table= new Table( $template );
	$script = $_SERVER['PHP_SELF'];
	$thisId = $compProducer->row->pid;
	$table->sortBy = Motl::getParameter( "sort", "surname" );
	$s = $table->produce( "DbCoupleEventProducer", $rows, "$script?id=e$thisId,c" );
	return $s;
	
	$s = "";
	$s .= "<table cellspacing=0 border=1>";
		$s .= "<tr>";
		$s .= "<td class='DbTableHeader'>Фамилия</td>";
		$s .= "<td class='DbTableHeader'>Имя</td>";
		$s .= "<td class='DbTableHeader'>Клуб</td>";
		$s .= "<td class='DbTableHeader'>Организация</td>";
		$s .= "</tr>";
	foreach( $rows as $row ) {
		$s .= "<tr>";
		$s .= "<td class='DbTableCell'>$row->surname&nbsp;</td>";
		$s .= "<td class='DbTableCell'>$row->name&nbsp;</td>";
		$s .= "<td class='DbTableCell'>$row->club&nbsp;</td>";
		$s .= "<td class='DbTableCell'>$row->organization&nbsp;</td>";
		$s .= "</tr>";
	}
	$s .= "</table>";
	return $s;
  }
  
  //===========
 }

//=====================

?>
