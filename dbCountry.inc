<?php
//=====================
require_once "subjectUtils.inc";
//=====================

class DbCountryProducerFull extends DbCountryProducer {
  //----------------

  public function produceFull() {
    $id = $this->row->id;
    
    $s = "";
    
    //===== Show Common Data
    $title = $this->row->name;
    $s .= "<h1>База данных: страна $title</h1>";

    if( Motl::getParameter( "editmode" ) ) {
      $s .= $this->makeEditLink() . "<p>";
    }

    //===== End Show Common Data

    $s .= "&nbsp;<p>";

    //===== Show Couples
    
    $editmode = Motl::getParameter( "editmode" );
    $url = "db.php?id=s$id";
    $column = new BaseColumnCountry();
    
    $s .= SubjectUtils::makePupils( $url, $column, $id );
    
    //===== End Show Competitions
    
    return $s;
  }

  //----------------
}

//=====================
?>
