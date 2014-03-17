<?php
//===============================================================
require_once "baseColumn.inc";
//===============================================================

class DbThingProducer extends DbProducer {
  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    switch( $format ) {
      case "full":
      default:
        return $this->produceFull();

      case "edit":
		return $this->makeEditForm();
    }
  }

  //----------------
  
  public function loadFromString( $fromString ) {
    $this->row->name = $fromString;
  }
  
  //----------------
  
  protected function makeAttr( $label, $col ) {
    if( $this->row->$col ) {
		$column = BaseColumn::createColumn( $col );
		$arr = explode( ",", $this->row->$col );
		$value = $column->makeListImploded( $arr, ", " );
		return "<b>$label:</b> $value<p>";
	}
	return null;
  }
  
}

//===============================================================

class DbClubProducer extends DbThingProducer {
  //----------------

  public function produceFull() {
	require_once "dbClub.inc";
	return DbClubProducerFull::produceFull();
  }

  //================
  
  public function getTableName() {
    return "db_club";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"name"	=>	"[text|name]",
		"disambig"	=>	"[text|disambig]",

		"country"	=>	"[text|country]",
		"city"	=>	"[text|city]",
		"boss"	=>	"[text|boss]",
		"teacher"	=>	"[text|teacher]",

		"motl"	=>	"[text|motl]"
	);
  }

  //----------------
}

//===============================================================

class DbCityProducer extends DbThingProducer {

  public function produceFull() {
	require_once "dbCity.inc";
	return DbCityProducerFull::produceFull();
  }

  //----------------
  
  public function getTableName() {
    return "db_city";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"name"	=>	"[text|name]",
		"disambig"	=>	"[text|disambig]",
		"motl"	=>	"[text|motl]"
	);
  }

  //----------------
}

//===============================================================

class DbCountryProducer extends DbThingProducer {
  //----------------

  public function produceFull() {
	require_once "dbCountry.inc";
	return DbCountryProducerFull::produceFull();
  }

  //----------------

  public function getTableName() {
    return "db_country";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"name"	=>	"[text|name]",
		"motl"	=>	"[text|motl]"
	);
  }

  //----------------
}

//===============================================================

class DbHallProducer extends DbThingProducer {
  //----------------

  public function produceFull() {
	require_once "dbHall.inc";
	return DbHallProducerFull::produceFull();
  }

  //----------------

  public function getTableName() {
    return "db_halls";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"name"	=>	"[text|name|style='width:200']",
		"title"	=>	"[text|title|style='width:400']",
		"info"	=>	"[textarea|info|style='width:800;height:300']"
	);
  }

  //----------------
}

//===============================================================

class DbPersonProducer extends DbThingProducer {
  //----------------
  
  public function loadFromString( $fromString ) {
    $ar = explode( " ", $fromString );
    if( isset($ar[0]) && $ar[0] ) {  $this->row->surname = $ar[0];  }
    if( isset($ar[1]) && $ar[1] ) {  $this->row->name = $ar[1];  }
  }
  
  //----------------

  public function produceFull() {
	require_once "dbPerson.inc";
	return DbPersonProducerFull::produceFull();
  }

  //----------------

  public function getTableName() {
    return "db_person";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"surname"	=>	"[text|surname]",
		"name"	=>	"[text|name]",
		"sex"	=>	"[select|sex|M;F]",
		"birthDate"	=>	"[text|birthDate]",
		"disambig"	=>	"[text|disambig]",
		"isTeacher"	=>	"[text|isTeacher]",
		"isJudge"	=>	"[text|isJudge]",

		"country"	=>	"[text|country]",
		"city"	=>	"[text|city]",
		"club"	=>	"[text|club]",
		"teacher"	=>	"[text|teacher]",

		"motl"	=>	"[text|motl]",
		"motl_teacher"	=>	"[text|motl_teacher]"
	);
  }

  //----------------
}

//===============================================================

class DbCoupleProducer extends DbThingProducer {
  //----------------

  public function produceFull() {
	require_once "dbCouple.inc";
	return DbCoupleProducerFull::produceFull();
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

//===============================================================

class DbEventProducer extends DbThingProducer {
  //----------------

  public function produce( $format=null ) {
	require_once "dbEvent.inc";
	$full = new DbEventProducerFull($this->row);
	$full->id = $this->id;
	return $full->produce($format);
  }

  //----------------

  public function produceFull() {
	require_once "dbEvent.inc";
	$full = new DbEventProducerFull($this->row);
	$full->id = $this->id;
	$full->aid = $this->aid;
	return $full->produceFull();
  }

  //----------------

  public function getFields( $full = false ) {
  
	if( $full ) {
		require_once "dbEvent.inc";
		$hallOptions = DbEventProducerFull::makeHallOptions();
	}
	else {
		$hallOptions = "";
	}
	
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
  
  public function makeResultValue( $ceRow ) {
	require_once "regManager.inc";
    $handler = RegManager::getHandlerForCode( $this->row->regType, "results" );
    return $handler->makeResultValue( $this->row, $ceRow );
  }
  
  //----------------
  
	public function processEditCommand_medit() {
		$id = $this->row->id;
		$returl = Motl::makeRetUrl();
		return "<A class='editLink' href='db.php?id=e$id,e&returl=$returl' >Multiedit</a>";
	}
	
  //----------------
}

//===============================================================

class DbCoupleEventProducer extends DbThingProducer {
  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"idEvent"	=>	"[text|idEvent]",
		"idCouple"	=>	"[text|idCouple]",
		"idSolo"	=>	"[text|idSolo]",
		"number"	=>	"[text|number]",
		"mbirthdate"	=>	"[text|mbirthdate]",
		"fbirthdate"	=>	"[text|fbirthdate]",
		"class"	=>	"[text|class]",
		"raw_country"	=>	"[text|raw_country]",
		"raw_city"	=>	"[text|raw_city]",
		"raw_club"	=>	"[text|raw_club]",
		"raw_boss"	=>	"[text|raw_boss]",
		"raw_teacher"	=>	"[text|raw_teacher]",
		"country"	=>	"[text|country]",
		"city"	=>	"[text|city]",
		"club"	=>	"[text|club]",
		"boss"	=>	"[text|boss]",
		"teacher"	=>	"[text|teacher]",
		"place"	=>	"[text|place]",
		"d1"	=>	"[text|d1]",
		"d2"	=>	"[text|d2]",
		"d3"	=>	"[text|d3]",
		"d4"	=>	"[text|d4]",
		"d5"	=>	"[text|d5]",
		"d6"	=>	"[text|d6]",
		"d7"	=>	"[text|d7]",
		"d8"	=>	"[text|d8]",
		"d9"	=>	"[text|d9]",
		"da"	=>	"[text|da]",
		"pointsSt"	=>	"[text|pointsSt]",
		"pointsLa"	=>	"[text|pointsLa]"
	);
  }
  
  //----------------

  public function getTableName() {
    return "db_couple_event";
  }

  //----------------
}

//===============================================================
//===============================================================
?>
