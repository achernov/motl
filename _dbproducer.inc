<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_templator.inc";
require_once "dataUtil.inc";
require_once "sortManager.inc";
//=====================

class DbProducer extends Producer {

  public $isNew;

  public $row;
  public $id;

  public $format;

  //----------------

  function __construct( $row = null ) {
    $this->row = $row;
    if( $this->row )
      if( isset($this->row->id) ) {
        $this->id = $this->row->id;
        $this->aid = array( $this->row->id );
      }
    ASSERT_alnum( $this->id );
  }

  //================

  public function setFormat( $format ) {
    $this->format = $format;
  }

  //================

  public function loadFromDB($id) {
//    ASSERT_alnum( $id );

    $this->id = $id;

    $this->aid = Motl::splitId($id);

    $this->loadRowFromDB( $this->aid[0] );
  }

  //----------------
  // SQLPARAM  $cond, $order
  
  public function loadMultipleFromDB( $cond=null, $order=null ) { // Assert yourself!!!
  
    $sql = "SELECT * FROM " . $this->getTableName();
    
    // PAR $cond
    if( $cond ) $sql .= " WHERE $cond";
    
    // PAR $order
    if( $order ) $sql .= " ORDER BY $order";
    
    $rows = MotlDB::queryRows( $sql ); 
    
    $out = array();

    $class = get_class($this);

    for( $k=0; $k<count($rows); $k++ ) {
      $out[] = new $class( $rows[$k] );
    }

    return $out;  
  }

  //----------------
  // SQLPARAM  $cond, $order
  
  public function produceMultiple( $format, $cond, $sort="date DESC", $noItemsText="" ) {

    $s = "";
    
    if( Motl::getParameter("editmode") ) {
      $s .= $this->makeAddLink( null ) . "<br><br>";
    }

    $items = $this->loadMultipleFromDB( $cond, $sort ); // PAR $cond, $sort

    for( $k=0; $k<count($items); $k++ ) {
      $s .= $items[$k]->produce( $format );
    }
	
	if( ! count($items) ) {
		$s .= $noItemsText;
	}
	
    return $s;
  }

  //----------------

  public function loadRowFromDB($id) {
    ASSERT_alnum( $id );
    $this->row = MotlDB::queryRow( "SELECT * FROM " . $this->getTableName() . " WHERE id='$id'" );
  }

  //----------------

  public function killFromDB() {
    $this->killRowFromDB();
  }

  //----------------

  public function killRowFromDB() {

    $table = $this->getTableName();
    $id = $this->row->id;
  
    ASSERT_alnum( $id );
    MotlDB::query( "DELETE FROM $table WHERE id='$id'" );
  }

  //----------------

  public function saveToDB() {
    $this->saveRowToDB();
  }

  //----------------

  public function saveRowToDB() {

    $fields = $this->getFieldNames();
    
    $fieldsForDb = array();

    for( $k=0; $k<count($fields); $k++ ) {
    
      $fname = $fields[$k];
      
      if( isset($this->row->$fname) ) {
      
        $fvalue = $this->row->$fname;
        
        if( ! strlen($fvalue) ) {
          $fvalue = null;
        }

        if( $fname != "id" ) {
          ASSERT_name( $fname );
          $fieldsForDb[$fname] = $fvalue;
        }
        
      }
      
    }

    ASSERT_alnum( $this->id );

    $id = MotlDB::saveRow( $this->getTableName(), "id", $this->id, $fieldsForDb );

    $this->id = $id;
    $this->row->id = $id;
  }

  //----------------
  
  public function loadFromString( $fromString ) {
  }
  
  //----------------

  public function loadFromQuery( $rownumber=null ) {
//  echo "<pre>";print_r($_REQUEST);echo"<hr>";
    $fields = $this->getFieldNames();//print_r($fields);
    $this->row = new stdClass();
    $this->id = $this->getValueFromQuery( "dbf_id", $rownumber );

    $fromString = $this->getValueFromQuery( "fromString", $rownumber );
    //echo "==$fromString==<br>";
    if( strlen($fromString) ) {
      $this->loadFromString( $fromString );
    }
    
    for( $k=0; $k<count($fields); $k++ ) {
      $fname = $fields[$k];
      $fvalue = $this->getValueFromQuery( "dbf_$fname", $rownumber );
      $nl = is_null($fvalue);
//echo "$fname :$nl: $fvalue<p>";
      if( ! is_null($fvalue) ) { // If given some value
        $this->row->$fname = $fvalue;
      }
    }

//  echo "<pre>";print_r($this->row);echo"</pre>";
    //print_r($this);
//die();
  }

  //------

  private function getValueFromQuery( $fname, $rownumber ) {
  
    if( ! isset( $_REQUEST[$fname] ) ) return null; // means 'not given at all'
    
    $v = $_REQUEST[$fname];
    
    if( !is_array($v) && is_null($rownumber) ) { // scalar value
      return $v;
    }

    if( is_array($v) && !is_null($rownumber) ) { // item of array
      return $v[$rownumber];
    }

    if( is_array($v) && is_null($rownumber) ) { // array for one item - it's a multiple <select>
      return implode( ';', $v );
    }

    // else error
    $a = is_array($v)?1:0;
    $r = is_null($rownumber)?0:1;
    die( "$fname a$a r$r" );
  }

  //================

  public function getTableName() {
    return "";
  }

  //----------------

  public function getFieldNames() {
	return array_keys( $this->getFields() );
  }
  
  //----------------

  public function getFields( $full = false ) {
    die( "<b>ERROR: getFields() not defined for <font color=blue>".get_class($this)."</font>" );
  }

  //================

  public function produce() {
  }

  //----------------  in case it's sortable

  private function makeSortId() {
    return get_class($this).$this->id;
  }
  
  public function makeSortLink() {
    return SortManager::makeSortLink( $this->makeSortId() );
  }

  public function checkSorting() {
    return SortManager::checkSorting( $this->makeSortId() );
  }
  
  //----------------

  public function isEditMode() {
     return ( Motl::getParameter( "editmode", "0" ) ? 1 : 0 );
  }

  //----------------

  public function makeEditLink( $label="Edit", $id=null ) {
    $returl = Motl::makeRetUrl();

    $id0 = $id ? $id : $this->aid[0];
    
    return "<A class='editLink' href='editObject.php?returl=$returl&what=".get_class($this)."&dbf_id=$id0' >$label</a>";
  }

  //----------------

  public function makeCopyLink($label="Copy") {
    $returl = Motl::makeRetUrl();

    $id0 = $this->aid[0];
    
    return "<A class='editLink' href='editObject.php?returl=$returl&doCopy=1&what=".get_class($this)."&dbf_id=$id0' >$label</a>";
  }

  //----------------

  public function makeAddLink( $pid, $init_fields=null ) {
    $returl = Motl::makeRetUrl();
    $url = "editObject.php?returl=$returl&what=".get_class($this)."&dbf_pid=$pid";

    if( $init_fields ) {
      foreach( $init_fields as $name=>$value ) {
        $url .= "&dbf_$name=$value";
      }
    }

    return "<A class='editLink' href='$url' >Add</a>";
  }

  //----------------

  public function makeSqlLink( $label="Sql", $id=null ) {
	if( $id ) {
		$id0 = $id;
	}
	else {
		if( isset($this->aid) ) {
			$id0 = $this->aid[0];
		}
		else {
			return null;
		}
	}

    return "<A class='editLink' href='sqlObject.php?what=".get_class($this)."&dbf_id=$id0'>$label</a>";
  }

  //----------------

  public function makeKillLink( $label="Del", $id=null ) {
    $returl = Motl::makeRetUrl();
    $id0 = $id ? $id : $this->aid[0];
    return "<A class='editLink' href='killObject.php?returl=$returl&what=".get_class($this)."&dbf_id=$id0' onclick='return confirmKill()'>$label</a>";
  }

  //----------------
  
  protected function makeMultiEditRowTemplate( $fields ) {
  
	$fieldsInfo = $this->getFields(true);
	
	$template = "<tr class='DbTableCell'> ";
	foreach( $fields as $fieldName ) {
		$fieldTemplate = $fieldsInfo[$fieldName];
		$template .= "<td>$fieldTemplate</td>";
	}
	$template .= " </tr>\n";
	return $template;
  }
  
  //----------------
  
  public function makeMultiEditForm($rows,$fields) {
	$template = $this->makeMultiEditRowTemplate( $fields );
	
	$s = "";
	$rowNumber = 0;
	
    $returl = Motl::htmlEscape( Motl::getParameter("returl") );

	$ids = array();
	foreach( $rows as $row ) {
		$ids[] = $row->id;
	}
	$sids = implode( ",", $ids );
	
    $s = "<form action='saveObject.php' method='POST'>"
		."<input type=hidden name='returl' value='$returl'>"
		."<input type=hidden name='what' value='".get_class($this)."'>"
		."<input type=hidden name='dbf_ids' value='$sids'>"
		."<input type=hidden name='editmode' value='1'>";

	$s .= "<table border=1 cellspacing=0 cellpadding=10>\n";

	$s .= "<tr class='DbTableHeader'> ";
	foreach( $fields as $fieldName ) {
		$s .= "<td>$fieldName</td>";
	}
	$s .= " </tr>\n";

	foreach( $rows as $row ) {
		$s .= $this->makeText( $row, $template, null, $rowNumber."_" );
		$rowNumber ++;
	}
	
	$s .= "</table><p>\n";
	
    $s .= "<input type=submit value='Save'>"
		."</form>";
	
	return $s;
	//return $this->makeEditForm( $template );
  }
  
  //----------------
  
  protected function makeEditTemplate() {
	$fields = $this->getFields(true);
	$template = "";
	foreach( $fields as $fieldName=>$fieldTemplate ) {
		$template .= "<b>$fieldName<br>$fieldTemplate<br>";
	}
	return $template;
  }
  
  //----------------

  public function makeEditForm( $template=null ) {
  
	if( ! $template ) {
		$template = $this->makeEditTemplate();
	}
  
    $returl = Motl::htmlEscape( Motl::getParameter("returl") );

    $form = "<form action='saveObject.php' method='POST'>"
            ."<input type=hidden name='returl' value='$returl'>"
            ."<input type=hidden name='what' value='".get_class($this)."'>"
            ."<input type=hidden name='dbf_id' value='$this->id'>"
            ."<input type=hidden name='editmode' value='1'>";

	$form .= $this->makeSqlLink();

    $form .= $this->makeText( $this->row, $template );

    $form .= "<input type=submit value='Save'>"
            ."</form>";
    return $form;
  }

  //----------------

  public function makeText( $dataRow, $template, $templator=null, $prefix=null ) {

    if( ! $templator )
      $templator = new DbProducerTemplator();

    $templator->obj = $this;
    $templator->row = $dataRow;
    $templator->prefix = $prefix;

    return $templator->process( $template );
  }

  //----------------
  
  public function makeList( $dataRows, $template, $separator=", " ) {
    
    $arr = array();
    
    for( $k=0; $k<count($dataRows); $k++ ){
      $arr[]  = $this->makeText( $dataRows[$k], $template );
    }
    return implode( $separator, $arr );
  }
  
  //----------------
  // SQLPARAM $table
  
  private function loadPath( $root, $table, $script, $field, $idTemplate=null ) {
    $editmode = Motl::getParameter( "editmode", 0 );

    $stack = array();

    $pid = isset($this->row->pid) ? $this->row->pid : null;
    ASSERT_alnum($pid);
    $start_row = $this->isNew ? (object)array( 'id'=>'new', 'pid'=>$pid, $field=>'new object' )
                              : $this->row;

    // PAR $table
    // ASS,FLD $r->pid
    for( $r=$start_row; $r; $r=MotlDB::queryRow( "SELECT * FROM $table WHERE id='$r->pid'" ) ) {  
      $stack[] = $r;

      if( !isset($r->pid) || is_null($r->pid) ) break;
    }

    if( ! is_null($root) ) {
      $stack[] = (object)array("id"=>null,$field=>$root);
    }

    $apath = array();
    for( $k=count($stack)-1; $k>=0; $k-- ) {
      $level = $stack[$k];
      $title = isset($level->$field) ? $level->$field : "";

      if( isset($level->id) && ! is_null($level->id) ) {
        $idLev = $idTemplate ? str_replace( "@", $level->id, $idTemplate ) : $level->id;
      }
      else {
        $idLev = null;
      }

      $apath[] = (object)array( "id"=>$idLev, "title"=>$title, "last"=>($k==0) );
    }

    return $apath;
  }

  //----------------

  public function makePath( $root, $table, $script, $field, $idTemplate=null ) {
    $editmode = Motl::getParameter( "editmode", 0 );
  
    $pathData = $this->loadPath( $root, $table, $script, $field, $idTemplate );

    $apath = array();
    for( $k=0; $k<count($pathData); $k++ ) {
      $level = $pathData[$k];

      $levelHtml = $level->last 
                      ? "<font class='ForumPathNoLink' >$level->title</font>"
                      : "<A class='ForumPathLink' href='$script?id=$level->id'>"
                              ."$level->title</a>";
                              
      $apath[] = $levelHtml;
    }

    return "<div class='ForumPath'>". implode( " <b>/</b> ", $apath ) ."</div>";
  }

  //----------------

  public function makePathWap( $head, $table, $script, $field, $idTemplate=null ) {
    $editmode = Motl::getParameter( "editmode", 0 );
  
    $pathData = $this->loadPath( null, $table, $script, $field, $idTemplate );

    $apath = array();
    for( $k=0; $k<count($pathData); $k++ ) {
      $level = $pathData[$k];

      $levelHtml = $level->last 
                      ? "<b>$level->title</b>"
                      : "<a href='$script?id=$level->id&amp;editmode=$editmode'>"
                              ."$level->title</a>";
                              
      $apath[] = $levelHtml;
    }

    return "<p>". $head . implode( " <b>/</b> ", $apath ) ."</p>";
  }

  //----------------
}

//=====================

class DbProducerTemplator extends Templator {

  public $obj;

  public $row;

  public $prefix;

  function __construct( $row = null ) {
    $this->row = $row;
  }

  //===========

  protected function getPart( $tagParts, $k ) {
    return isset( $tagParts[$k] ) ? $tagParts[$k] : "";
  }

  //===========

  protected function getFName( $tagParts, $num=1 ) {
    return $this->getPart( $tagParts, $num );
  }

  //-----------

  protected function getMore( $tagParts ) {
    return $this->getPart( $tagParts, 2 );
  }

  //-----------

  protected function getFValue( $tagParts, $num=1 ) {
    if( ! $this->row )
      return "";

    $fn = $this->getFName( $tagParts, $num );
    $fv = isset($this->row->$fn) ? $this->row->$fn : "";

    return $fv;
  }

  //-----------

  protected function filterSpan( $value, $tagParts ) {
    if( isset($tagParts[2]) ) {
      $value = strlen($value) ? "<span $tagParts[2]>$value</span>" : "";
//      $value = strlen($value) ? "<font $tagParts[2]>$value</font>" : "";
    }
    return $value;
  }

  //===========

  function processTag_if( $tagParts ) {  // [if|cond|then_part|else_part]
//echo "<hr>";
    $cond = $this->getPart( $tagParts, 1 );
    
    if( $this->calcBoolExpression( $cond, $this->row ) ) {
      return $this->getPart( $tagParts, 2 );  // The THEN
    }
    else {
      return $this->getPart( $tagParts, 3 );  // The ELSE
    }
    
  }

  //===========
  
  private function calcBoolExpression( $expr, $row ) {
//echo "$expr<br>";
    $orList = explode( ",", $expr );
    
    $ored = false;
    foreach( $orList as $orTerm) {
      $andList = explode( "&", $orTerm );
      
      $anded = true;
      foreach( $andList as $andTerm) {
        $anded = $anded && $this->calcBoolTerm( $andTerm, $row );
      }
//      echo "anded=$anded<br>";
      $ored = $ored || $anded;
    }

    return $ored;
  }  
  
  //-----------
  
  private function calcBoolTerm( $c , $row ) {
    $result = isset($row->$c) && strlen($row->$c) && $row->$c;
//    echo "calcBoolTerm($c,... = $result<br>";
    return $result;
  }
  
  //===========

  function processTag_editbar( $tagParts ) {
    $editmode = Motl::getParameter( "editmode", 0 );
    if( ! $editmode ) return "";

    $list = $this->getPart( $tagParts, 1 );
    if( ! $list ) $list = "edit,del,add";

    if( count($tagParts) >= 4 ) {
      $objClassName = $this->getPart( $tagParts, 2 );
      $objId        = $this->getPart( $tagParts, 3 );
      $obj = new $objClassName;
      $obj->loadFromDB($objId);
    }
    else {
      $obj = $this->obj;
    }
    
   
    $addLink  = $obj->makeAddLink( $obj->row ? $obj->row->id : null );

    $links = array();
	$alist = explode( ",", $list );
	foreach( $alist as $cmd ) {
		switch( $cmd ) {
		
			case "edit": 
				$links[] = $obj->makeEditLink(); 
				break;
				
			case "copy": 
				$links[] = $obj->makeCopyLink(); 
				break;
				
			case "del":  
				$links[] = $obj->makeKillLink(); 
				break;
				
			case "add":  
				$links[] = $obj->makeAddLink( 
								$obj->row ? $obj->row->id : null ); 
				break;
				
			case "sort": 
				$links[] = $obj->makeSortLink(); 
				break;
				
			default:
				$method = "processEditCommand_$cmd";
				$links[] = $obj->$method();
				//die("$method");
				break;
		}
	}
    $slinks = implode( "&nbsp;", $links );

    $more = $this->getPart( $tagParts, 2 );

    return "$slinks$more";
  }

  //-----------

  function processTag_prefix( $tagParts ) {
    return $this->prefix;
  }

  //-----------

  function processTag_check( $tagParts ) {
    return "<input type='checkbox' name='dbf_".$this->prefix.$this->getFName($tagParts)."' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------

  function processTag_ahidden( $tagParts ) {
    return "<input type='hidden' name='dbf_".$this->getFName($tagParts)."[]' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------

  function processTag_atext( $tagParts ) {
    return "<input type='text' id='dbf_".$this->prefix.$this->getFName($tagParts)."' name='dbf_".$this->getFName($tagParts)."[]' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------


  function processTag_hidden( $tagParts ) {
    return "<input type='hidden' name='dbf_".$this->prefix.$this->getFName($tagParts)."' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------

  function processTag_text( $tagParts ) {
    return "<input type='text' name='dbf_".$this->prefix.$this->getFName($tagParts)."' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------

  function processTag_textarea( $tagParts ) {
    $value = $this->getFValue($tagParts);
    $value = htmlspecialchars( $value );
    return "<textarea name='dbf_".$this->prefix.$this->getFName($tagParts)."' ".$this->getMore($tagParts).">$value</textarea>";
  }

  //-----------

  function processTag_readonly( $tagParts ) {
    return "<input type='text' readonly style='background-color:#e0e0e0' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($this->getFValue($tagParts))."'>";
  }

  //-----------

  function processTag_ts( $tagParts ) {
    $value = DbProducerTemplator::dateDbToHuman( $this->getFValue($tagParts) );
    return "<input type='text' readonly style='background-color:#e0e0e0;color:green' ".$this->getMore($tagParts)." value='".Motl::htmlEscape($value)."'>";
  }

  //-----------

  function processTag_select( $tagParts ) {
    $value = $this->getFValue($tagParts);
    $alist = $this->getPart( $tagParts, 2 );
    $more  = $this->getPart( $tagParts, 3 );

    $list = explode( ";", $alist );
    $slist = "<option value=''>&lt;&lt;none&gt;&gt;";

    $aval = explode( ";", $value ); // in case it's multiple
    $fval = array();
    foreach( $aval as $v ) {
      $fval[$v] = 1;
    }

    foreach( $list as $item ) {
      $aitem = explode( ":", $item );
      $label = $aitem[0];
      $value = isset($aitem[1]) ? $aitem[1] : $aitem[0];

      $selected = isset($fval[$value]) ? " selected" : "";

      if( isset($aitem[1]) ) {
        $slist .=  "<option value='$aitem[1]'$selected>$aitem[0]";
      }
      else {
        $slist .=  "<option$selected>$aitem[0]";
      }
    }

    $brakets = strchr( $more, "multiple" ) ? "[]" : "";

    return "<select name='dbf_".$this->prefix.$this->getFName($tagParts)."$brakets' $more>$slist</select>";
  }

  //-----------

  function processTag_plain( $tagParts ) {
    $value = $this->getFValue($tagParts);
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }

  //-----------

  function processTag_numword( $tagParts ) {
    $value = $this->getFValue($tagParts);
    if( strlen($value) == 0 ) return "";

    $w1 = $this->getPart( $tagParts, 2 );
    $w2 = $this->getPart( $tagParts, 3 );
    $w5 = $this->getPart( $tagParts, 4 );

    $w = DataUtil::numForm( $value, $w1, $w2, $w5 );

    $value = $this->filterSpan( "$value $w", $tagParts );
    return $value;
  }

  //-----------

  function processTag_plainBB( $tagParts ) {
    $value = DataUtil::filterBB( $this->getFValue($tagParts) );
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }

  //-----------

  function processTag_plainDateTime( $tagParts ) {
    $value = DbProducerTemplator::dateDbToHuman( $this->getFValue($tagParts) );
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }

  //-----------

  function processTag_plainDate( $tagParts ) {
  
    $value = $this->getFValue($tagParts);
    if( $value && $value!="0000-00-00 00:00:00" ) {
      $dateTS = strtotime( $value );
      $value = date( "d.m.Y", $dateTS );
    }
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }
  
  //-----------

  function processTag_plainDateAge( $tagParts ) {
  /*$date1 = strtotime( "2001-1-1" );
  $date2 = strtotime( "2002-1-1" );
  die( "$date2-$date1=".($date2-$date1) );*/
    $value = $this->getFValue($tagParts);
    
    if( $value ) {
    
      $dateTS = strtotime( $value );
      $dateNow = time();
      
      $age = round( ($dateNow-$dateTS)/(365.25*86400), 1 );
      $word = DataUtil::numForm( floor($age), "год", "года", "лет" );
      
      $value = date( "d.m.Y ($age $word)", $dateTS );
    }
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }
  
  //-----------

  function processTag_plainDateRange( $tagParts ) {
    $value  = $this->getFValue($tagParts);
    $value1 = $this->getFValue($tagParts,2);
    
    if( $value ) {
      $value = date( "d.m.Y", strtotime( $value ) );
    }
    if( $value1 ) {
      $value1 = date( "d.m.Y", strtotime( $value1 ) );
    }
    
    if( $value && $value1 ) {
      $a = explode( ".", $value );
      $b = explode( ".", $value1 );
      if( $a[0] != $b[0] ) {
        $value = "$a[0]-$b[0].$a[1].$a[2]";
      }
      if( $a[1] != $b[1] ) {
        $value = "$a[0].$a[1]-$b[0].$b[1].$a[2]";
      }
      if( $a[2] != $b[2] ) {
        $value = "$a[0].$a[1].$a[2]-$b[0].$b[1].$b[2]";
      }
    }
    else {
      $value = $value.$value1;
    }
    
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }

  //===========

  private static function dateDbToHuman( $s ) {
    $dateTS = strtotime( $s );
    return $s ? date( "d.m.Y H:i", $dateTS ) : "";
  }

  //----------------

  private static function dateHumanToDb( $s ) {

    $dateTS = $this->parseHumanDate( $s );

    return $dateTS ? date( "Y-m-d H:i:s", $dateTS ) : null;
  }
  //===========
}

//=====================

?>
