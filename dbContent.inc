<?php
require_once "_core.inc";
require_once "_dbproducer.inc";

require_once "dbAll.inc";
//require_once "dbClub.inc";
//require_once "dbCity.inc";
//require_once "dbCountry.inc";
//require_once "dbHall.inc";
//require_once "dbPerson.inc";
//require_once "dbCouple.inc";
//require_once "dbEvent.inc";

//=====================

class DbContent extends Producer {

  public function produce( $format = null ) {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );

    $s = "";

    $aid = Motl::splitId($id);

    switch( $aid['c'] ) {
    
      case null:
        if( $id == 'f' ) {
          Motl::$Rubric = "reg";
          $s .= "<h1>Онлайн-регистрация</h1>"; 
          MotlLayout::$noRight = false;
          $ep = new DbEventProducer();
		  
		  $cond = "(pid Is Null OR pid=0) AND type='event' AND (regOpen OR registration<>'')";
		  if( ! Motl::getParameter( "editmode" ) ) {
			$cond .= " AND (hidden Is Null OR not hidden)";
		  }
		  
          $s .= $ep->produceMultiple( "shortRegForm", 
                    $cond,
                    "date ASC"/*,
					"Сейчас на сайте не идет регистрация на какой-либо турнир"*/ );
        }
        else
        if( $id == 'r' ) {
          Motl::$Rubric = "res";
          MotlLayout::$noRight = false;
          $s .= "<h1>Результаты соревнований</h1>"; 
          $s .= $this->produceResults();
        }
        break;
        
      case 'e': // Определенное событие
        $itemProducer = new DbEventProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;

      case 'c': // Определенная пара
        $itemProducer = new DbCoupleProducer();
        $itemProducer->loadFromDb( $id );
        if( ! $itemProducer->row ) { Motl::redirect( "base.php?id=couples" );  }
        $s .= $itemProducer->produceFull();
        break;

      case 'p': // Определенная персона
        $itemProducer = new DbPersonProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;

      case 'k': // Определенный клуб
        $itemProducer = new DbClubProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;

      case 'h': // Определенный зал
      //die("qqq");
        $itemProducer = new DbHallProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;

      case 'g': // Определенный город
        $itemProducer = new DbCityProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;

      case 's': // Определенная страна
        $itemProducer = new DbCountryProducer();
        $itemProducer->loadFromDb( $id );
        $s .= $itemProducer->produceFull();
        break;
    }

    return $s;
  }	
  
  //----------------
/*
SELECT distinct year(date)-(month(date)<7)
FROM db_events 
WHERE type='event' AND date Is Not Null AND (haveScores or havePlaces)
*/
  function produceResults() {
    $ep = new DbEventProducer();
    
    //----- Season selector

    $seasonParam = Motl::getIntParameter( "season" );
    
    $sql = "SELECT distinct year(date)-(month(date)<7) as season "
          ."FROM db_events "
          ."WHERE type='event' AND date Is Not Null AND (haveScores or havePlaces) "
          ."ORDER BY season";

    $seasons = MotlDB::queryValues( $sql );
    
    $items = array();
    
    $style='padding-right:20';
    
    $maxSeason = $seasons[count($seasons)-1];
    if( ! $seasonParam ) {
      $seasonParam = $maxSeason;
    }
    
    for( $k=count($seasons)-1; $k>=0; $k-- ) {
      $year0 = $seasons[$k];
      $year1 = $seasons[$k]+1;
      
      $label = "сезон $year0-$year1";
      if( $year0 == $seasonParam ) {
        $items[] = "<b style='$style'>$label</b>";
      }
      else {
        $items[] = "<A style='$style' href='db.php?id=r&season=$year0'>$label</a>";
      }
    }
    $selector = implode( "<span width=100>", $items );
    
    //----- Load events
    
    if( $seasonParam ) {
      // ASS $seasonParam
      $year0 = (int)$seasonParam;
      $year1 = (int)$seasonParam+1;
      $seasonCond = "((date>'$year0-07-15')AND(date<'$year1-07-15'))";
    }
    else {
      $seasonCond = "true";
    }
    
    $s = "";
    $s .= "$selector<p>";
    
    // COD $seasonCond
    $s .= $ep->produceMultiple( "shortResults", 
              "type='event' AND (havePlaces Or haveScores Or length(results)) AND $seasonCond" ); 
    return $s;
  }
  
  //----------------
}

//=====================

/*

SELECT db_events.id, exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and number is not null ) FROM db_events
UPDATE db_events SET haveNumbers=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and number is not null )
UPDATE db_events SET havePlaces=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and place is not null )
UPDATE db_events SET haveScores=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and ( d1 is not null or d2 is not null or d3 is not null or d4 is not null or d5 is not null ) )

*/
?>
