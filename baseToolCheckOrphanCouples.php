<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "dbAll.inc";
require_once "tables.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

checkOrphanCouples();

//=====================


function checkOrphanCouples() {

  $sql = "SELECT Couple.id,"
            ."Man.surname as msurname, Man.name as mname,"
            ."Lady.surname as fsurname,Lady.name as fname, "
            ."(SELECT count(*) FROM db_couple_event WHERE idCouple=Couple.id) AS nRegs "
         ."FROM db_couples AS Couple "
            ."LEFT JOIN db_person AS Man ON Couple.idMan=Man.id "
            ."LEFT JOIN db_person AS Lady ON Couple.idLady=Lady.id "
         ."WHERE Man.id Is Null OR Lady.id Is Null";

  $rows = MotlDB::queryRows( $sql );
  
  //----------
  
  $colId      = new Column( "id",   "id",         "[editbar|edit,del] <A href=db.php?id=c[plain|id]>[plain|id]</a>", Column::NoSort );
  $colMan     = new Column( "man",  "партнер",    "<nobr>[plain|msurname] [plain|mname]</nobr>", Column::NoSort );
  $colLady    = new Column( "lady", "партнерша",  "<nobr>[plain|fsurname] [plain|fname]</nobr>", Column::NoSort );
  $colRegs    = new Column( "regs", "N регистраций",  "[plain|nRegs]", Column::NoSort );
  
  $template = array( $colId, $colMan, $colLady, $colRegs );
  //$template = array( $colMan, $colLady, $colRegs );
  
  $_REQUEST['editmode'] = 1;
  
  $table = new Table( $template );
  $table->sortBy = Motl::getParameter( "sort" );
  $tableText = $table->produce( "DbCoupleProducer", $rows );
  
  echo "<h2>Пары без танцоров</h2>";
  if( count($rows) ) {
    echo $tableText;
  }
  else {
    echo "- не найдены";
  }
  echo "<p>";
  echo "<A href='base.php?id=tools'>вернуться</a>";
//  echo $sql;
}

//=====================
?>
