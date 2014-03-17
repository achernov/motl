<?php
//=====================

class BaseColumn {

  private $baseItems = null;
  private $indexes = array();
  
  //----------------
  
  public static function createColumn( $fieldName ) {
    switch( $fieldName ) {
      case 'country': return new BaseColumnCountry();
      case 'city':    return new BaseColumnCity();
      case 'club':    return new BaseColumnClub();
      case 'boss':    return new BaseColumnBoss();
      case 'teacher': return new BaseColumnTeacher();
    }
    return null;
  }
  
  //----------------
  
	public function getRawFieldName() {
		return "raw_" . $this->getName();
	}
	
  //----------------
  
	public function getTextFieldName() {
		return $this->getHtmlFieldName()."0";
	}
	
  //----------------
  
	public function getHtmlFieldName() {
		return "text" . ucfirst( $this->getName() );
	}
	
  //----------------
  
  public function makeListImploded( $ids, $separator, $sorted=false ) {
  
    $items = $sorted ? $this->getItemListBySorted( "id", $ids )
					 : $this->getItemListBy( "id", $ids );
    
    $out = array();
    
    foreach( $items as $item ) {
      $code = $this->getIdCode();
      $disambig = isset($item->disambig) ? "</nobr> <nobr><i>(".$item->disambig.")</i>" : "";
      $sitem = "<A href='db.php?id=$code$item->id'>$item->name$disambig</a>";
	  
		if( getLocalSetting('DISABLE_BASE_LINK') ) {
			$sitem = "$item->name$disambig";
			$sitem = "$item->name";
		}
	  
      $out[] = $sitem;
    }
    
    return implode( $separator, $out );
  }
  
  //----------------
  
  public function sidsToSvalues( $sids, $separator=", " ) {
  
    $items = $this->getItemListBy( "id", explode(",",$sids) );
    
    return $this->extractAtrImploded( $items, "name", $separator );
    //return implode( $separator, $out );
  }
  
  //----------------
  
  public function getBaseItems() {
    if( ! $this->baseItems ) {
      $this->baseItems = $this->loadItemsMiddle();
      
      for( $k=0; $k<count($this->baseItems); $k++ ) {
        $this->baseItems[$k]->ThePosition = $k;
      }
    }
    return $this->baseItems;
  }
  
  //----------------
  
  public function getSimilarItems( $fieldValue ) {
    //--- Load All
    
    $rows = $this->loadSimilar( $fieldValue );
    
    //--- Select Final
    
    $maxFinal = 6;
    $nFinal = $maxFinal;
    if( count($rows) <= $maxFinal ) {
      $nFinal = count($rows);
    }
    else {
      $finScore = $rows[$maxFinal-1]->score;
      for( $k=$maxFinal; $k<count($rows); $k++ ) {
        if( $rows[$k]->score < $finScore ) {
          $nFinal = $k-1;
          break;
        }
      }
    }
    
    //--- Make Final
    
    $out = array();
    for( $k=0; $k<$nFinal; $k++ ) {
      $out[] = $rows[$k];
    }
    
    //---
   
    return $out;
  }
  
  //----------------

  public function loadSimilar( $fieldValue ) {
	$simSql = $this->makeSimilarity( "name", $fieldValue );
	$fieldName = $this->getName();

	if( $fieldName=='city' || $fieldName=='club' ) {
		$disambig = "disambig";
	}
	else {
		$disambig = "'' AS disambig";
	}

	 // $simSql,$fieldName - safe, COD $disambig
	$sql = "SELECT id, name as title, $disambig, $simSql as score FROM db_$fieldName "
		."WHERE name Is Not Null AND name<>'' ORDER BY score DESC";

	return MotlDB::queryRows( $sql );
  }

  //----------------

  function makeSimilarity( $field, $s ) {
    $a = array();
	
	$len = mb_strlen($s,"utf-8");
	if( $len < 3 ) {
		return "($field='".MRES($s)."')";
	}
	
    for( $k=0; $k<=$len-3; $k++ ) {
      $tri = mb_substr($s,$k,3,"utf-8");
      $triSql = mysql_real_escape_string( $tri, MotlDB::$db );
      $a[] = "($field like '%$tri%')";
    }
	
    $sum = implode( "+", $a );
	
    return $sum;
  }

  //----------------
  
  protected function loadItemsLow( $table, $outFields, $whereCond=null, $sortFields=null ) {
    
    $sql = "SELECT $outFields FROM $table";
    
    if( $whereCond ) {
      $sql .= " ORDER BY $whereCond";
    }
    
    if( $sortFields ) {
      $sql .= " ORDER BY $sortFields";
    }

    return MotlDB::queryRows( $sql ); // All parameters safe
  }
  
  //----------------
  
  protected function loadItemsMiddle( $whereCond=null, $sortFields=null ) { return array(); }
  
  //----------------
  
	function getItemBy( $atrName, $atrValue ) {
		$items = $this->getItemsBy( $atrName, $atrValue );
		if( $items ) {
			return $items[0];
		}
		return null;
	}
  
  //----------------
  
  function getItemsBy( $atrName, $atrValue ) {

  if( $atrValue == null ) return null;
  //die( "$atrName, $atrValue" );
    $items = $this->getBaseItems();
    
    if( ! isset($this->indexes[$atrName]) ) {
    
      $index = array();
      
      for( $k=0; $k<count($items); $k++ ) {
		$item = $items[$k];
		$name = $item->$atrName;
		if( ! isset($index[$name]) ) {
			$index[$name] = array();
		}
        $index[$name][] = $item;
      }
      $this->indexes[$atrName] = $index;
    }

    if( isset($this->indexes[$atrName][$atrValue]) ) {
      return $this->indexes[$atrName][$atrValue];
    }
    //print_r($this);die();
    
    return null;
  }
  
  //----------------
  
  function normValue($s) {
    return mb_strtolower( trim($s), "utf-8" );
  }

  //----------------
  
  function getItemListBy( $atrName, $atrValues ) {
	return $this->getItemListByUnsorted( $atrName, $atrValues );
	//return $this->getItemListBySorted( $atrName, $atrValues );
  }
  
  //----------------
  
  function getItemListBySorted( $atrName, $atrValues ) {
    $byPos = array();
    
    for( $i=0; $i<count($atrValues); $i++ ) {
      $item = $this->getItemBy( $atrName, $atrValues[$i] );
      if( $item ) {
        $byPos[$item->ThePosition] = $item;
      }
    }

    ksort($byPos);

    $out = array();
    foreach( $byPos as $key=>$item ) {
        $out[] = $item;
    }

    return $out;
  }
  
  //----------------
  
  function getItemListByUnsorted( $atrName, $atrValues ) {
    $out = array();
    
    for( $i=0; $i<count($atrValues); $i++ ) {
      $item = $this->getItemBy( $atrName, $atrValues[$i] );
      if( $item ) {
		$out[] = $item;
      }
    }

    return $out;
  }
  
  //----------------
  
  static function extractAtr( $items, $atrName ) {
    
    $out = array();
    
    for( $k=0; $k<count($items); $k++ ) {
      $out[] = $items[$k]->$atrName;
    }

    return $out;
  }
  
  //----------------

  static function extractAtrImploded( $items, $atrName, $separator ) {
  
    $values = BaseColumn::extractAtr( $items, $atrName );
    
    return implode( $separator, $values );  
  }
  
  //----------------
}

//=====================

class BaseColumnCountry extends BaseColumn {
  function getName()      { return "country"; }
  function getRusName()   { return "Страна"; }
  function getTableName() { return "db_country"; }
  function getIdCode()    { return "s"; }
  function getProducerClass() { return "DbCountryProducer"; }
  
  function loadItemsMiddle( $whereCond=null, $sortFields=null ) { 
    return $this->loadItemsLow( "db_country", "id,name", 
                                $whereCond, 
                                $sortFields ? $sortFields : "name" ); 
  }
  
}

//=====================

class BaseColumnCity extends BaseColumn {
  function getName()      { return "city"; }
  function getRusName()   { return "Город"; }
  function getTableName() { return "db_city"; }
  function getIdCode()    { return "g"; }
  function getProducerClass() { return "DbCityProducer"; }
  
  function loadItemsMiddle( $whereCond=null, $sortFields=null ) { 
    return $this->loadItemsLow( "db_city", "id,name,disambig", 
                                $whereCond, 
                                $sortFields ? $sortFields : "name" ); 
  }
  
}

//=====================

class BaseColumnClub extends BaseColumn {
  function getName()      { return "club"; }
  function getRusName()   { return "Клуб"; }
  function getTableName() { return "db_club"; }
  function getIdCode()    { return "k"; }
  function getProducerClass() { return "DbClubProducer"; }
  
  function loadItemsMiddle( $whereCond=null, $sortFields=null ) { 
    return $this->loadItemsLow( "db_club", "id,name,disambig", 
                                $whereCond, 
                                $sortFields ? $sortFields : "name" ); 
  }
  
}

//=====================

class BaseColumnTeacher extends BaseColumn {

  function getName()      { return "teacher"; }
  function getRusName()   { return "Преподаватель"; }
  function getTableName() { return "db_person"; }
  function getIdCode()    { return "p"; }
  function getProducerClass() { return "DbPersonProducer"; }

  function loadItemsMiddle( $whereCond=null, $sortFields=null ) { 
    return $this->loadItemsLow( "db_person", "id,CONCAT_WS(' ',surname,name) AS name,disambig", 
                                $whereCond, 
                                $sortFields ? $sortFields : "sex DESC,surname" ); 
  }

  public function loadSimilar( $fieldValue ) {
//      $simSql = $this->makeSimilarity( "surname", $fieldValue );
      $simSql = $this->makeSimilarity( "CONCAT_WS(' ',surname,name)", $fieldValue );
      $sql = "SELECT id, CONCAT_WS(' ',surname,name) as title, disambig, $simSql as score FROM db_person " //  $simSql safe
            ."WHERE surname Is Not Null AND surname<>'' ORDER BY score DESC";
//      echo "$sql<br>";
      return MotlDB::queryRows( $sql );
  }
  
}

//=====================

class BaseColumnBoss extends BaseColumnTeacher {

  function getName()      { return "boss"; }
  function getRusName()   { return "Руководитель"; }
}

//=====================

class BaseColumnReplace extends BaseColumn { // Not exactly Column...

  function getName()      { return "replace"; }
  function getRusName()   { return "replace"; }
  function getTableName() { return "db_replace"; }
  function getIdCode()    { return null; }
  function getProducerClass() { return null; }

  
  function __construct( $fname ) {
	$this->fname = $fname;
  }
  
  function loadItemsMiddle( $whereCond=null, $sortFields=null ) { 
	return null;
  }

  public function loadSimilar( $fieldValue ) {
      $simSql = $this->makeSimilarity( "textIn", $fieldValue );
	  
	  $minScore = mb_strlen($fieldValue,"utf-8")/2;
	  
	  // COD $simSql, $minScore
      $sql = "SELECT textOut as id, textOut as title, $simSql as score FROM db_replace " //  $simSql safe
            ."WHERE fieldName='$this->fname' "
            ."ORDER BY score DESC";
		$sql = "SELECT * FROM ($sql) AS Q WHERE score>=$minScore ";
      //echo "$sql<br>";
      return MotlDB::queryRows( $sql );
  }
  
}

//=====================
?>