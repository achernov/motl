<?php
require_once "_dbproducer.inc";
//=====================

class PageContent extends DbProducer {

  //----------------

  public function produce( $format = null ) {
    if( ! $format ) $format = $this->format;

    $id = $this->id;

    $s = "";
    if( $format == "edit" ) {

      $s .= $this->makeEditForm();
    }
    else {
      global $isMainPage;
      if( $isMainPage ) {
        $wapPage = new PageContent();
        $wapPage->id = "wapNews";
        $wapPage->loadFromDB("wapNews");
        $wapEditLink = $wapPage->makeEditLink("Edit WAP");
      }
      else {
        $wapEditLink = "";
      }
    
      $s .= $this->makeText( $this->row, 
                             "[editbar|edit|&nbsp;&nbsp;$wapEditLink<br>][plainBB|pageText]" );
    }
    
    return $s;
  }

  //----------------

  public function getTableName() {
    return "staticPages";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"noRight"	=>	"[text|noRight|style='width:20']",
		"pageText"	=>	"[textarea|pageText|style='width:100%; height:400']" 
	);
  }

  //----------------
}

//=====================
?>
