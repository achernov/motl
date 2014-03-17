<?php
//======================================================================
//==  RatingPartitionSolo
//======================================================================

require_once "regTypeSolo.inc";

class RatingPartitionSolo extends RatingPartition {

  var $rows = array( 
                array( "10_13", "10-13" ),
                array( "_9",    "9 и мл." )
            );
            
  var $cols = array( 
                array( "sn3",  "Соло N3" ),
                array( "sn4",  "Соло N4" ),
                array( "sn5",  "Соло N5" ),
            );
              
  public function getCode() { return "solo"; }
  
  public function makeGroup( $colCode, $rowCode ) { return "$colCode.$rowCode"; }
  
  public function makeName( $colName, $rowName ) { return "$colName $rowName"; }
  
  public function listGroups() {
    $groups = array();
    foreach( $this->cols as $col ) {
      for( $k=count($this->rows)-1; $k>=0; $k-- ) {
        $group = $this->makeGroup( $col[0], $this->rows[$k][0] );
        $name  = $this->makeName( $col[1], $this->rows[$k][1] );
        $groups[] = array($group,$name);
      }
    }
    return $groups;
  }
  
  //================

  function makeRating( $group ) {
  
    //--- Load competitions
    
    $comps = $this->loadCompetitions( $group );
    
    $idcomps = $this->getCompIds( $comps );
    
    //--- Load participations
    
    $dancers = $this->loadParticipants( $idcomps );
    //print_r($dancers);die();
    //--- Calc rating
    
    
    $dancerRating = array();
    
    foreach( $dancers as $idDancer=>$dancer ) {
    
      $dancerRating[$idDancer] = $this->processDancer( $dancers[$idDancer] );
    }

    //--- Sort rating
    
    asort( $dancerRating );
    //print_r( $dancerRating );
    //--- Output rating
    
    $s = "";
    
    $s .= "<font color='#800000'>Цветом выделены</font> пары Москвы и Московской области<br>";
    
    $s .= $this->makeRatingText( $comps, $dancers, $dancerRating  );
    
    return $s;
  }
  
  //================
 
  function loadParticipants( $idcomps ) {
  
    $sidcomps = implode( ",", $idcomps );
    ASSERT_ints( $sidcomps );
    
    // ASS $sidcomps
    $sql = "SELECT idEvent,idSolo,place, CONCAT_WS(' ',surname,name) AS name, db_person.city "
          ."FROM db_couple_event INNER JOIN db_person ON db_couple_event.idSolo=db_person.id "
          ."WHERE idEvent In($sidcomps)";

    $rows = MotlDB::queryRows( $sql );  
//print_r($rows);

    $dancers = array();
    for( $k=0; $k<count($rows); $k++ ) {
      $idComp = $rows[$k]->idEvent;
      $idDancer = $rows[$k]->idSolo;
      $dancers[$idDancer]->comps[$idComp] = $rows[$k];
      $dancers[$idDancer]->name = $rows[$k]->name;
      $dancers[$idDancer]->city = $rows[$k]->city;
    }

    return $dancers;
  }
  
  //================

  function processDancer( &$dancer ) {
    $sumWV = 0;
    $sumW = 0;
    
    $diploms = $this->countDiploms($dancer);

    $numRating = 999999-(10000*$diploms[1]+100*$diploms[2]+$diploms[3]);
    $textRating = $numRating . $dancer->name;
    
    $dancer->rating = $textRating;
    
    return $dancer->rating;
  }
  
  //================
  
  function countDiploms( $dancer ) {
    $diploms = array( 1=>0, 2=>0, 3=>0 );
    
    foreach( $dancer->comps as $k=>$c ) {
      $score = $dancer->comps[$k]->place;
      $diplom = RegTypeHandlerSolo::calcDiplom( $score );
      if( isset($diploms[$diplom]) ) {
        $diploms[$diplom] ++;
      }
    }
    return $diploms;
  }
  
  //================
  
  function makeRatingText( $comps, $dancers, $dancerRating  ) {
    
    $debug = 0;
    
    $editmode = $debug ? 1 : Motl::getParameter("editmode",0);
    
    $highlight = Motl::getParameter("highlight",0);
  
    $s = "";
    
    $idcomps = $this->getCompIds( $comps );
    $ncomps = count($idcomps);
    
    $s .= "<table border=1 cellspacing=0>\n";
    
    $s .= "<tr style='font-size:12'>";
    //$s .= "<td rowspan='2' class='HeaderCell'><b>место</b></td>";
    if($debug)
      $s .= "<td rowspan='2' class='HeaderCell'><b>рейтинг</b></td>";
    $s .= "<td rowspan='2' class='HeaderCell'><b>Пара</b></td>";
    $s .= "<td colspan='3' class='HeaderCell'><b>Кол-во дипломов:</b></td>";
    $s .= "<td colspan='$ncomps' class='HeaderCell' align=center>Турниры<br>(<small>название показывается при наведении указателя мыши на дату</small>)</td>";
    $s .= "</tr>";
    $s .= "<tr style='font-size:12'>";
    $s .= "<td class='HeaderCell'><b>1 ст.</b></td>";
    $s .= "<td class='HeaderCell'><b>2 ст.</b></td>";
    $s .= "<td class='HeaderCell'><b>3 ст.</b></td>";
    
    for( $i=0; $i<count($idcomps); $i++ ) {
      $idComp = $idcomps[$i];
      $comp = $comps[$idComp];
      $date = DbProducer::makeText( $comp, "[plainDate|date]" );
      $s .= "<td class='HeaderCell' tyle='width:150' align=center><small>"
            ."<A title='$comp->eventName' href=db.php?id=e$comp->idEvent,r$comp->idComp,scores>"
            //."<b>$comp->eventName</b><hr>"
            //."$comp->compName<hr>"
            ."<nobr>$date</nobr></a></td>";
    }
    $s .= "</tr>";
    
    $k = 1;
    foreach( $dancerRating as $idDancer=>$rating ) {
      $rating = round( $rating, 2 );
      $dancer = $dancers[$idDancer];
      $dancerName = $dancer->name;

      $color   = $this->isMosObl($dancer) ? "#800000" : "#000000";

      $bg = ($idDancer==$highlight) ? "bgcolor=#ffa0a0" : "";
      
      $s .= "<tr $bg>";
      if($debug)
        $s .= "<td class='DataCell' style='color:$color'>$rating</td>";
      $s .= "<td class='DataCell' style='color:$color'><A style='color:$color' href='db.php?id=p$idDancer'>$dancerName</a></td>";

      $diploms = $this->countDiploms($dancer);
      $s .= "<td class='DataCell'>$diploms[1]</td>";
      $s .= "<td class='DataCell'>$diploms[2]</td>";
      $s .= "<td class='DataCell'>$diploms[3]</td>";
      
      for( $i=0; $i<count($idcomps); $i++ ) {
        $idComp = $idcomps[$i];
        //$comp = $comps[$idComp];
        $c = "";
        if( isset($dancer->comps[$idComp]) ) {
          $comp = $dancer->comps[$idComp];
          //if($idDancer==93)
          //print_r($comp);
          $diplom = RegTypeHandlerSolo::calcDiplom( $comp->place );
          
          $c = "<center><b> $diplom ст.</b> / <small><i>$comp->place</i><small></center>";
        }
        else {
          $c = "&nbsp";
        }
        $s .= "<td class='DataCell' style='color:$color'>$c</td>";
      }
      
      $s .= "</tr>\n";
      $k++;
    }
    $s .= "</table>\n";
    
    return $s;
  }
  
  //================
};

//=====================
?>
