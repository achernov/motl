<?php
require_once "dbAll.inc";
require_once "tabs.inc";
require_once "tables.inc";
//die("dbEvent.inc");
//=====================

class DbEventProducerFull extends DbEventProducer {
//st5 Вальс,Танго,Венский вальс,Медленный фокстрот,Квикстеп
//la5 Ча-ча-ча,Самба,Румба,Пасодобль,Джайв
//3   Вальс,Полька,Ча-ча-ча
//4   Вальс,Квикстеп,Ча-ча-ча,Джайв
//N5  Вальс,Квикстеп,Ча-ча-ча,Румба,Джайв
//N5s Вальс,Квикстеп,Ча-ча-ча,Самба,Джайв
//6   Вальс,Венский вальс,Квикстеп,Ча-ча-ча,Румба,Джайв
//8   Вальс,Танго,Венский вальс,Квикстеп,Ча-ча-ча,Самба,Румба,Джайв
//10  Вальс,Танго,Венский вальс,Медленный фокстрот,Квикстеп,Ча-ча-ча,Самба,Румба,Пасодобль,Джайв
  //----------------
  
  public function makeViewLink( $label, $id=null ) {
	$url = "db.php?id=e$id";
	return "<A href='$url'>$label</a>";
  }
  
  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    switch( $format ) {
      case 'shortRegForm': return $this->produceShortRegistration(); 
      case 'shortResults': return $this->produceShortResults(); 
      case 'short': return $this->produceShort(); 
      case 'full':  return $this->produceFull(); 
      case 'edit':  return $this->produceEdit(); 
    }

  }

  //----------------

  public function produceShort() {
    //$script = $_SERVER['PHP_SELF'];
    $script = "db.php";
    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";

    $template = "[editbar|edit,del|&nbsp;]<A href=$script?id=e[plain|id]><b>[plainDateRange|date|date1] &nbsp; [plain|title]</b></a><br>";
    
    $s .= $this->MakeText( $this->row, $template );
/*
	  if( $this->row->type == 'event' ) {
        $s .= $this->makeSwitch( $script );
	  }
*/
	$s .= "<p>";
	
    return $s;
  }

  //----------------

  public function produceShortRegistration() {
    //$script = $_SERVER['PHP_SELF'];
    $script = "db.php";

    $s = "";

//    $templateTitle = "<b>[plainDateRange|date|date1] &nbsp; [plain|title]</b>[if|status|<br>[plain|status]]";
    $templateTitle = "<b>[plainDateRange|date|date1] &nbsp; [plain|title]</b>";
    
    $template = "[editbar|edit,del|&nbsp;]$templateTitle<br>";
    
    $s .= $this->MakeText( $this->row, $template );
	
	if( ! $this->row->registration ) {
		$s .= $this->makeSwitch( $script );
	}
	else {
		$s .= "<div style='margin-top:5;margin-bottom:20'>";
		$s .= $this->row->registration;
		$s .= "</div>";
	}

    return $s;
  }

  //----------------

  public function produceShortResults() {
    //$script = $_SERVER['PHP_SELF'];
    $script = "db.php";
    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";

    $templateTitle = "<b>[plainDateRange|date|date1] &nbsp; [plain|title]</b>";
    
    if( ! $this->row->results ) {
      $template = "<A href=$script?id=e[plain|id],r>$templateTitle</a><br>";
    }
    else {
      $template = "$templateTitle<br>[plain|results]<p>";
    }
    //$template = "<div style='margin-bottom:20'>[editbar|edit,del|&nbsp;]$template</div>";

      $idEvent = $this->row->id;
      ASSERT_int($idEvent);

      // ASS $idEvent
      $sql = "SELECT 1 "
            ."FROM db_events "
            ."WHERE id=$idEvent AND ( "
            ."idGallery or "
            ."EXISTS (SELECT 1 FROM db_events AS comp1 WHERE pid=$idEvent and idGallery<>'') "
            ."or "
            ."EXISTS (SELECT 1 FROM db_events AS rnd INNER JOIN db_events AS comp ON rnd.pid=comp.id WHERE comp.pid=$idEvent and rnd.idGallery<>'') "
            .");";
      $haveMedia = MotlDB::queryValue( $sql );  
      
      $idMedia = "e$idEvent,m";
      //$idMedia = null;
      $galleryLink = $this->makeGalleryLink( $idMedia );
      $galleryLinkIfPresent = $haveMedia ? $galleryLink : "";
      
    $template = "<table class=mainText style='margin-bottom:20;margin-left:-5'><tr> "
               ."<td width=15>$galleryLinkIfPresent</td> "
               ."<td style='padding-left:10'>[editbar|edit,del|&nbsp;]$template</td> "
               ."</tr></table>";

    $s .= $this->MakeText( $this->row, $template );

    return $s;
  }

  //----------------
  
  public function produceFull() {
    $script = $_SERVER['PHP_SELF'];
//die();
    $sql = "SELECT * FROM db_events WHERE pid={$this->row->id} AND type='competition'";
    $compRows = MotlDB::queryRows( $sql );
	if( count($compRows)==1 && $compRows[0]->title=='' ) {
		//$comp = new DbEventProducerFull($compRows[0]);
		//return $comp->produceFull();
		$this->id .= $compRows[0]->id;
		$this->aid[1] .= $compRows[0]->id;
	}

    
    if( $this->row->type=='competition' || $this->row->type=='round' ) {
      $id = $this->row->id;
      $ids = $this->getIds();
      Motl::redirect( "$script?id=e$ids->idEvent,c$id" );
    }
    
    $s = "";

    $s .= "<script src='db.js'></script>";
    $s .= "<script src='tip.js'></script>";

    //$s .= "<h1>Соревнования: " . $this->row->title . "</h1>";
    $s .= $this->MakeText( $this->row, "<h1>[plainDateRange|date|date1] &nbsp; [plain|title]</h1>" );
    $s .= $this->MakeText( $this->row, "[if|caution|<div style='color:red;font-size:18;font-weight:bold'>[plain|caution]</div>|]" );


	  if( $this->row->havePlaces || $this->row->haveScores ) {
	    Motl::$Rubric = "res";
	  }
    else
	  if( $this->row->regOpen ) {
	    Motl::$Rubric = "reg";
	  }
    
    if( $this->checkSorting() ) {
      $s .= $this->produceSorting();
      return $s;
    }
	
    $param = isset( $this->aid[1] ) ? $this->aid[1] : "";

    $thisId = $this->row->id;
    $returl = Motl::makeRetUrl();
    $initLink = "<A class='editLink' href='initEvent.php?id=$thisId&returl=$returl'>Init</a>";
    $s .= $this->MakeText( $this->row, "[editbar|edit,add,sort|&nbsp;&nbsp;$initLink<p>]" );
    
    $mode    = substr($param,0,1);
    $submode = isset( $this->aid[2] ) ? $this->aid[2] : "";

    if( ! $mode ) {
      if( $this->row->havePlaces || $this->row->haveScores )
        $mode = 'r';
      else
      if( $this->row->afisha )
        $mode = 'a';
      else
      if( $this->row->regOpen )
        $mode = 'f';
    }

	  if( $this->row->type == 'event' ) {
      if( $mode != 'm' )
        $s .= $this->makeSwitch( $script, $mode, $submode );
	  }

    switch( $mode ) {

      case 'e':
        $s .= $this->produceMultiEdit();
        break;

      case 'a':
        $s .= $this->produceAfisha();
        break;

      case 's':
        $s .= $this->row->schedule;
        break;

      case 'p':
        $s .= $this->row->prizes;
        break;

      case 'w':
        if( $this->row->way ) {
          $s .= $this->row->way;
        }
        else {
          if( $this->row->idPlace ) {
        //die("SELECT info FROM db_halls WHERE id=".$this->row->idPlace);
            ASSERT_int($this->row->idPlace);
            // ASS $this->row->idPlace
            $s .= MotlDB::queryValue( "SELECT info FROM db_halls WHERE id=" . $this->row->idPlace );  
          }
        }
        
        break;

      case 'f':
        $s .= $this->produceRegForm();
        break;

      case 'm':
        $s .= $this->produceMedia( substr($param,1) );
        break;

      case 'c':
        $s .= $this->produceRegList( substr($param,1) );
        break;

      case 'r':
        $s .= $this->produceResults( substr($param,1) );
        break;

      case 't':
        $s .= $this->produceStats();
        break;
    }

    return $s;
  }

  //----------------

  public function makeSwitch( $script, $mode=null, $submode=null ) {
	require_once "regManager.inc";

    $idEvent = $this->row->id;
    ASSERT_int($idEvent);
	  motlASSERT( $this->row->type=='event', $this->row->type );
    
    /*if( isset($this->aid[1]) && strlen($this->aid[1])>1 ) { // have idComp
      $idComp = substr( $this->aid[1], 1 );
      //echo "$idComp<p>";
    }
    else {*/
      $idComp = "";
    //}

    $haveResults = $this->row->havePlaces || $this->row->haveScores;

    $s = "";
    $s .= "<div style='margin-top:5;margin-bottom:20'>";

    if( ! $haveResults ) { // После турнира эти пункты не нужны
    
      if( $this->row->afisha ) {
        $s .= $this->makeSwitchItem( $mode=="a", "$script?id=e$idEvent,a", "Афиша" );
        $s .= " &nbsp; ";
      }

      if( $this->row->schedule ) {
        $s .= $this->makeSwitchItem( $mode=="s", "$script?id=e$idEvent,s", "Расписание" );
        $s .= " &nbsp; ";
      }

      if( $this->row->way || $this->row->idPlace ) {
        $s .= $this->makeSwitchItem( $mode=="w", "$script?id=e$idEvent,w", "Как проехать" );
        $s .= " &nbsp; ";
      }

      if( $this->row->prizes ) {
        $s .= $this->makeSwitchItem( $mode=="p", "$script?id=e$idEvent,p", "Наградной материал" );
        $s .= " &nbsp; ";
      }

      if( $this->row->regOpen ) {
      
        // ASS $idEvent
        $regTypes = MotlDB::queryValues( "SELECT DISTINCT regType FROM db_events WHERE pid=$idEvent" );  
        
        $regOptions = RegManager::makeOptions( $regTypes );
      
        foreach( $regOptions as $option ) {
          $label = $option->label ? "Регистрация: $option->label" : "Регистрация";
          $code = $option->code;
          
          $active = false;
          if( $mode == "f" ) {
            $handler = RegManager::getHandlerForCode($code,"regform");
            $active = $handler->codeIsMine($submode,"regform");
          }
          
          $s .= $this->makeSwitchItem( $active, "$script?id=e$idEvent,f,$code", $label );
          $s .= " &nbsp; ";
        }
      }
      
    }
    
    //if( $this->row->ncouples ) {
      $s .= $this->makeSwitchItem( $mode=="c", "$script?id=e$idEvent,c$idComp", "Список участников" );
      $s .= " &nbsp; ";
    //}

    if( $haveResults ) {
      $s .= $this->makeSwitchItem( $mode=="r", "$script?id=e$idEvent,r$idComp", "Результаты" );
      $s .= " &nbsp; ";
    }

    if( $mode == "m" ) {
      $s .= $this->makeSwitchItem( $mode=="m", "$script?id=e$idEvent,m$idComp", "Фото/видео" );
      $s .= " &nbsp; ";
    }
    
    if( User::isAdmin() ) { // Пока не откываем общий доступ
      $s .= $this->makeSwitchItem( $mode=="t", "$script?id=e$idEvent,t", "Статистика" );
    }
    
    if( User::isAdmin() ) {
      //$s .= $this->makeSwitchItem( false, "downloadEvent.php?id=e$idEvent", "<font color=green>Download</font>" );
    }

    $s .= "</div>";
    return $s;
  }

  //----------------

  private function makeSwitchItem( $selected, $url, $label ) {
    $editmode = Motl::getParameter( "editmode", 0 );
    return $selected ? "<b>$label</b>" : "<A href='$url'>$label</a>";
  }

  //----------------

  private function produceSorting() {
    $idEvent = $this->row->id;
    ASSERT_int($idEvent);
    
    // ASS $idEvent
    $comps = $this->loadMultipleFromDB( "pid=$idEvent AND type In('competition','separator') ORDER BY position,id" );   
    
    $s = "";
    $s .= SortManager::beginSortForm();

    $s .= "<div style='margin-top:20' onselectstart='return false'>";
    
    for( $k=0; $k<count($comps); $k++ ) {
    
      $child = $comps[$k];
      
      $sChild = $child->makeText( $child->row, "<div style='margin:5;padding:3; border:1px solid black'>[plain|title]</div>" );
//      $sChild = $child->produce("short");
        
      $s .= SortManager::processChild( $child, $sChild);
    }

    $s .= "</div>";

    $s .= SortManager::endSortForm();

    return $s;
  }
  
  //----------------
  
  private function produceMultiEdit() {
	$id = $this->row->id;
	
	$rows = MotlDB::queryRows( 
		"SELECT id,type,regType,groupCode,ratingLevel,title "
		."FROM db_events WHERE pid=$id ORDER BY position "
		//."LIMIT 7" 
		);
								
	$fields = array( "id", "type", "regType", "groupCode", "ratingLevel", "title" );
	$fields = array( "id", "groupCode", "title" );
	//print_r($rows);die();
	return $this->makeMultiEditForm($rows,$fields);
  }
  
  //----------------

  private function produceAfisha() {
    $picFolder = Motl::getPicturesFolder();
    $afisha = $this->row->afisha;
    return "<p><img src='$picFolder/$afisha'>";
  }

  //----------------

  private function produceRegForm() {
	require_once "regManager.inc";
	
    $code = isset( $this->aid[2] ) ? $this->aid[2] : "";
    
    $handler = RegManager::getHandlerForCode( $code, "regform" );

    return $handler->makeForm( $this->aid[0] );
  }

  //----------------
  
  private function produceMedia( $cid ) {
  
    $script = "db.php";
  
    $s = "";
    
    $s .= $this->makeSwitch( $script, "m" );
    
    if( $cid ) {
      ASSERT_int($cid);
      $compProducer = new DbEventProducerFull();
      $compProducer->loadFromDB( $cid );  
      
      $s .= "<h2>".$compProducer->row->title."</h2>";
      
      // ASS $cid
      $galIds = MotlDB::queryValues( "SELECT DISTINCT idgallery FROM db_events WHERE (id=$cid or pid=$cid) and  idgallery" );  
    }
    else {
      $idEvent = $this->row->id;
      ASSERT_int($idEvent);
      
      $cids = MotlDB::queryValues( "SELECT DISTINCT id FROM db_events WHERE pid=$idEvent" );  // ASS $idEvent
      $scids = implode( ",", $cids );

      // COD $scids
      $galIds = MotlDB::queryValues( "SELECT DISTINCT idgallery FROM db_events WHERE (id In($scids) or pid In($scids)) and  idgallery" );  
    }
    
    $prototype = new GalleryItemProducer();
    
    $s .= $prototype->produceItems( $galIds );
    
    return $s;
  }
  
  //----------------

  private function produceRegList( $cid ) {
	require_once "regManager.inc";

    $editmode = Motl::getParameter( "editmode", 0 );

    $s = "";

    if( $cid ) {
      $compProducer = new DbEventProducerFull();
      $compProducer->loadFromDB( $cid );
      $s .= $compProducer->produceCompetitionSelector( 'c' );

      $s .= "<p>";

      $handler = RegManager::getHandlerForCode( $compProducer->row->regType, "reglist" );
      $s .= $handler->makeRegList( $compProducer );
    }
    else {
      $s .= $this->produceCompetitionSelector( 'c' );
    }

    return $s;
  }

  //----------------

  private function produceResults( $crid ) {
	require_once "regManager.inc";

    $editmode = Motl::getParameter( "editmode", 0 );
//die("res $crid");
    $s = "";

    if( $crid ) {
      $compProducer = new DbEventProducerFull();
      $compProducer->loadFromDB( $crid );
      $s .= $compProducer->produceCompetitionSelector( 'r' );

      $s .= "<p>";

      if( $compProducer->row->results ) {
        $s .= $compProducer->row->results;
        return $s;
      }

      $handler = RegManager::getHandlerForCode( $compProducer->row->regType, "results" );
      $s .= $handler->makeResults( $compProducer );
    }
    else {
      $s .= $this->produceCompetitionSelector( 'r' );
    }

    return $s;
  }

  //----------------

  private function produceStats() {

    $s = "";

	$id = (int)$this->row->id;
	
	$idsSolo = MotlDB::queryValues( "SELECT DISTINCT idSolo "	// COD $id
					."FROM db_events AS e INNER JOIN db_couple_event AS ce ON e.id=ce.idEvent "
					."WHERE pid=$id AND idSolo Is Not Null" );
	$nSolo = count($idsSolo);

	$idsCouple = MotlDB::queryValues( "SELECT DISTINCT idCouple "	// COD $id
					."FROM db_events AS e INNER JOIN db_couple_event AS ce ON e.id=ce.idEvent "
					."WHERE pid=$id AND idCouple Is Not Null" );
	$nCouples = count($idsCouple);
					
	$s .= "<b>Всего пар:</b> $nCouples<p>";
	$s .= "<b>Всего солистов:</b> $nSolo<p>";
	
	$s .= "<b>Города:</b><br>" . $this->produceColumnStats( $id, "city" );
	$s .= "<p>";
	
	$s .= "<b>Клубы:</b><br>" . $this->produceColumnStats( $id, "club" );

    return $s;
  }

  //----------------
  
  private function produceColumnStats( $idEvent, $columnName ) {
  
		$ids = MotlDB::queryValues( "SELECT DISTINCT $columnName "
					."FROM db_events AS e INNER JOIN db_couple_event AS ce ON e.id=ce.idEvent "
					."WHERE pid=$idEvent AND $columnName<>'' " );
					
		$sids = implode( ',', $ids );
		$ids  = explode( ',', $sids );
		
		$column = BaseColumn::createColumn( $columnName );
		return $column->makeListImploded( $ids, '<br>', true );
  }
  
  //----------------
  
  private function getIds() {
    
    $ids = new stdClass();
    
    ASSERT_int($this->row->id);;
    
    switch( $this->row->type ) {

      case 'event':
        $ids->idEvent = $this->row->id;
        $ids->idComp  = null;
        $ids->idRound  = null;
        return $ids;

      case 'competition':
        $ids->idComp  = $this->row->id;
        
        // ASS $ids->idComp
        $ids->idEvent = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$ids->idComp'", 0 );  
        
        $ids->idRound  = null;
        return $ids;

      case 'round':
        $ids->idRound = $this->row->id;
        
        // ASS $ids->idRound
        $ids->idComp  = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$ids->idRound'", 0 );  
        
        // COD $ids->idComp
        $ids->idEvent = MotlDB::queryValue( "SELECT pid FROM db_events WHERE id='$ids->idComp'", 0 );  
        
        return $ids;
    }
    
    return null;
  }
  
  //----------------

  private function produceCompetitionSelector( $param ) {
    $editmode = Motl::getParameter( "editmode", 0 );
    
    $ids = $this->getIds();
	
//Q( "produceCompetitionSelector( $param ): idEvent=$ids->idEvent idComp=$ids->idComp idRound=$ids->idRound param=$param" );
    $script = $_SERVER['PHP_SELF'];
    $id0 = $this->aid[0];

    // COD $ids->idEvent
    $sql = "SELECT comp.*, (select count(*) from db_events as rnd where rnd.pid=comp.id and idgallery ) as roundGalleries FROM db_events as comp "
          ."WHERE pid=$ids->idEvent AND type In('competition','separator') ORDER BY position,id";
    $sql = "SELECT comp.*, 0 as roundGalleries FROM db_events as comp "
          ."WHERE pid=$ids->idEvent AND type In('competition','separator') ORDER BY position,id";
    //die($sql);
    
    $compRows = MotlDB::queryRows( $sql );  


    $s = "";

//    $s .= "<b>Выберите группу:</b>";

    $s .= "<table border=0 cellspacing=0 cellpadding=0>";

//	$templateCouples = ($param=='c') ? "[if|ncouples| <i>[numword|ncouples|пара|пары|пар]</i> | ]" : "";

	$showNCouples = ($param=='c');

	$templateCouples = $showNCouples ? "[if|ncouples| <i>[plain|ncouples]</i> | ]" : "";
	
    for( $k=0; $k<count($compRows); $k++ ) {
      $row = $compRows[$k];
      
      $rowDisabled = false;
      if( $param=='r' ) {
        if( $row->type!="separator" && !$editmode /*!User::isAdmin()*/ ) {
			//$rowDisabled = !$row->havePlaces && !$row->haveScores;
			$rowDisabled = !$row->haveNumbers;
        }
      }
      
      $dbi = new DbEventProducerFull($row);

      $idMedia = "e$ids->idEvent,m".$row->id;
      $galleryLink = $this->makeGalleryLink( $idMedia);
      $galleryLinkIfPresent = ( $row->idGallery || $row->roundGalleries ) ? $galleryLink : "";
      
      if( $row->type == "separator" ) {
		if( $editmode ) {
			$spanTemplate = "<span title='уровень сепаратора' class='separatorLevel'>[plain|ratingLevel]</span>";
			$levelTemplate = "[if|ratingLevel|$spanTemplate]";
		}
		else {
			$levelTemplate = "";
		}
		
		$sepClass = "separator".$row->ratingLevel;

        $s .= $dbi->makeText( $row, 
                        "<tr> "
                           ."<td ></td>"
                           ."<td style='padding-top:7;padding-bottom:3' class='dataCell'>[editbar|edit,copy,del]"
								."$levelTemplate<span class='$sepClass'>[plain|title]</span> </td>"
                           ."<td class='dataCell'></td>"
                       ."</tr>" );
      }
      else {
		$labelTemplate = "[plain|title]";
		
		if( $row->id == $ids->idComp ) {
			$dataCell = "dataCellRed";
			$dotted   = "dottedRed";
		}
		else 
		if( $rowDisabled ) {
			$dataCell = "dataCellGray";
			$dotted   = "dotted";
		}
		else {
			$dataCell = "dataCell";
			$dotted   = "dotted";
			$labelTemplate = "<A href=$script?id=e$ids->idEvent,$param"."[plain|id]>$labelTemplate</a>";
		}
		
		if( $editmode ) {
			$spanTemplate = "<span title='класс турнира' class='ratingLevel'>[plain|ratingLevel]</span>";
			$ratingLevelTemplate = "[if|ratingLevel|$spanTemplate]";
			$labelTemplate = $ratingLevelTemplate.$labelTemplate;
		}
		
		if( !$showNCouples || !$row->ncouples ) {
			$dotted = "";
		}
		
        $s .= $dbi->makeText( $row, 
							"<tr> "
								."<td >$galleryLinkIfPresent</td>"
								."<td class='$dataCell'><div class='$dotted'><div class='undotted'>[editbar|edit,copy,del] $labelTemplate</div>&nbsp;</div> </td>"
								."<td class='$dataCell'>$templateCouples</td>"
							."</tr>" );
	  }
    }

    $s .= "</table>";

    return $s;
  }

  //----------------
  
  private function makeGalleryLink( $idMedia ) {
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
  
  protected function makeHallOptions() {
    $rows = MotlDB::queryRows( "SELECT id,name FROM db_halls ORDER BY name" );
    $a = array();
    for( $k=0; $k<count($rows); $k++ ) {
      $r = $rows[$k];
      $a[] = "$r->name:$r->id";
    }
    $s = implode( ";", $a );
    //print_r($s);
    return $s;
  }
  
  //----------------

  public function getFields() {
    $hallOptions = $this->makeHallOptions();
	return array(
		"id" => "[readonly|id]",
		"ts_created" => "[ts|ts_created]",
		"pid" => "[text|pid]",
		"position" => "[text|position]",
		"type" => "[select|type|event;competition;round;separator]",
		"regType" => "[select|regType|couple;solo;solocomp;team;seminar]",
		"groupCode" => "[text|groupCode]",
				
		"ratingLevel" => "[select|ratingLevel|A;B;C]",

		"round" => "[text|round]",
		"title" => "[text|title|style='width:400']",
		"caution" => "[textarea|caution|style='width:400;height:100']",
		"status" => "[textarea|status|style='width:400;height:100']",
		"idGallery" => "[text|idGallery]",
		"date" => "[text|date]",
		"date1" => "[text|date1]",

		"hidden" => "[text|hidden]",

		"dances" => "[text|dances|style='width:400']",
		"judges" => "[textarea|judges|style='width:400;height:150']",
		"registration" => "[textarea|registration|style='width:400;height:100']",
		"results" => "[textarea|results|style='width:400;height:100']",
		"ncouples" => "[text|ncouples]",
		"regOpen" => "[text|regOpen]",
		"haveNumbers" => "[text|haveNumbers]",
		"haveScores" => "[text|haveScores]",
		"havePlaces" => "[text|havePlaces]",

		"afisha" => "[text|afisha|style='width:400']",
		"schedule" => "[textarea|schedule|style='width:600;height:400']",
		"prizes" => "[textarea|prizes|style='width:400;height:200']",
		"way" => "[textarea|way|style='width:400;height:200']",

		"idPlace" => "[select|idPlace|$hallOptions]",
		"placeLabel" => "[textarea|placeLabel|style='width:400;height:200']",
		"organizer" => "[textarea|organizer|style='width:400;height:200']",

		"teamFormat" => "[textarea|teamFormat|style='width:400;height:150']"
	);
  }
  
  //----------------
  
  private function produceEdit() {
    $script = Motl::getParameter("script");
    $id = Motl::getParameter("id");

    $s = "";

    $s .= $this->makePath();
    
    if( $this->isNew ) {
      $this->row->type = "competition";
    }
    
    $s .= $this->makeEditForm();

    return $s;
  }

  //===========
  
  function getDances( $parentRow=null ) {
	$sdances = $this->getDancesString( $parentRow );
	return $sdances ? explode( ",", $sdances ) : null;
  }
  
  //===========
  
  function getDancesString( $parentRow=null ) {
  
	if( $this->row->dances ) {
		return $this->row->dances;
	}
  
	switch( $this->row->type ) {
		case 'competition':
			return $this->dancesFromGroupCode( $this->row->groupCode );
			
		case 'round':
			if( ! $parentRow ) {
				$pid = (int)$this->row->pid;
				$parentRow = MotlDB::queryRow( "SELECT * FROM db_events WHERE id=$pid" ); // COD $pid
			}
			$parent = new DbEventProducerFull( $parentRow );
			return $parent->getDancesString();
	}
	
	return null;
  }
  
  //===========
  
  public function dancesFromGroupCode( $groupCode ) {
    $gcArr  = explode( ".", $groupCode );
    $classCode = $gcArr[0];
    $prog = $gcArr[count($gcArr)-1];
//Q($gcArr);
//Q($prog);
//Q($classCode);

    if( $prog == "10" ) {
		return "Вальс,Танго,Венский вальс,Медленный фокстрот,Квикстеп,"
			."Ча-ча-ча,Самба,Румба,Пасодобль,Джайв";
	}
	else
    if( $prog == "st" ) {
    
      switch( $classCode ) {
        case 'n':
          return "Вальс,Квикстеп";
          
        case 'e':
          return "Вальс,Венский вальс,Квикстеп";
          
        case 'd':
          return "Вальс,Танго,Венский вальс,Квикстеп";
          
		default:
        case 'c':
        case 'ba':
        case 'juv':case 'juv1':case 'juv2':
        case 'jun':case 'jun1':case 'jun2':
        case 'you':case 'you1':case 'you2':
        case 'am':case 'youam':case 'sen':
          return "Вальс,Танго,Венский вальс,Медленный фокстрот,Квикстеп";
      }

    }
    else
    if( $prog == "la" ) {
    
      switch( $classCode ) {
        case 'n':
          return "Ча-ча-ча,Джайв";
          
        case 'e':
          return "Ча-ча-ча,Румба,Джайв";
          
        case 'd':
          return "Ча-ча-ча,Самба,Румба,Джайв";
          
		default:
        case 'c':
        case 'ba':
        case 'juv':case 'juv1':case 'juv2':
        case 'jun':case 'jun1':case 'jun2':
        case 'you':case 'you1':case 'you2':
        case 'am':case 'youam':case 'sen':
          return "Ча-ча-ча,Самба,Румба,Пасодобль,Джайв";
      }

    }
    else {
    
      switch( $classCode ) {
        case 'sn3':
        case 'n3':
          return "Вальс,Полька,Ча-ча-ча";
          
        case 'sn4':
        case 'n4':
          return "Вальс,Квикстеп,Ча-ча-ча,Джайв";
          
        case 'sn5':
        case 'n5':
          return "Вальс,Квикстеп,Ча-ча-ча,Румба,Джайв";
          
        case 'sn5s':
        case 'n5s':
          return "Вальс,Квикстеп,Ча-ча-ча,Самба,Джайв";
          
        case 'e':
          return "Вальс,Венский вальс,Квикстеп,Ча-ча-ча,Румба,Джайв";
          
        case 'es':
          return "Вальс,Венский вальс,Квикстеп,Ча-ча-ча,Самба,Джайв";
          
        case 'et':
          return "Вальс,Танго,Квикстеп,Ча-ча-ча,Румба,Джайв";
          
        case 'd':
          return "Вальс,Танго,Венский вальс,Квикстеп,Ча-ча-ча,Самба,Румба,Джайв";
		  
		case 'sc_p': return "Полька";
		case 'sc_w': return "Медленный вальс";
		case 'sc_q': return "Квикстеп";
		case 'sc_c': return "Ча-ча-ча";
		case 'sc_s': return "Самба";
		case 'sc_r': return "Румба";
		case 'sc_j': return "Джайв";
      }
      
    }

	switch( $groupCode ) {
		case 'juv1': 
		case 'juv.6': 
			return "Вальс,Венский вальс,Квикстеп,Ча-ча-ча,Румба,Джайв";
			
		case 'juv2': 
		case 'juv.8': 
			return "Вальс,Танго,Венский вальс,Квикстеп,Ча-ча-ча,Самба,Румба,Джайв";
	}
	
  }
  
  //===========

  public function makePath( ) {
    $script = $_SERVER['PHP_SELF'];
    return parent::makePath( "Соревнования", "db_events", $script, "title", "e@" );
  }

  //----------------

  public function getTableName() {
    return "db_events";
  }

  //----------------

  public function getFieldNames() {
    return array( "id", "pid", "position", "type", "regType", "groupCode", "ratingLevel", "round", 
                  "title", "caution", "status", "date", "date1", 
                  "afisha", "schedule", "prizes", "way", "idPlace", "placeLabel", "organizer", "teamFormat", 
                  "dances", "judges", "registration", "results", "ncouples", "regOpen", "haveNumbers", "haveScores", "havePlaces",
                  "idGallery", "hidden" );
  }

  //----------------

  public function killFromDB() {
    $id = $this->id;
    ASSERT_int($id);
    MotlDB::query( "DELETE FROM db_couple_event WHERE idEvent=$id" );  // ASS $id

    $this->killRowFromDB();
  }

  //----------------
}

//=====================
?>