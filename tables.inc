<?php
//=====================

class Column {

  public $id;
  public $template;
  public $customSort;
//  public $highlightRow;
  public $templator;
  
  const NoSort = "NoSortFunctionAtAll!";

  //-----------

  function __construct( $id, $headerTemplate, $dataTemplate, $customSort=null ) {
    $this->id         = $id;
    $this->headerTemplate = $headerTemplate;
    $this->dataTemplate   = $dataTemplate;
    $this->customSort = $customSort;
  }

  //-----------

  function sortObjects( $objects ) {

    $order = array();

    //-----
    
    if( ! $this->customSort ) { // Standart method - sort by usual template
    
      for( $r=0; $r<count($objects); $r++ ) {
        $order[$r] = $objects[$r]->makeText( $objects[$r]->row, $this->dataTemplate );
      }
      asort($order);
      
    }
    else
    if( $this->customSort == Column::NoSort ) { // No sort at all
    
    }
    else
    if( function_exists( $this->customSort ) ) { // Custom sort function

      for( $r=0; $r<count($objects); $r++ ) {
        $order[$r] = $objects[$r]->row;
      }
      uasort( $order, $this->customSort );
      
    }
    else { // Sort by custom template

      for( $r=0; $r<count($objects); $r++ ) {
          $order[$r] = $objects[$r]->makeText( $objects[$r]->row, $this->customSort );
        }
        
      asort($order);
      
    }

    //-----

    $outOrder = array();
    foreach ( $order as $key=>$value ) {
      $outOrder[] = $key;
    }

    return $outOrder;
  }

  //-----------

  function makeHeader( $object, $baseUrl=null, $currentSort=null ) {

    $value = $object->makeText( null, $this->headerTemplate );

    if( $baseUrl ) {
      if( $this->customSort != Column::NoSort ) {
        $sort = ($currentSort==$this->id) ? "-".$this->id : $this->id;
        $value = "<A href='$baseUrl&sort=$sort'>$value</a>";
      }
    }

    return "<td class='DbTableHeader'>$value</td>";
//    return "<td class='DbTableHeader'>$value&nbsp;</td>";
  }

  //-----------

  function makeCell( $object, $rowNumber ) {
    $nm = "#";
    $object->row->$nm = $rowNumber;
    $value = $object->makeText( $object->row, $this->dataTemplate, $this->templator, $rowNumber."_" );
    $nbsp = (trim($value)=="") ? "&nbsp;" : "";
    return "<td class='DbTableCell'>$value$nbsp</td>";
  }

  //-----------
}

//=====================

class Table extends Producer {

  public $columns;
  public $sortBy;
  public $highlightRow;
  public $templator;

  //-----------

  function __construct( $columns ) {
    $this->columns = $columns;
  }

  //-----------

  public function getColumnById( $id ) {
    for( $k=0; $k<count($this->columns); $k++ ) {
      if( $this->columns[$k]->id == $id ) {
        return $this->columns[$k];
      }
    }
    return null;
  }

  //-----------

  public function produce( $class, $rows, $baseUrl=null ) {

    $s = "\n<table cellspacing=0 border=1>\n";

    //----- Init

    for( $k=0; $k<count($this->columns); $k++ ) {
      $this->columns[$k]->templator = $this->templator;
    }
	
    //----- Header

    $headerObject = new DbProducer();

    $s .= "<tr>";

    for( $k=0; $k<count($this->columns); $k++ ) {
      $s .= $this->columns[$k]->makeHeader( $headerObject, $baseUrl, $this->sortBy );
    }

    $s .= "</tr>\n";

    //----- Prepare Data

    $order   = array();

    $objects = array();

    for( $k=0; $k<count($rows); $k++ ) {
      $order[$k]   = $k;
      $objects[$k] = new $class( $rows[$k] );
    }

    //----- Sort

    if( $this->sortBy ) {
    
      if( $this->sortBy[0] == "-" ) {
        $idSort = substr( $this->sortBy, 1 );
        $inverse = 1;
      }
      else {
        $idSort = $this->sortBy;
        $inverse = 0;
      }
      
      $sortColumn = $this->getColumnById( $idSort );

      if( $sortColumn ) {
        $order = $sortColumn->sortObjects( $objects );
        
        if( $inverse ) {
          $order = array_reverse( $order );
        }
      }
    }

    //----- Data

    for( $r=0; $r<count($rows); $r++ ) {
//    for( $r=0; $r<10; $r++ ) {
      $object = $objects[ $order[$r] ];

      $highlight = ($object->row == $this->highlightRow);
      
      $s .= $highlight ? "<tr style='background-color:#ffa0a0'>" : "<tr>";
      for( $k=0; $k<count($this->columns); $k++ ) {
        $s .= $this->columns[$k]->makeCell( $object, $r );
      }
      $s .= "</tr>\n";
    }
    
    $s .= "</table>\n";

    return $s;    
  }

  //-----------

}

//=====================
?>