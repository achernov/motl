<?php
require_once "_dbproducer.inc";
require_once "dbEvent.inc";
//=====================

MotlDB::connect();

$event = new DbEventProducer();
$event->loadFromDB( Motl::getParameter( "id" ) );

//print_r($event);die();
//=====================

header( "Content-Type: text/xml; charset=utf-8" );
$writer = new XMLWriter();
$writer->openMemory();

$writer->startDocument( "1.0", "UTF-8" );
$writer->startElement( "OpenDance" );
writeEvent( $writer, $event );
$writer->endElement();
$writer->endDocument();

//echo "<textarea readonly cols=100 rows=40>";
echo $writer->outputMemory();
//echo "</textarea>";
//=====================

function writeEvent( $writer, $event ) {
  $idEvent = $event->row->id;
  ASSERT_int($idEvent);
  
  // ASS $idEvent
  $compRows = MotlDB::queryRows( "SELECT * FROM db_events "
                                ."WHERE pid='$idEvent' And type='competition' "
                                ."ORDER BY position" );  
                                
//print_r($compRows);die();
  $writer->startElement( "event" );
  $writer->writeAttribute ( "id", $idEvent );
  $writer->writeAttribute ( "name", $event->row->title );

  $writer->startElement( "catList" );
  for( $k=0; $k<count($compRows); $k++ ) {
    $cr = $compRows[$k];
    $writer->startElement( "category" );
    $writer->writeAttribute ( "id", makeCompId( $cr ) );
    $writer->writeAttribute ( "name", $cr->title );
    $writer->endElement();
  }
  $writer->endElement();

  for( $k=0; $k<count($compRows); $k++ ) {
//  for( $k=0; $k<1; $k++ ) {
    writeComp( $writer, $compRows[$k] );
  }
  
  $writer->endElement();
}

//=====================

function makeCompId( $compRow ) {
  return $compRow->id . "_" . $compRow->groupCode;
}

//=====================

function writeComp( $writer, $compRow ) {
  $idComp = $compRow->id;
  ASSERT_int($idComp);
  
  // ASS $idComp
  $cplRows = MotlDB::queryRows( "SELECT dbce.*,dbc.idMan,dbc.idLady FROM db_couple_event AS dbce "
              ."INNER JOIN db_couples AS dbc ON dbce.idCouple=dbc.id "
              ."WHERE idEvent='$idComp' ORDER BY place" ); 
              
  $writer->startElement( "category" );
  $writer->writeAttribute ( "id", makeCompId( $compRow ) );
  $writer->writeAttribute ( "name", $compRow->title );
  
  writeCouples( $writer, $cplRows );
  
  writePlaces( $writer, $cplRows );
  
  $rndRows = MotlDB::queryRows( "SELECT * FROM db_events WHERE pid='$idComp' ORDER BY round DESC" );  // ASS $idComp
  writeRounds( $writer, $rndRows); // SqlSafe
  
  $writer->endElement();
}

//=====================

function writeCouples( $writer, $cplRows ) {
  $writer->startElement( "couples" );
  
  for( $k=0; $k<count($cplRows); $k++ ) {
    writeCouple( $writer, $cplRows[$k] );
  }
  
  $writer->endElement();
}

//=====================

function writeCouple( $writer, $cplRow ) {
//print_r($cplRow);die();
  $writer->startElement( "couple" );
  $writer->writeAttribute ( "id", $cplRow->idCouple );
  $writer->writeAttribute ( "number", $cplRow->number );
  $writer->writeAttribute ( "class", $cplRow->class );
  writePerson( $writer, "man", $cplRow->idMan ); // SqlSafe
  writePerson( $writer, "lady", $cplRow->idLady ); // SqlSafe
  $writer->endElement();
}

//=====================
// SQLPARAM $idPerson

function writePerson( $writer, $elName, $idPerson ) {
 
  $row = MotlDB::queryRow( "SELECT * FROM db_person WHERE id='$idPerson'" );  // PAR $idPerson
//  print_r($row);die();

  $writer->startElement( $elName );
  $writer->writeAttribute ( "id", $row->id );
  $writer->writeAttribute ( "surname", $row->surname );
  $writer->writeAttribute ( "name", $row->name );
  $writer->writeAttribute ( "birth", $row->birthDate );
  $writer->endElement();
}

//=====================

function writePlaces( $writer, $cplRows ) {
  $writer->startElement( "places" );
  
  for( $k=0; $k<count($cplRows); $k++ ) {
    $writer->startElement( "place" );
    $writer->writeAttribute ( "number", $cplRows[$k]->number );
    $writer->writeAttribute ( "place",  $cplRows[$k]->place );
    $writer->endElement();
  }
  
  $writer->endElement();
}

//=====================

function writeRounds( $writer, $rndRows ) {
  $writer->startElement( "rounds" );
  
  for( $k=0; $k<count($rndRows); $k++ ) {
    writeRound( $writer, $rndRows[$k] );
  }
  
  $writer->endElement();
}

//=====================
// SQLPARAM $rndRow

function writeRound( $writer, $rndRow ) {
  $idRnd = $rndRow->id;
  ASSERT_int($idRnd);
  
  // ASS $idRnd
  $cplRows = MotlDB::queryRows( "SELECT dbce.*,dbc.idMan,dbc.idLady FROM db_couple_event AS dbce "
              ."INNER JOIN db_couples AS dbc ON dbce.idCouple=dbc.id "
              ."WHERE idEvent='$idRnd' ORDER BY place" );  

  $writer->startElement( "round" );
  $writer->writeAttribute ( "level", $rndRow->round );
  $writer->writeAttribute ( "name",  $rndRow->title );

  $judges = getJudges( $rndRow ); // PAR $rndRow
  writeJudges($writer,$judges);
  
  writeRoundCouples( $writer, $cplRows );
  
  $writer->endElement();
}

//=====================

function writeJudges( $writer, $judges ) {
  $writer->startElement( "judges" );
  
  for( $k=0; $k<count($judges); $k++ ) {
    $judge = $judges[$k];
    $num = $k+1;
    $ajudge = explode( " ", $judge );
    if( count($ajudge) > 2 ) {
    //print_r($ajudge);
      $surname = $ajudge[1];
      $name = $ajudge[2];
    }
    else {
      $surname = $ajudge[0];
      $name = $ajudge[1];
    }
    //print_r($ajudge); die();
    $writer->startElement( "judge" );
    $writer->writeAttribute ( "number", $num );
    $writer->writeAttribute ( "surname",  $surname );
    $writer->writeAttribute ( "name",  $name );
    
    $writer->endElement();
  }
  
  $writer->endElement();
}

//=====================
// SQLPARAM $rndRow

function getJudges( $rndRow ) {

    $judgesText = $rndRow->judges;
    if( ! $judgesText ) {
      if( $rndRow->type == "round" ) {
        $pid = $rndRow->pid;
        $compJudges = MotlDB::queryValue( "SELECT judges FROM db_events WHERE id='$pid'", null );  // PAR $pid
        if($compJudges) {
          $judgesText = $compJudges;
        }
      }
    }
    
    $aj = explode( "\n", $judgesText );
    $out = array();
    foreach( $aj as $item ) {
      $item = trim($item);
      $item = str_replace( "\t", " ", $item );
      $item = str_replace( "  ", " ", $item );
      $item = str_replace( "  ", " ", $item );
      if( strlen($item) ) {
        $out[] = $item;
      }
    }
    return $out;
}

//=====================

function writeRoundCouples( $writer, $cplRows ) {
  $nums = array();
  for( $k=0; $k<count($cplRows); $k++ ) {
    $nums[] = $cplRows[$k]->number;
  }
  $snums = implode( ",", $nums );
  
  $writer->startElement( "couples" );
  $writer->writeAttribute ( "numbers",  $snums );
  $writer->endElement();
}

//=====================
?>