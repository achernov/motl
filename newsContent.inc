<?php
require_once "_dbproducer.inc";
//=====================

class NewsContent extends Producer {

  public function produce( $format = null ) {

    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );
    $editmode = Motl::getParameter( "editmode", 0 );

    $rows = MotlDB::queryRows( "SELECT * FROM news ORDER BY date DESC" );

    $s = "";

    if( $id ) {
      for( $k=0; $k<count($rows); $k++ ) 
        if( $rows[$k]->id == $id ){
          $producer = new NewsItemProducer( $rows[$k] );
          $s .= $producer->produce("full");
        }
    }
    else {
      $s .= "<h1>Новости</h1>";

      if( $editmode ) {
        $item = new NewsItemProducer();
        $s .= $item->makeAddLink( null ) . "<br><br>";
      }

      for( $k=0; $k<count($rows); $k++ ) {
        $producer = new NewsItemProducer( $rows[$k] );
        $s .= $producer->produce("short");
      }
    }

    return $s;
  }
}

//=====================

class NewsItemProducer extends DbProducer {
  public $row;

  //----------------

  public function getTableName() {
    return "news";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"date"	=>	"[text|date]",
		"title"	=>	"[text|title|style='width:100%']",
		"shortText"	=>	"[textarea|shortText|style='width:100%; height:150']",
		"theFullText"	=>	"[textarea|theFullText|style='width:100%; height:200']",
		"author"	=>	"[text|author]" 
	);
  }

  //----------------

  public function loadFromQuery() {
    parent::loadFromQuery();
    $this->row->dateHuman = Motl::getParameter( "dbf_dateHuman" );
  }

  //----------------

  public function saveToDB() {
    $this->row->date = DataUtil::dateHumanToDb( $this->row->dateHuman );

    $this->saveRowToDB();
  }

  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    $script = $_SERVER['PHP_SELF'];
    $editmode = Motl::getParameter( "editmode", 0 );
    $linkFull   = "<A href=$script?id=$this->id>Читать дальше</a>";

    if( $this->row )
      if( $this->row->date )
        $this->row->dateHuman = DataUtil::dateDbToHuman( $this->row->date );

    $s = "";

    //=== Templates ===
     
    $templateFull  = "[editbar|edit]<h1>[plain|dateHuman] [plain|title]</h1><i>[plainBB|shortText]</i><br>[plainBB|theFullText]";

    $templateShort = "<table class='NewsItem'><tr><td>[editbar|edit,del]"
                     ."<div width=100%><b>[plain|dateHuman] - [plain|title]</b><br>"
                     ."[plainBB|shortText] [if|theFullText|$linkFull]<p></div></td></tr></table>";

    //=== End Templates ===

    switch( $format ) {

      case 'full':
      default:
        $s .= $this->makeText( $this->row, 
                           $templateFull );
        return $s;

      case 'short':
        $s .= $this->makeText( $this->row, $templateShort );
        return $s;

      case 'edit':
        if( ! isset($this->row->dateHuman) || ! $this->row->dateHuman ) {
          $this->row->dateHuman = date( "d.m.Y H:i", time() );
        }
        $s .= $this->makeEditForm();
        return $s;
    }
  }
}

//=====================
?>
