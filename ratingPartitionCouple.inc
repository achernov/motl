<?php
//======================================================================
//==  RatingPartitionCouple
//======================================================================

class RatingPartitionCouple extends RatingPartition {

  private $scoreMonthK = .52;//.6;//0.52;
  private $coupleMonthK = 0.87;//.95;

  //================

  function makeRating( $group, $specMode=null ) {
  
    //--- Load competitions
    
    $comps = $this->loadCompetitions( $group );
    
    $idcomps = $this->getCompIds( $comps );
    
    $minDaysGlobal = 1000;
    foreach( $comps as $comp ) {
      $minDaysGlobal = min( $minDaysGlobal, $comp->days );
    }
    
    //--- Load participations
    
    $couples = $this->loadParticipants( $idcomps );
    
    //--- Calc rating
    
    
    $coupleRating = array();
    
    foreach( $couples as $idCouple=>$couple ) {
    
      if( $couple->split ) {
        continue;
      }

      $coupleRating[$idCouple] = $this->processCouple( $couples[$idCouple], $comps, $minDaysGlobal, $specMode );
    }
   //print_r($couples);

    //--- Sort rating
    
    arsort( $coupleRating );
    
    //--- Output rating
    
    $s = "";
    
    $s .= "<font color='#800000'>Цветом выделены</font> пары Москвы и Московской области<br>";
    
    $s .= $this->makeRatingText( $comps, $couples, $coupleRating  );
    
    return $s;
  }
  
  //================
 
  function loadParticipants( $idcomps ) {
  
    $sidcomps = implode( ",", $idcomps );
    ASSERT_ints($sidcomps);
    
    // ASS $sidcomps
    $sql = "SELECT idEvent,idCouple,place,split "
          ."FROM db_couple_event INNER JOIN db_couples ON db_couple_event.idCouple=db_couples.id "
          ."WHERE idEvent In($sidcomps)";

    $rows = MotlDB::queryRows( $sql );  

    $couples = array();
    $idcouples = array();
    for( $k=0; $k<count($rows); $k++ ) {
      $idComp = $rows[$k]->idEvent;
      $idCouple = $rows[$k]->idCouple;
      $couples[$idCouple]->comps[$idComp] = $rows[$k];
      $couples[$idCouple]->split = $rows[$k]->split;
      $idcouples[] = $idCouple;
    }
    $sidcouples = implode( ",", $idcouples );

    // FLD $sidcouples
    $sql = "SELECT Couple.id as idCouple,Couple.city,Couple.country,"
              ."CONCAT_WS( ' - ', CONCAT_WS(' ',Man.surname,Man.name),CONCAT_WS('&nbsp;',Lady.surname,Lady.name) ) AS coupleName "
          ."FROM db_couples AS Couple "
              ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
              ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id " 
          ."WHERE Couple.id In($sidcouples)";
  
    $rows = MotlDB::queryRows( $sql );  
    for( $k=0; $k<count($rows); $k++ ) {
      $idCouple   = $rows[$k]->idCouple;
      $couples[$idCouple]->name = $rows[$k]->coupleName;
      $couples[$idCouple]->city = $rows[$k]->city;
    }
    
    return $couples;
  }
  
  //================

  private function specScore( $comp ) {
    $place = $comp->place;
    //print_r($comp->idEvent);die();
    if( $comp->idEvent>=431 && $comp->idEvent<=489 )   {
      switch( $place ) {
        case 1:  return 14;
        case 2:  return 10;
        case 3:  return 8;
        case 4:  return 7;
        case 5:  return 6;
        case 6:  return 5;
        case 7:  return 4;
        case 8:  return 3;
        case 9:  return 2;
        case 10:  return 1;
        default: return 0;
      }
    }
    else {
      switch( $place ) {
        case 1:  return 10;
        case 2:  return 6;
        case 3:  return 4;
        case 4:  return 3;
        case 5:  return 2;
        case 6:  return 1;
        default: return 0;
      }
    }
  }
  
  function processCouple( &$couple, $comps, $minDaysGlobal, $specMode ) {
    $sumWV = 0;
    $sumW = 0;
    
    $specSum = 0;
    
    $minDays = 1000;
    foreach( $couple->comps as $k=>$c ) {
      $place = $this->normPlace( $couple->comps[$k]->place );
      
      $specSum += $this->specScore( $couple->comps[$k] );
      
      $comp = $comps[$couple->comps[$k]->idEvent];
      $total = $comp->ncouples;
      $normal = ($total-$place)/($total-1);
      $couple->comps[$k]->score = $score = 100*$normal;
      $couple->comps[$k]->days = $days = $comp->days;
      $minDays = min( $minDays, $days );
      $Ct = pow( $this->scoreMonthK, ($days/30) );
      $couple->comps[$k]->Ct = $Ct;
      $sumWV += $Ct*$score;
      $sumW  += $Ct;
    }
    
    $couple->rawRating = ($sumWV/$sumW);
    $couple->coeff     = pow( $this->coupleMonthK, (($minDays-$minDaysGlobal)/30) );
    
    $rating = $couple->rawRating * $couple->coeff;
    
    if( $specMode ) {
      //$rating += $specSum;
      $rating = $specSum;
    }
    
    $couple->rating = $rating;
    
    
    return $rating;
  }
  
  //================

  function normPlace( $place ) {
    $p = explode( "-", $place );
    $v = (count($p)>1) ? (float)trim($p[1]) : (float)trim($p[0]);
    return $v;
  }

  //=================
  
  function makeRatingText( $comps, $couples, $coupleRating  ) {
    
    $highlight = Motl::getParameter("highlight",0);
    
    $s = "";
    
    $idcomps = $this->getCompIds( $comps );
    $ncomps = count($idcomps);
    
    $s .= "<table border=1 cellspacing=0>\n";
    
    $s .= "<tr style='font-size:12'>";
    $s .= "<td rowspan='2' class='HeaderCell'><b>место</b></td>";
    $s .= "<td rowspan='2' class='HeaderCell'><b>рейтинг</b></td>";
    $s .= "<td rowspan='2' class='HeaderCell'><b>пара</b></td>";
    $s .= "<td colspan='$ncomps' class='HeaderCell' align=center>Турниры<br>(<small>название показывается при наведении указателя мыши на дату</small>)</td>";
    $s .= "</tr>";
    $s .= "<tr style='font-size:12'>";
    
    for( $i=0; $i<count($idcomps); $i++ ) {
      $idComp = $idcomps[$i];
      $comp = $comps[$idComp];
      $date = DbProducer::makeText( $comp, "[plainDate|date]" );
      $s .= "<td class='HeaderCell' tyle='width:150' align=center><small>"
            ."<A title='$comp->eventName' href=db.php?id=e$comp->idEvent,r$comp->idComp>"
            //."<b>$comp->eventName</b><hr>"
            //."$comp->compName<hr>"
            ."<nobr>$date</nobr></a></td>";
    }
    $s .= "</tr>";
    
    $k = 1;
    foreach( $coupleRating as $idCouple=>$rating ) {
      $rating = round( $rating, 2 );
      $couple = $couples[$idCouple];
      $coupleName = $couple->name;
      $c = "";
      
      $c = "<b>$couple->rawRating * $couple->coeff</b><br>";

      $color   = $this->isMosObl($couple) ? "#800000" : "#000000";
      
      $bg = ($idCouple==$highlight) ? "bgcolor=#ffa0a0" : "";

      $s .= "<tr $bg>";
      $s .= "<td class='DataCell' style='color:$color'><u><b>$k</b></u></td>";
      $s .= "<td class='DataCell' style='color:$color'>$rating</td>";
      $s .= "<td class='DataCell' style='color:$color'><A name=row$idCouple></a><A style='color:$color' href='db.php?id=c$idCouple'>$coupleName</a></td>";
      //$s .= "<td>$c</td>";
      
      for( $i=0; $i<count($idcomps); $i++ ) {
        $idComp = $idcomps[$i];
        //$comp = $comps[$idComp];
        $c = "";
        if( isset($couple->comps[$idComp]) ) {
          $comp = $couple->comps[$idComp];
          //if($idCouple==93)
          //print_r($comp);
          $score = round( $comp->score*$comp->Ct, 1 );
          $score = round( $comp->score, 1 );
          $c = "<center><b>$comp->place</b> / <small><i>$score</i><small></center>";
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
  
  //=================
}

//=====================
?>
