<?php
//=====================
require_once "_dbproducer.inc";
require_once "baseColumn.inc";
//=====================

define( "PROTOCOL_TARUDKO",  1 );
define( "PROTOCOL_ALEKSEEV", 2 );

define( "PROTOCOL_TYPE", PROTOCOL_TARUDKO );
//define( "PROTOCOL_TYPE", PROTOCOL_ALEKSEEV );

//=====================

define( "MODE_REGLIST",  1 );
define( "MODE_RESULT_COMMON",  2 );
define( "MODE_RESULT_SCORES",  3 );

//=====================

function placeCmp( $aRow, $bRow ) {
  $placeA = clearPlace( $aRow->place );
  $placeB = clearPlace( $bRow->place );

  if( $placeA != $placeB ) return $placeA - $placeB;
}

function clearPlace( $v ) {
  
  $v = trim( $v );
  
  //----- Если разделенное место (10-12), берем первое число
  $v = explode( "-", $v );
  $v = $v[0];
  $v = trim( $v );
  
  $n = (int)$v; //---- Чтобы сортировать по численному значению
  
  if( ! $n  ) $n = 99999; //---- не число, будет после чисел. (например, 'д' - дисквал.)
  
  return $n;
}

//=====================

function crossCmp( $aRow, $bRow ) {
  if( $bRow->crosses != $aRow->crosses ) return $bRow->crosses - $aRow->crosses;
  return $aRow->number - $bRow->number;
}

//=====================

class RegTypeHandler {

  public function codeIsMine( $code, $operation=null ) { // returns bool
  }
  
  //===========
  
  public function getCanonicalCode() {
  }
  
  //===========
  
  public function getLabel() {
  }
  
  //===========
  
  public function makeForm( $idEvent ) {
    return "";
  }
  
  //===========

  public function checksVisible( $checksText ) {
	return is_int(strpos($checksText,"type='checkbox'"));
  }
  
  //===========

  protected function parsePerson( $text ) {
  
	mb_regex_encoding("utf-8");
	$pattern = "[^\\w]+";
	$text = mb_ereg_replace ( $pattern, " ", $text );

    $text = trim( $text );
	
    $ar = explode( " ", $text, 2 );

    return isset($ar[1]) ? array($ar[0],$ar[1]) : array($ar[0],null);
  }
  
  //===========
    
  public function makeRegList( $compProducer ) {
  }
  
  //===========
  
  private function loadDancersTable( $compProducer, $mode ) {
  }
  
  //===========
  
  private function makeDancersTable( $compProducer, $rows, $mode, $editmode ) {
  }
  
  //===========

  protected function deleteSelected( $compProducer ) {
    //print_r( $_REQUEST );die();

    $ids     = $_REQUEST['dbf_id'];
    
    $selids = array();

    for( $k=0; $k<count($ids); $k++ ) {
      if( isset($_REQUEST["dbf_".$k."_check"]) ) {
        $selids[] = $ids[$k];
      }
    }

    $sids = implode( ",", $selids );
    
    ASSERT_ints($sids);

    if( strlen($sids) ) {
      MotlDB::query( "DELETE FROM db_couple_event WHERE id In($sids)" );  // ASS $sids
    }
    
    DataUtil::updateEventBase();
    
    Motl::redirectToRetUrl();
  }
  
  //===========
  
  public function makeJudges( $compProducer ) {
    $judges = $this->loadJudges( $compProducer );
//    $judges = $this->linkJudges( $judges );
    $out = implode( ", ", $judges );
    return $out;
  }
  
  //-----------
  
  private function linkJudges( $judges ) {
    $sqlJudges = implodeStringsWithMRES( $judges );
    
    // COD $sqlJudges
    $sql = "SELECT id, concat_ws(' ',surname,name) as title "
          ."FROM db_person WHERE concat_ws(' ',surname,name) In($sqlJudges)";
    
    $rows = MotlDB::queryRows( $sql );  
    
    $table = array();
    foreach( $rows as $row ) {
      $table[$row->title] = $row->id;
    }
    
    for( $k=0; $k<count($judges); $k++ ) {
      $title = $judges[$k];
      if( isset($table[$title]) ) {
        $id = $table[$title];
        $judges[$k] = "<A href='db.php?id=p$id'>$title</a>";
      }
    }
    
    //die($sql);
    return $judges;
  }
  
  //-----------
  
  public function loadJudges( $compProducer ) {
    $judgesText = $compProducer->row->judges;
    if( $compProducer->row->type == "round" ) {
      $pid = (int) $compProducer->row->pid;
      $compJudges = MotlDB::queryValue( "SELECT judges FROM db_events WHERE id='$pid'", null );  // COD $pid
      if($compJudges) {
        $judgesText = $compJudges;
      }
    }
    
    $aj = explode( "\n", $judgesText );
    $out = array();
    foreach( $aj as $item ) {
      $item = trim($item);
      if( strlen($item) ) {
        $out[] = $item;
      }
    }
    return $out;
  }
  
  //===========
 
  function makeScoreTipJS( $regIds, $judges, $nDances ) {
  
	$sids   = implode( ",", $regIds );
	$sjudges = "'" . implode( "','", $judges ) . "'";

	$s = "";
	$s .= "<script>\n";
//	$s .= "var regIds = Array($sids)\n";
	$s .= "var regIds = [$sids]\n";
	$s .= "var judges = Array($sjudges)\n";
	$s .= "setScoreSpans(regIds,$nDances)\n";
	$s .= "</script>\n";
	return $s;
  }
  
  //=========== Utils For Fill
  
  //-----------
  
  protected function makeFillArea( $compProducer, $name, $label, $width, $height=700 ) {
    
    $s = "";
    $s .= "<td><center><b>$label</b><br><textarea style='width:$width; height:$height;' name=$name></textarea></td>\n";
    return $s;
  }
  
  //-----------

  protected function arrayItem( $array, $index ) {
    return isset($array[$index]) ? $array[$index] : "";
  }
  
  //-----------
  
  protected function arraysDataHeight( $arrays ) {
    $height = 0;
    for( $k=0; $k<count($arrays); $k++ ) {
      $col = $arrays[$k];
      for( $i=count($col)-1; $i>=0; $i-- ) {
        if( trim($col[$i]) != "" ) {
          $height = max( $height, $i+1 );
          break;
        }
      }
    }
    return $height;
  }
  
  //-----------
  
  protected function getArrayParameter( $name, $trim=true ) {
    $v = Motl::getParameter( $name );
    $a = explode( "\n", $v );
    if( $trim ) {
      for( $k=0; $k<count($a); $k++ ) {
        $a[$k] = trim( $a[$k] );
      }
    }
    return $a;
  }
  
  //-----------
  
  private function trimEndNL( $s ) {
  //die( "-". (substr($s,-1)=="\r"||substr($s,-1)=="\n") ."-");
	while( strlen($s) && (substr($s,-1)=="\r"||substr($s,-1)=="\n") ) {
		//echo "($s.<br>";
		$s = substr($s,0,-1);
		//echo ".$s)<br>";
	}
	return $s;
  }
  
  //-----------
  
	protected function expandDances( $ndances ) {
	
		$dx = Motl::getParameter("dx");
		$lines = $this->getArrayParameter( "dx", false );

		$a0 = explode( "\t", $lines[0] );
		$njudges = count($a0) / $ndances;
		
		$dances = array();
		for( $d=0; $d<$ndances; $d++ ) {
			$dances[$d] = "";
		}
		
		for( $k=0; $k<count($lines); $k++ ) {
			$line = $this->trimEndNL( $lines[$k] ); // trim - критически важно!!!!!
			$a = explode( "\t", $line );
			//print_r($a);
			$c = array_chunk( $a, $njudges );
			//print_r($c);
			for( $d=0; $d<$ndances; $d++ ) {
				if( count($c) < $ndances ) {
					$dances[$d] .= "\r\n";
				}
				else {
					$dances[$d] .= implode( "\t", $c[$d] ) . "\r\n";
				}
			}
		}
		
		for( $d=0; $d<$ndances; $d++ ) {
			$d1 = $d+1;
			$this->setParameter( "d$d1", $dances[$d] );
		}
		
		//echo "<pre>";print_r( $_REQUEST );die();
	}
	
  //-----------
  
	protected function setParameter( $name, $value ) {
		$_REQUEST[$name] = $value;
	}
  
  //-----------
  
  protected function formatScores( $round, $scores ) {
//    $scores = str_replace( "\r", "", $scores );
//    $scores = str_replace( "\n", "", $scores );
    
    $ascores = explode( "\t", $scores );
    for( $k=0; $k<count($ascores); $k++ ) {
      $score = trim( $ascores[$k] );
      if( $score=="" || $score=="0" || $score=="-" ) {
        $score = "-";
      }
      else {
        if( $round ) { // if not final
          $score = "x";
        }
      }
      $ascores[$k] = $score;
    }
    return implode( "", $ascores );
  }
  
  //===========
  
	public function substCodedCCCT( $rows ) {
	    $colCountry = BaseColumn::createColumn( "country" );
	    $colCity    = BaseColumn::createColumn( "city" );
	    $colClub    = BaseColumn::createColumn( "club" );
		$colBoss    = BaseColumn::createColumn( "boss" );
	    $colTeacher = BaseColumn::createColumn( "teacher" );
		
		$cids = array();
		
		// Select Clubs
		foreach( $rows as $k=>$row ) {
			$club = $row->club;
			if( $club ) {
				$aclub = explode( ",", $club );
				foreach( $aclub as $i=>$c ) {
					$cids[] = $c;
				}
			}
		}
		$scids = implode( ",", $cids );
		$clubRows = MotlDB::queryRowsIndexed( "id", 
				 			"SELECT id,boss FROM db_club WHERE id In($scids)" );
							//die("SELECT id,boss FROM db_club WHERE id In($scids)");
		//print_r($clubRows);
		
		foreach( $rows as $k=>$row ) {
			
			$club = $row->club;
			$bids = array();
			if( $club ) {
				$aclub = explode( ",", $club );
				foreach( $aclub as $i=>$c ) {
					if( isset($clubRows[$c]) ) {
						$cr = $clubRows[$c];
						if( $cr->boss ) {
							$bids[] = $cr->boss;
						}
					}
				}
				$row->boss = implode( ",", $bids );
				//print_r($row);
			}
			
			RegTypeHandler::substCodedCcctOne( $row, $colCountry );
			RegTypeHandler::substCodedCcctOne( $row, $colCity );
			RegTypeHandler::substCodedCcctOne( $row, $colClub );
			$row->raw_boss = '';
			RegTypeHandler::substCodedCcctOne( $row, $colBoss );
			RegTypeHandler::substCodedCcctOne( $row, $colTeacher );
		}
		
	}
	
  //-----------
  
	public function substCodedCcctOne( $row, $column ) {
		$field = $column->getName();
		$rawField = $column->getRawFieldName();
		if( isset($row->$field) && $row->$field ) {
			$row->$rawField = $column->makeListImploded( explode(",",$row->$field), "</nobr>, <nobr>" );
		}
	}
  
  //===========
}

//=====================

?>
