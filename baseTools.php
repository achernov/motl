<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseContent.inc";

//=====================

header( "Content-Type: text/html; charset=utf-8" );

echo "<link href='motl.css' type='text/css' rel='stylesheet'><body style='padding-left:10;padding-top:15'>";
echo "<script src='baseTools.js'></script>\n";

MotlDB::connect();

//=====================
//print_r( $_REQUEST ); echo "<p>";//die();
//=====================

$manNames = array(
	"denis", "dmitro", "dmytro", "gaetano", "jurij", "kirill", "ryslan", "vadym", "yaroslav",
	"александр", "алексей", "али", "анатолий", "андрей", "андрий", "антон", "антэ", "арарат", 
	"арсений", "артем", "артём", "артемий", "артур", "архип", "астемир", "астимир", "аяз", 
	"бессо", "богдан", "борис", "вадим", "валентин", "валерий", "василий", "виктор", "виталий",
	"владимир", "владислав", "всеволод", "вячеслав", "гайк", "гарри", "геннадий", "георгий", 
	"гера", "герман", "глеб", "гоша", "григорий", "даврон", "дамир", "даниил", "данила", "демид",
	"демьян", "денис", "дима", "дионис", "дмитрий", "дмитро", "добрыня", "дюшан", "евгений", 
	"егор", "ефим", "захар", "иван", "игнат", "игнатий", "игорь", "ильдар", "илья", "исламбек", 
	"кирилл", "константин", "лев", "леон", "леонид", "макар", "макс", "максим", "марат", "марк",
	"марко", "матвей", "мирослав", "михаил", "мурат", "мухриддин", "никита", "николай", "олег", 
	"павел", "петр", "пётр", "рауль", "рафаэль", "родион", "рома", "роман", "ростислав", "руслан",
	"рустам", "святослав", "семен", "семён", "сергей", "славик", "станислав", "стас", "степан", 
	"стивен", "тагир", "тенгиз", "тимофей", "тимур", "федор", "фёдор", "феруз", "шарон", "эдуард", 
	"эльдар", "эмиль", "юлиан", "юрий", "яник", "ярослав",
	"david", "ilya", "jack", "jerry", "konstantin", "luca", "max", "авенир", "авет", "адам", 
	"акаки", "альберт", "амир", "аркадий", "вагит", "вахтанг", "влад", "гусейн", "давид", "данил", 
	"даниэль", "даня", "елисей", "кирюша", "максимилиан", "марсель", "мирон", "митя", "муслим", 
	"нарек", "савелий", "салмерон", "саша", "саша", "сероб", "филипп", "эмин", "яков", "ян"
	
);
//$manNames = array(1,2);

//---------------------

$womanNames = array(
	"anna", "caterina", "ela", "elena", "elvira", "emanuela", "jagoda", "nataliya", "olesia", 
	"vika", "аделина", "адель", "аделя", "айгуль", "айка", "айсылу", "алевтина", "александра", "алена", "алёна", "алеся", "алика", 
	"алина", "алиса", "алисия", "алла", "алсу", "альбина", "амалия", "амина", "амира", "анастасия", "ангелина", "анжелика", "анна", 
	"анна николь", "антонина", "аня", "арина", "ася", "белла", "валентина", "валерия", "варвара", 
	"василиса", "венера", "вера", "вероника", "виана", "вика", "виктория", "виолетта", "виталия", "влада", "владлена", "галина",
	"галия", "галя", "гуля", "дарина", "дария", "дарья", "даша", "диана", "диляра", "дина", "дуняша",
	"дуся", "ева", "евгения", "екатерина", "елена", "елизавета", "жанна", "желана", "злата", "илона",
	"инга", "инна", "ирина", "карина", "карияна", "катарина", "катя", "кира", "кристина", 
	"ксения", "лада", "лариса", "лаура", "лейла", "лена", "лиана", "лидия", "лиза", "лилия",
	"лолита", "любовь", "людмила", "майя", "маншук", "маргарита", "мариам", "марина", "мария",
	"марта", "маруся", "марьям", "маша", "мелине", "мерине", "милана", "милена", "надежда",
	"надя", "наира", "настя", "натали", "наталия", "наталья", "неля", "нигина", "ника",
	"нина", "нино", "оксана", "олеся", "ольга", "патрисия", "полина", "сабрина", "светлана",
	"севда", "селена", "серафима", "соня", "софия", "софья", "таисия", "тамара", "татьяна",
	"тона", "ульяна", "феличия", "феона", "эвелина", "элина", "эльвира",
	"эльмира", "этель", "юлиана", "юлианна", "юлия", "юля", "яна", "ярослава",
	"alena", "ekaterina", "tatyana", "айназ", "анна-мария", "аполинария", "астхик", "владислава", 
	"даяна", "долли", "ильмира", "ильсина", "индира", "ирма", "каролина", "лида", "линда", 
	"мадина", "мэрим", "наргиз", "оля", "раиса", "роберта", "снежана", "стефания", "сюзанна", 
	"тамрико", "элеонора"
	);

//=====================

if( isset($_REQUEST['deletePersons']) ) {
  deletePersonsAsk();
}
else
if( isset($_REQUEST['deletePersonsConfirmed']) ) {
  deletePersonsFinish();
}
if( isset($_REQUEST['deleteCouples']) ) {
  deleteCouplesAsk();
}
else
if( isset($_REQUEST['deleteCouplesConfirmed']) ) {
  deleteCouplesFinish();
}
else 
if( isset($_REQUEST['mergePersons']) ) {
  mergePersons();
}
else
if( isset($_REQUEST['mergePersonsConfirmed']) ) {
  mergePersonsFinish();
}
else
if( isset($_REQUEST['mergeCouples']) &&  Motl::getParameter( "confirmed" ) ) {
  mergeCouplesFinish();
}
else
if( isset($_REQUEST['checkDupCouples']) ) {
  checkDupCouples();
}
else
if( isset($_REQUEST['checkNames']) ) {
  checkNames();
}
else
if( isset($_REQUEST['byName']) ) {
  listByName();
}

//=====================

function deletePersonsAsk() {
  $selids = getSelectedIds();
  $aselids = implode( ",", $selids );
  ASSERT_ints($aselids);

  $prototype = new DbPersonProducer();
  
  // ASS $aselids
  $persons = $prototype->loadMultipleFromDB( "id in ($aselids)", "surname,name" );
  
  $spersons = "";
  for( $k=0; $k<count($persons); $k++ ) {
    $spersons .= $persons[$k]->makeText( $persons[$k]->row, 
                      "<A target=_new href=db.php?id=p[plain|id]>'[plain|surname] [plain|name]'</a> <br>" );
  }
  
  echo "<b>Вы уверены, что хотите удалить эти записи?</b><br>";
  echo $spersons."<p>";
  echo "- <A href='baseTools.php?deletePersonsConfirmed=1&ids=$aselids'>УДАЛИТЬ!</a><p>";
  echo "- <A href='base.php?id=tools'>вернуться</a>";
  
}

//=====================

function deletePersonsFinish() {
  $sids = Motl::getParameter( "ids" );
  ASSERT_ints($sids);  
  
  // ASS $sids
  MotlDB::query( "DELETE FROM db_person WHERE id in($sids)" );
  
  echo "Записи удалены<p>";
  echo "<A href='base.php?id=tools'>вернуться</a>";
}

//=====================

function deleteCouplesAsk() {

  $selids = getSelectedIds();
  $aselids = implode( ",", $selids );
//die($aselids);
  $scouples = BaseContent::makeCouplesTable( $aselids );
  
  echo "<b>Вы уверены, что хотите удалить эти записи?</b><br>";
  echo $scouples."<p>";
  echo "- <A href='baseTools.php?deleteCouplesConfirmed=1&ids=$aselids'>УДАЛИТЬ!</a><p>";
  echo "- <A href='base.php?id=tools'>вернуться</a>";
  
}

//=====================

function deleteCouplesFinish() {
  $sids = Motl::getParameter( "ids" );
  ASSERT_ints($sids);  
  
  // ASS $sids
  MotlDB::query( "DELETE FROM db_couples WHERE id in($sids)" ); 
  
  echo "Записи удалены<p>";
  echo "<A href='base.php?id=tools'>вернуться</a>";
}

//=====================

function mergePersons() {

  if( ! isset($_REQUEST['dbf_id']) && ! isset($_REQUEST['ids']) ) {
	echo "<form method=POST>";
	echo "<b>ids:</b><br>";
	echo "<textarea name='ids'></textarea><p>";
	echo "<input type='submit' value='Объединить...'>";
	echo "</form>";
	return;
  }

  $selids = getSelectedIds();
  if( $selids == null ) {
	$selids = getListedIds();
	  if( $selids == null ) {
		die('qwe');
	  }
  }
  
  $aselids = implode( ",", $selids );
  ASSERT_ints($aselids);  
  
  $prototype = new DbPersonProducer();
  
  // ASS $aselids
  $persons = $prototype->loadMultipleFromDB( "id in ($aselids)" );
  
  $spersons = "";
  for( $k=0; $k<count($persons); $k++ ) {
    $selected = ($k==0) ? "checked" : "";
    $spersons .= $persons[$k]->makeText( $persons[$k]->row, 
                      "<tr> <td><input type=radio $selected name=idMain value='[plain|id]'></td> <td>[editbar|edit]&nbsp;<A href=db.php?id=p[plain|id]>[plain|id]</a></td> <td>[plain|surname]</td> <td>[plain|name]</td> </td>" );
  }
  $spersons = "<table border=1 cellspacing=0 cellpadding=3>$spersons</table>";
  
  echo "<form action='baseTools.php' method='POST'>\n";
  echo "<input type=hidden name=ids value='$aselids'>\n";
  echo "Вы уверены, что это один и тот же человек и Вы хотите объединить эти записи?";
  echo $spersons;
  echo "<input type='submit' name='mergePersonsConfirmed' value='Объединить'>\n";
  echo "</form>\n";
  //print_r($persons);
}

//---------------------

function mergePersonsFinish() {
  $sids = Motl::getParameter( "ids" );
  $ids = explode( ",", $sids );
  $idMain = Motl::getParameter( "idMain" );
  ASSERT_int($idMain);
  
  $nIds = count($ids);
  $delIds = array();
  for( $k=0; $k<count($ids); $k++ ) {
    if( $ids[$k] != $idMain ) {
      $delIds[] = $ids[$k];
    }
  }
  $sDelIds = implode( ",", $delIds );
  ASSERT_ints($sDelIds);
  
  $testMain = MotlDB::queryValue( "SELECT count(*) FROM db_person WHERE id='".MRES($idMain)."'", 0 );
  
  // ASS $sDelIds
  $testDel  = MotlDB::queryValue( "SELECT count(*) FROM db_person WHERE id in($sDelIds)", 0 );
  
  if( $testMain!=1 || $testDel!=($nIds-1) ) {
    die( "error" );
  }

//  $simulation = true;
  $simulation = false;
  
  mergeInField( $simulation, "db_couples", "idMan",  $idMain, $sDelIds ); // SqlSafe
  mergeInField( $simulation, "db_couples", "idLady", $idMain, $sDelIds ); // SqlSafe 
  
  mergeInList( $simulation, "db_person",  "teacher", $idMain, $delIds );
  mergeInList( $simulation, "db_couples", "teacher", $idMain, $delIds );

  // ASS $idMain,$sDelIds
  $sqlMerge  = "UPDATE db_couple_event SET idSolo=$idMain WHERE idSolo In($sDelIds)";

  // ASS $sDelIds
  $sqlDelete = "DELETE FROM db_person WHERE id In($sDelIds)";
  echo "$sqlMerge<p>$sqlDelete<p>";
  
  if( ! $simulation ) {
    MotlDB::query( $sqlMerge );
    MotlDB::query( $sqlDelete );
  }
  
  if( ! $simulation ) {
    checkDupCouples();
  }
  
  echo "Persons №№ $sDelIds merged to <A href='db.php?id=p$idMain'>$idMain</a>";
}

//=====================
// SQLPARAM $newId, $sOldIds
function mergeInField( $simulation, $table, $field, $newId, $sOldIds ) {
  
  // COD:PAR  $table, $field
  // PAR $newId, $sOldIds
  $sqlMerge  = "UPDATE $table SET $field=$newId WHERE $field In($sOldIds)";
  
  echo "$sqlMerge<p>";
  if( ! $simulation ) {
    MotlDB::query( $sqlMerge );
  }
}

//=====================

function mergeInList( $simulation, $table, $field, $newId, $oldIds ) {
  foreach( $oldIds as $oid ) {
    ASSERT_int($oid);
    
    // COD:PAR $table, $field
    // ASS $oid
    $sqlSelect  = "SELECT DISTINCT $field FROM $table WHERE CONCAT(',',$field,',') Like '%,$oid,%'";
    
    $values = MotlDB::queryValues( $sqlSelect );
    
    echo "$sqlSelect<p>";
    foreach( $values as $value ) {
      $tmp = str_replace( ",$oid,",  ",$newId,",  ",$value," );
      $newValue = substr( $tmp, 1, strlen($tmp)-2 );
      
      // COD:PAR $table, $field
      $sqlMerge  = "UPDATE $table SET $field='".MRES($newValue)."' WHERE $field='".MRES($value)."'";
      
      echo "$sqlMerge<p>";
      if( ! $simulation ) {
        MotlDB::query( $sqlMerge ); 
      }
    }
  }
}

//=====================

function checkNames() {
global $manNames, $womanNames;

  trimNames();
  
  echo "<A href='base.php?id=tools'>назад</a><p>";

  echo "<b>Мужские имена:</b><br>";
  showNamesBySex( 'M', $manNames, true );

  echo "<p>";
  
  echo "<b>Женские имена:</b><br>";
  showNamesBySex( 'F', $womanNames, true );
}

//---------------------

function trimNames() {
  MotlDB::query( "UPDATE db_person SET surname=trim(surname), name=trim(name) "
                ."WHERE surname like '% %' or  name like '% %'" );
}

//---------------------
// SQLPARAM $sex
function showNamesBySex( $sex, $knownNames, $skipKnown=false ) {

  $names = MotlDB::queryValues( "SELECT DISTINCT binary name FROM db_person WHERE sex='$sex' ORDER BY name" );
  
  for( $k=0; $k<count($names); $k++ ) {
    $name = $names[$k];
	//print_r($knownNames);die();
	$lname = mb_strtolower($name,"utf-8");
	$known = in_array($lname,$knownNames);
	if( $known && $skipKnown ) {
		continue;
	}
	$style = $known ? "" : "style='color:red'";
	$qpname = QPUTF8( $name );
    echo "<A $style href='baseTools.php?byName=$qpname&sex=$sex'>$name</a><br>";
  }
  
}

//=====================

function QPUTF8( $str ) {
	$hex = bin2hex ( $str );
	$s = "";
	for( $k=0; $k<strlen($hex); $k+=2 ) {
		$c2 = substr( $hex, $k, 2 );
		$s .= "%$c2";
	}

	return $s;
}

//=====================

function listByName() {
  $name = Motl::getParameter( "byName" );
  $sex  = Motl::getParameter( "sex" );

  echo "<A href='baseTools.php?checkNames=1'>назад</a><p>";
  
  echo BaseContent::makeDancersTable( "baseTools.php?byName=$name&sex=$sex", 
		"binary name='".MRES($name)."' AND sex='".MRES($sex)."'" );  
}

//=====================

function checkDupCouples() {

  $rows = MotlDB::queryRows( "SELECT idMan,idLady FROM db_couples GROUP BY idMan,idLady HAVING count(*)>1" );
  $nrows = count($rows);

  echo "<b><u>Проверка пар-дублей - найдено $nrows</u></b><p>";

  for( $k=0; $k<count($rows); $k++ ) {
    $idMan  = $rows[$k]->idMan;
    $idLady = $rows[$k]->idLady;
    
    // FLD $idMan, $idLady
    $couples = MotlDB::queryValues( "SELECT id FROM db_couples WHERE idMan=$idMan and idLady=$idLady" ); 
    
    $idMain = array_shift( $couples );
    mergeCouple( $idMain, $couples );

    // FLD $idMan
    $titleMan  = MotlDB::queryValue( "SELECT concat_ws(' ',surname,name) FROM db_person WHERE id=$idMan", null );
    
    // FLD $idLady
    $titleLady = MotlDB::queryValue( "SELECT concat_ws(' ',surname,name) FROM db_person WHERE id=$idLady", null );
    
    $sids = implode( ',', $couples );
    echo "<b>$titleMan - $titleLady</b><br>";
    echo "id $sids удален(ы) пользу $idMain<p>";
  }
  
  echo "<A href='base.php?id=tools'>назад</a><br>";
}

//---------------------

function mergeCouple( $idMain, $delIds ) {

  ASSERT_int($idMain);

  $sDelIds = implode( ",", $delIds );
  ASSERT_ints($sDelIds);
  
  // ASS $idMain
  $testMain = MotlDB::queryValue( "SELECT count(*) FROM db_couples WHERE id=$idMain", 0 );

  // ASS $sDelIds
  $testDel  = MotlDB::queryValue( "SELECT count(*) FROM db_couples WHERE id in($sDelIds)", 0 );
  
  if( $testMain!=1 || $testDel!=count($delIds) ) {
    die( "error" );
  }

  // ASS $idMain, $sDelIds
  $sqlMerge  = "UPDATE db_couple_event SET idCouple=$idMain WHERE idCouple In($sDelIds)";

  // ASS $sDelIds
  $sqlDelete = "DELETE FROM db_couples WHERE id In($sDelIds)";

  MotlDB::query( $sqlMerge );
  MotlDB::query( $sqlDelete );

}

//=====================

function getListedIds() {
	if( ! isset($_REQUEST['ids']) ) {
		return null;
	}
	
	$ids = $_REQUEST['ids'];
	
	$ids1 = preg_replace( "/[^0-9]+/", " ", $ids );

	$ids2 = trim( $ids1 );
	
	return explode( " ", $ids2 );
}

//=====================

function getSelectedIds() {
  if( ! isset($_REQUEST['dbf_id']) ) {
	return null;
  }
  
  $ids     = $_REQUEST['dbf_id'];
  $selids = array();

  for( $k=0; $k<count($ids); $k++ ) {
    if( isset($_REQUEST["dbf_".$k."_check"]) ) {
      $selids[] = $ids[$k];
    }
  }
  return $selids;
}

//=====================
?>
