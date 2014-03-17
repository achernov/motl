<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "dbPerson.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";die();
//=====================

if( Motl::getParameter("setMales") ) {
  setSexForSelected('M');
}
else
if( Motl::getParameter("setFemales") ) {
  setSexForSelected('F');
}

checkSex();

//=====================

function setSexForSelected( $sex ) {

  foreach( $_REQUEST as $ctl=>$v ) {
    if( substr($ctl,0,4) == 'chk_' ) {
      $id = (int)substr($ctl,4);
      MotlDB::query( "UPDATE db_person SET sex='$sex' WHERE id='$id'" ); // COD $id, $sex
    }
  }
}

//=====================

function checkSex() {
        
  //----- 'Неправильные'
  
  $nM = countWrongSex( "idMan",  "M" );
  $nF = countWrongSex( "idLady", "F" );
  $nN = countNeutral();
  
  echo "Неправильные значения для $nM партнеров и $nF партнерш, $nN без указания пола<br>"; 
  
  if( $nM || $nF || $nN ) {
    echo "Автоматическое исправление...<br>"; 
    repairWrongSex( "idMan",  "M" );
    repairWrongSex( "idLady", "F" );
    
    $nM = countWrongSex( "idMan",  "M" );
    $nF = countWrongSex( "idLady", "F" );
    $nN = countNeutral();
  
    echo "Результат:<br>"; 
    if( $nM || $nF || $nN ) {
      echo "Неправильные значения для $nM партнеров и $nF партнерш, $nN без указания пола<p>"; 
      
      $rows = loadNeutral();
      echo "<form action='baseToolCheckSex.php' method='POST'>"; 
      for( $k=0; $k<count($rows); $k++ ) {
        $row = $rows[$k];
        $person = new DbPersonProducer($row);
        $editLink = $person->makeEditLink();
        echo "$editLink "
            ."<input type='checkbox' name='chk_$row->id'>"
            ."<A href='db.php?id=p$row->id'>$row->surname $row->name</a><br>";
      }
      echo "<p>"; 
      echo "<input type='submit' name='setMales' value='make М'>";
      echo "&nbsp;"; 
      echo "<input type='submit' name='setFemales' value='make Ж'>";
      echo "</form>"; 
    }
    else {
      echo "- OK!<br>"; 
    }
    
  }
  
  echo "<p>";

  //----- 'Гермафродиты'
  $sql = "SELECT DISTINCT c1.idMan "
        ."FROM db_couples as c1 INNER JOIN db_couples as c2 ON c1.idMan=c2.idLady "
        ."INNER JOIN db_person as p ON c1.idMan=p.id";
  
  $idsMix = MotlDB::queryValues( $sql );
  $nMix = count($idsMix);
  if( count($idsMix) ) {
    echo "Противоречивая информация о следующих $nMix танцорах:<br>";
    for( $k=0; $k<count($idsMix); $k++ ) {
      $id = $idsMix[$k];
      $title = MotlDB::queryValue( "SELECT concat_ws(' ',surname,name) FROM db_person WHERE id=".MRES($id), null );
      echo "<A href='db.php?id=p$id'>$title&nbsp;</a><br>";
    }
  }

  echo "<A href='base.php?id=tools'>вернуться</a>";
}

//---------------------

function countNeutral() {
  $sql = "SELECT count(*) FROM db_person  "
        ."WHERE sex Is Null OR sex not in('M','F')";
  $n = MotlDB::queryValue( $sql );
  return $n;
}

//---------------------

function loadNeutral() {
  $sql = "SELECT * FROM db_person "
        ."WHERE sex Is Null OR sex not in('M','F') "
        ."ORDER BY name";
  return MotlDB::queryRows( $sql );
}

//---------------------

function countWrongSex( $field, $goodValue ) {
  ASSERT_name($field);
  
  $sql = "SELECT DISTINCT db_person.id FROM db_couples INNER JOIN db_person ON db_couples.$field=db_person.id "
        ."WHERE sex Is Null OR sex<>'".MRES($goodValue)."'";
  $ids = MotlDB::queryValues( $sql );
  return count($ids);
}

//---------------------

function repairWrongSex( $field, $goodValue ) {
  ASSERT_name($field);
  
  $sql = "UPDATE db_couples INNER JOIN db_person ON db_couples.$field=db_person.id SET sex='".MRES($goodValue)."'";
 // echo "$sql<p>";
  MotlDB::query( $sql );
}

//=====================
?>
