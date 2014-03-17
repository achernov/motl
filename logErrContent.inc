<?php
require_once "_core.inc";
require_once "_dbproducer.inc";
require_once "tables.inc";
//=====================

class LogErrContent extends Producer {

  //================

  function produce() {
    $id  = Motl::getParameter( "id" );
	
	if( ! $id ) {
	  $rows = MotlDB::queryRows( "SELECT * FROM errors ORDER BY id DESC" );
	  return LogErrItem::produceTable( $rows );
	}
	else {
	  $item = new LogErrItem();
	  $item->loadFromDb( $id );
	  return $item->produce();
	}
	
  }


  //================
}

//=====================

class LogErrItem extends DbProducer {

  //================

  function produce() {
  
    $template = "<table cellpadding=4 cellspacing=0 border=1>";
	$template .= "<tr>	<td><b>id</b></td>	<td>[plain|id]</td></tr>";
	$template .= "<tr>	<td><b>errorTime</b></td>	<td>[plain|errorTime]</td></tr>";
	$template .= "<tr>	<td><b>message</b></td>	<td>[plain|message]</td></tr>";
	$template .= "<tr>	<td><b>file</b></td>	<td>[plain|file]</td></tr>";
	$template .= "<tr>	<td><b>line</b></td>	<td>[plain|line]</td></tr>";

	$template .= "<tr>	<td colspan=2><b>callStack</b><p>	<pre>[plain|callStack]</pre></td></tr>";
	$template .= "<tr>	<td colspan=2><b>cookies</b><p>		<pre>[plain|cookies]</pre></td></tr>";
	$template .= "<tr>	<td colspan=2><b>getVars</b><p>		<pre>[plain|getVars]</pre></td></tr>";
	$template .= "<tr>	<td colspan=2><b>postVars</b><p>	<pre>[plain|postVars]</pre></td></tr>";

	$template .= "</table>";
  
    return $this->makeText( $this->row, $template );
  }

  //================

  public static function produceTable( $rows ) {

    $script = $_SERVER['PHP_SELF'];
  
    $columns = array(
      new Column( "id",		"id",		"[plain|id]" ),
      new Column( "errorTime",	"errorTime",	"<A href='$script?id=[plain|id]'>[plain|errorTime]</a>", "[plain|errorTime]" ),
      new Column( "message",	"message",	"[plain|message]" ),
      new Column( "file",	"file",		"[plain|file]" ),
      new Column( "line",	"line",		"[plain|line]" )
/*      new Column( "callStack",	"callStack",	"[plain|callStack]" ),
      new Column( "cookies",	"cookies",	"[plain|cookies]" ),
      new Column( "getVars",	"getVars",	"[plain|getVars]" ),
      new Column( "postVars",	"postVars",	"[plain|postVars]" )*/
	);
	
	$table= new Table( $columns );
	$table->sortBy = Motl::getParameter( "sort" );
	return $table->produce( "LogErrItem", $rows, "$script?" );
  }
  
  //----------------

  public function getTableName() {
    return "errors";
  }

  //----------------

  public function getFieldNames() {
    return array( "id", "errorTime", "message", "file", "line", "callStack", "cookies", "getVars", "postVars" );
  }

  //----------------
  //================
}

//=====================
//002184
?>
