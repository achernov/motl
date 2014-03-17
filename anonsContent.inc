<?php
//=====================

class AnonsPaneProducer extends Producer {

  public function produce( $format = null ) {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );
    $editmode = Motl::getParameter( "editmode", 0 );

    $rows = MotlDb::queryRows( "SELECT * FROM anons ORDER BY position" );

    $s = "";

//    $s .= "<h1>Новости</h1>";

    if( $editmode ) {
      $item = new AnonsItemProducer();
      $s .= $item->makeAddLink( null ) . "<br><br>";
    }

    for( $k=0; $k<count($rows); $k++ ) {
      $producer = new AnonsItemProducer( $rows[$k] );
      $s .= $producer->produce("short");
    }

    return $s;
  }
}

//=====================

class AnonsItemProducer extends DbProducer {
  public $row;

  //----------------

  public function getTableName() {
    return "anons";
  }

  //----------------

  public function getFields() {
	return array(
		"id"	=>	"[readonly|id]",
		"ts_created"	=>	"[ts|ts_created]",
		"position"	=>	"[text|position]",
		"theText"	=>	"[textarea|theText|class='AnonsTextArea']"
	);
  }

  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    $script = $_SERVER['PHP_SELF'];
    $linkFull   = "<A href=$script?id=$this->id>Читать дальше</a>";

    $s = "";

    switch( $format ) {

      default:
      case 'short':
        $s .= $this->makeText( $this->row, 
                         "[editbar|edit,del]<div class='Anons' width=100%>[plainBB|theText|class='Anons']</div>" );
        return $s;

      case 'edit':
        $s .= $this->makeEditForm();
        return $s;
    }
  }
}

//=====================
?>
