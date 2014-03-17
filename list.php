<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================
// Вальс,Танго,Венский вальс,Медленный фокстрот,Квикстеп
// Ча-ча-ча,Самба,Румба,Пасодобль,Джайв
header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

$id = Motl::getIntParameter( "id", 46 );

$compRows = MotlDB::queryRows( "SELECT * FROM db_events WHERE pid='$id'" );  // ASS $id

echo "<style>\n";
echo "table { border-collapse:collapse; border-left:1px solid black; border-top:1px solid black;}\n";
echo "td { border-right:1px solid black; border-bottom:1px solid black; padding:5; font-size:10; }\n";
echo "</style>\n";

for( $k=0; $k<count($compRows); $k++ ) {
  $title = $compRows[$k]->title;
  $cid = $compRows[$k]->id;
  echo "<b>$title</b><p>";

  // FLD $cid
  $cplRows = MotlDB::queryRows( "SELECT Event.*,"
                                ."Man.id as mid, Man.surname as msurname, Man.name as mname,"
                                ."Lady.id as fid, Lady.surname as fsurname,Lady.name as fname "
                            ."FROM ((db_couple_event AS Event INNER JOIN db_couples AS Couple ON Event.idCouple=Couple.id) "
                                ."INNER JOIN db_person AS Man ON Couple.idMan=Man.id) "
                                ."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
                            ."WHERE idEvent='$cid' ORDER BY msurname,mname" );
                            
  echo "<table width=1000 border=1 style='border-collapse:collapse;'  cellpadding=0 cellspacing=0>";
  for( $c=0; $c<count($cplRows); $c++ ) {
    $row = $cplRows[$c];

    echo "<tr>";
    echo "<td style='width:40' >&nbsp;</td>";
    echo "<td style='width:170'>$row->msurname $row->mname&nbsp;</td>";
    echo "<td style='width:170'>$row->fsurname $row->fname&nbsp;</td>";
    echo "<td style='width:180'>$row->raw_country, $row->raw_city&nbsp;</td>";
    echo "<td style='width:160'>$row->raw_club&nbsp;</td>";
    echo "<td tyle='width:150'>$row->raw_teacher&nbsp;</td>";
    echo "</tr>";
  }
  echo "</table>";
}

?>