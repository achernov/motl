<?php
//=====================
require_once "subjectUtils.inc";
//=====================

class DbClubProducerFull extends DbClubProducer {
  //----------------

  public function produceFull() {
    $id = $this->row->id;

    $s = "";
    
    //===== Show Common Data
    $title = $this->row->name;
    $subtitle = isset($this->row->disambig) ? " <i>(".$this->row->disambig.")</i>" : "";
    $s .= "<h1>База данных: клуб \"$title\"$subtitle</h1>";

    if( Motl::getParameter( "editmode" ) ) {
      $s .= $this->makeEditLink() . "<p>";
    }

	$s .= $this->makeAttr( "Руководитель",'boss' );
	$s .= $this->makeAttr( "Преподаватели",'teacher' );
	$s .= $this->makeAttr( "Город",'city' );
	$s .= $this->makeAttr( "Страна",'country' );
	
    //===== End Show Common Data

    $s .= "&nbsp;<p>";

    //===== Show Couples
    
    $editmode = Motl::getParameter( "editmode" );
    $url = "db.php?id=k$id";
    $column = new BaseColumnClub();
    
    $s .= SubjectUtils::makePupils( $url, $column, $id );
    
    //===== End Show Competitions

    return $s;
  }

  //----------------
}

//=====================
?>
