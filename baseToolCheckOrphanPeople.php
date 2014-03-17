<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

checkOrphanPeople();

//=====================


function checkOrphanPeople() {

  $sql = "SELECT p.* "
    ."FROM db_person AS p "
    ."WHERE "
    ."( isTeacher Is Null Or Not isTeacher ) "
    ."AND NOT EXISTS	(SELECT * FROM db_couples as s "
                     ."WHERE concat(',',s.teacher,',') like concat('%,',p.id,',%')) "
    ."AND NOT EXISTS	(SELECT * FROM db_person as s "
                     ."WHERE s.teacher<>'' and concat(',',s.teacher,',') like concat('%,',p.id,',%')) "
    ."AND NOT EXISTS	(SELECT * "
                     ."FROM db_couples AS cm INNER JOIN db_couple_event AS cme ON cme.idCouple=cm.id "
                     ."WHERE cm.idMan=p.id) "
    ."AND NOT EXISTS	(SELECT * "
                     ."FROM db_couples AS cl INNER JOIN db_couple_event AS cle ON cle.idCouple=cl.id "
                     ."WHERE cl.idLady=p.id) "
    ."AND NOT EXISTS	(SELECT * FROM db_couple_event AS cse WHERE cse.idSolo=p.id)";

  $rows = MotlDB::queryRows( $sql );
  
  //----------
  
  $colSel    = new Column( "sel",  "<u onclick='toggleSels(\"dbf_\",\"_check\",0)'>&#8730;</u>",            "[ahidden|id][check|check]", Column::NoSort );
  
  $colId       = new Column( "id",       "id",      "[editbar|edit,del] <A href=db.php?id=p[plain|id]>[plain|id]</a>", "[plain|id]" );
  $colName     = new Column( "name",     "Имя",     "[plain|name]" );
  $colSurname  = new Column( "surname",  "Фамилия", "[plain|surname]" );
  $colBirth    = new Column( "birth",    "Дата рожд.", "[plain|birthDate]" );
  
  $template = array( $colSel, $colId, $colSurname, $colName, $colBirth );
  
  $_REQUEST['editmode'] = 1;
  
  $table = new Table( $template );
  $table->sortBy = Motl::getParameter( "sort", "surname" );
  $tableText = $table->produce( "DbPersonProducer", $rows );
  
  echo "<h2>\"Мертвые души\"</h2>";
  
  echo "<script src='db.js'></script>";
    
  if( count($rows) ) {
    echo "<form action='baseTools.php' method='POST'>\n";
    echo $tableText;
    echo "<input type='submit' name='deletePersons' value='Удалить'>\n";
    echo "</form>\n";
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
