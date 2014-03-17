<?php
require_once "_motl_db.inc";
require_once "_motl_user.inc";
require_once "baseColumn.inc";
require_once "dataUtil.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );
header( "Expires: Tue, 01 Jun 1999 09:17:32 GMT" );

MotlDB::connect();

$fieldName  = Motl::getParameter( "fieldName" );

$fieldValue = Motl::getParameter( "fieldValue" );
$fieldValue = unescape( $fieldValue );

switch( $fieldName ) {
  case 'msurname':
    respondCouple($fieldValue); 
    break;
    
  case 'surname':
    respondSolo($fieldValue); 
    break;
}

//=====================

function respondCouple( $surname ) {
  $maxRows = 10;
  $maxRows = 7;

  $sql = "SELECT Couple.id,Couple.country,Couple.city,Couple.club,Couple.teacher,"
            ."Man.id as mid, Man.surname as msurname, Man.name as mname, Man.birthDate as mdate,"
            ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname, Lady.birthDate as fdate "
        ."FROM db_couples AS Couple "
            ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
            ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
        ."WHERE Man.surname Like '".MRES($surname)."%' "
        ."ORDER BY msurname, mname, fsurname, fname";
        
  $rows = MotlDB::queryRows( $sql ); 
  
  if( count($rows) > $maxRows ) {
    //die();
  }
  

  $bcCountry = new BaseColumnCountry();
  $bcCity = new BaseColumnCity();
  $bcClub = new BaseColumnClub();
  $bcTeacher = new BaseColumnTeacher();
  
  //$out = array();
  $s = "";
  $rowsDone = 0;
  foreach( $rows as $row ) {
    //$outRow = array();
    $country = $bcCountry->sidsToSvalues( $row->country );
    $city =    $bcCity->sidsToSvalues( $row->city );
    $club =    $bcClub->sidsToSvalues( $row->club );
    $teacher = $bcTeacher->sidsToSvalues( $row->teacher );

    $msurname = filter( $row->msurname );
    $mname = filter( $row->mname );
    $mdate = DataUtil::dateDbToHumanNoTime( $row->mdate );

    $fsurname = filter( $row->fsurname );
    $fname = filter( $row->fname );
    $fdate = DataUtil::dateDbToHumanNoTime( $row->fdate );
 
    $country = filter( $country );
    $city = filter( $city );
    $club = filter( $club );
    $teacher = filter( $teacher );
    
    //-----
    
    $label   = "$msurname $mname - $fsurname $fname";
    $tip     = "$country, $city, $club, $teacher";
    
    $onclick = "fillFormCouple('$msurname','$mname','$mdate',"
                             ."'$fsurname','$fname','$fdate',"
                             ."'$country','$city','$club','$teacher');return false;";
    
    $s .= "<A href='#' class='fillLink' title='$tip' onclick=\"$onclick\">$label</a><p>";
	$rowsDone ++;
	if( $rowsDone >= $maxRows ) {
		$s .= "......";
		break;
	}
  }
  
  if( $s ) {
    $caption = "<div class='fillCaption'>Похожие пары:<br><font class='fillSubCaption'>(щелкните мышкой, чтобы заполнить форму)</font></div>";
    die( "<div class='fillFrame'>$caption$s</div>" );
  }
  else {
    die();
  }
  
}

//=====================

function respondSolo( $surname ) {
  $maxRows = 10;

  $sql = "SELECT Dancer.id, Dancer.surname, Dancer.name, Dancer.birthDate, "
        ."Dancer.country,Dancer.city,Dancer.club,Dancer.teacher "
        ."FROM db_person AS Dancer "
        ."WHERE Dancer.surname Like '".MRES($surname)."%' ";
  //     die($sql); 
  $rows = MotlDB::queryRows( $sql );
  
  if( count($rows) > $maxRows ) {
    die();
  }

  $bcCountry = new BaseColumnCountry();
  $bcCity = new BaseColumnCity();
  $bcClub = new BaseColumnClub();
  $bcTeacher = new BaseColumnTeacher();
  
  //$out = array();
  $s = "";
  foreach( $rows as $row ) {
    //$outRow = array();
    $country = $bcCountry->sidsToSvalues( $row->country );
    $city =    $bcCity->sidsToSvalues( $row->city );
    $club =    $bcClub->sidsToSvalues( $row->club );
    $teacher = $bcTeacher->sidsToSvalues( $row->teacher );

    $surname = filter( $row->surname );
    $name = filter( $row->name );
    $date = DataUtil::dateDbToHumanNoTime( $row->birthDate );
 
    $country = filter( $country );
    $city = filter( $city );
    $club = filter( $club );
    $teacher = filter( $teacher );
    
    //-----
    
    $label   = "$surname $name";
    $tip     = "$country, $city, $club, $teacher";
    
    $onclick = "fillFormSolo('$surname','$name','$date',"
                           ."'$country','$city','$club','$teacher');return false;";

    $s .= "<A href='#' class='fillLink' title='$tip' onclick=\"$onclick\">$label</a><p>";
  }
  
  if( $s ) {
    $caption = "<div class='fillCaption'>Похожие имена:<br><font class='fillSubCaption'>(щелкните мышкой, чтобы заполнить форму)</font></div>";
    die( "<div class='fillFrame'>$caption$s</div>" );
  }
  else {
    die();
  }
  
}

//=====================

function filter( $s ) {
  $s = str_replace( ":", "", $s );
  $s = str_replace( ";", "", $s );
  return $s;
}

//=====================
?>
