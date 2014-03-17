<?php
require_once "bbtemplator.inc";
//=====================

class DataUtil {

  //================

  public function dateDbToHuman( $s ) {
    $dateTS = strtotime( $s );
    return $s ? date( "d.m.Y H:i", $dateTS ) : "";
  }

  //----------------

  public function dateDbToHumanNoTime( $s ) {
    $dateTS = strtotime( $s );
    return $s ? date( "d.m.Y", $dateTS ) : "";
  }

  //----------------

  public function dateHumanToDb( $s ) {

    $dateTS = DataUtil::parseHumanDate( $s );

    return $dateTS ? date( "Y-m-d H:i:s", $dateTS ) : null;
  }

  //----------------

  private function parseHumanDate( $dateHuman ) {
    $date1 = strtr( $dateHuman, ":/.,", "    " );
    $date1 = str_replace( "  ", " ", $date1 );
    $date1 = str_replace( "  ", " ", $date1 );
    $date1 = str_replace( "  ", " ", $date1 );
    $date1 = trim( $date1 );
    $dateA = explode( " ", $date1 );

    switch( count($dateA) ) {
      case 5: list($d,$m,$y,$h,$t) = $dateA; break;
      case 4: list($d,$m,$y,$h) = $dateA; $t=0; break;
      case 3: list($d,$m,$y) = $dateA; $h=0; $t=0; break;
      default: return null;
    }

    return mktime( $h,$t,0, $m,$d,$y );
  }

  //================
  
  public function formatDate( $format, $date ) { // "Y-m-d H:i:s"
	$dateTS = strtotime( $date );
	return date( $format, $dateTS );
  }
  
  //================

  function filterBB( $value ) {
    $templator = new BbTemplator();
    return $templator->process( $value );
  }

  //================
  
  public function numForm( $value, $w1, $w2, $w5 ) {

    $ivalue = (int)$value;

    if( $value != $ivalue ) {
      return $w1;
    }
    else {
      $ones = $ivalue % 10;
      $tens = ($ivalue-$ones) / 10;

      if( $tens == 1 ) {
        return $w5;
      }
      else {
        switch( $ones ) {
          case 1: return $w1;
					
          case 2:
          case 3:
          case 4: return $w2;

          case 5:
          case 6:
          case 7:
          case 8:
          case 9:
          case 0: return $w5;
        }
      }
    }
  }
  
  //================

	public function updateEventBase() {

	//--- Update 'competiotion's
	  MotlDB::query( "UPDATE db_events SET ncouples=(select count(*) from db_couple_event WHERE idEvent=db_events.id ) "
					."WHERE type='competition'" );

	  MotlDB::query( "UPDATE db_events SET haveNumbers=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and number is not null ) "
					."WHERE type='competition'" );
					
	  MotlDB::query( "UPDATE db_events SET havePlaces=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and place is not null ) "
					."WHERE type='competition'" );
					
	  MotlDB::query( "UPDATE db_events SET haveScores=exists (SELECT * FROM db_couple_event WHERE idEvent=db_events.id and ( d1 is not null or d2 is not null or d3 is not null or d4 is not null or d5 is not null ) ) "
					."WHERE type='competition'" );
					
	//--- Update 'event's

	  $numberIds = MotlDB::queryValues( "SELECT DISTINCT pid FROM db_events WHERE type='competition' AND haveNumbers" );
	  DataUtil::setFlagField( "event", "haveNumbers", $numberIds ); // SqlSafe

	  $placeIds = MotlDB::queryValues( "SELECT DISTINCT pid FROM db_events WHERE type='competition' AND havePlaces" );
	  DataUtil::setFlagField( "event", "havePlaces", $placeIds ); // SqlSafe

	  $scoreIds = MotlDB::queryValues( "SELECT DISTINCT pid FROM db_events WHERE type='competition' AND haveScores" );
	  DataUtil::setFlagField( "event", "haveScores", $scoreIds ); // SqlSafe
	}

  //================
  
	// SQLPARAM  $type, $field, $ids
	private function setFlagField( $type, $field, $ids ) {
	  if( count($ids) > 0 ) {
		$sids = "'" . implode( "', '", $ids ) . "'";
		
		// PAR $type, $field, $sids(ids)
		MotlDB::query( "UPDATE db_events SET $field=(id In($sids)) WHERE type='$type'" );  
	  }
	}

	//=====================
}
?>