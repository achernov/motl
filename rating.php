<?php
require_once "_core.inc";
require_once "_layout.inc";
require_once "tabs.inc";
require_once "ratingPartition.inc";
//=====================

MotlDB::connect();
Motl::$Rubric = "rating";

//=====================

class RatingProducer extends Producer {

  private $scoreMonthK = .52;//.6;//0.52;
  private $coupleMonthK = 0.87;//.95;

  //=================

  public function produce() {
  
    $s = "";

    //------- Select Partition
    
    $partition = $this->getPartition();
	
    $s .= "<h1>" . $partition->getTitle() . "</h1>";
	
	Motl::$Rubric = "rating".$partition->getCode();
   
    //------- End Select Partition
    
	//-------------------- TOPS
	
	$showTops = Motl::getParameter( "showTops", 0 );
	
	if( $showTops ) {
		$s .= "<style>h2{font-weight:bold;font-size:12pt;}
		td.cellHdr{font-weight:bold;padding-top:20px;padding-bottom:5px;}
		td.cellNum{padding-right:10px;}</style>";

		$s .= $partition->produceAllTops();
		die($s);
	}
	//return $s;
    //-------------------- End TOPS
	
    //------- End Make Tab Selector
/*
    $howTo = "<A href='page.php?id=ratingHow'>как расчитывается рейтинг?</a>";
    
    $s .= "<table border=0><tr><td valign=bottom >$tabSelector</td>"
          ."<td width=50></td>"
          ."<td valign=bottom class=dataCell tyle='padding-top:20'>$howTo</td></tr></table>"
          ."<p>";
  */  
    //------- Make Group Selector
    
    $s .= $partition->makeGroupMatrix();
	
	$partCode = $partition->getCode();
	$topsUrl = "rating.php?part=$partCode&showTops=1";
	$topsStyle = "style='font-family:arial;font-size:12;color:#808080'";
	$s .= "<A $topsStyle href='$topsUrl'>Показать все призовые места</a>";
	
	//print_r($partition);
    
    //------- End Make Group Selector
    
    $s .= "<P>&nbsp;<br>";
    
	$ratingText = $partition->makeRating();
	
    if( $ratingText != null ) {
      $s .= $ratingText;
    }
    else {
      $s .= MotlDB::queryValue( "SELECT pageText FROM staticpages WHERE id='aboutRating'" );
    }
  
    return $s;
  }

  //=================
  
  function getPartition() {
  
	$groupCode = Motl::getParameter( "ratingGroup" );
  
    $partitionClasses = new RatingPartitionClasses( $groupCode );
    $partitionAges = new RatingPartitionAges( $groupCode );
    //$partitionSolo = new RatingPartitionSolo( $groupCode );
    
    if( $groupCode ) {
      if( $partitionClasses->testGroup($groupCode) ) {
        return $partitionClasses;
      }
      else
      if( $partitionAges->testGroup($groupCode) ) {
        return $partitionAges;
      }
      /*else
      if( $partitionSolo->testGroup($groupCode) ) {
        return $partitionSolo;
      }*/
    }
    
	switch( Motl::getParameter("part") ) {
		case 'rating':  return $partitionAges; break;
		case 'class':   return $partitionClasses; break;
		//case 'solo':    return $partitionSolo; break;
	}
    
    return $partitionAges;
  }
  
  //=================
}

//=====================

$content = new RatingProducer();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
