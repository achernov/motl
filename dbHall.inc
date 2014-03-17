<?php
//=====================

class DbHallProducerFull extends DbHallProducer {
  //----------------

  public function produceFull() {
    $id = $this->row->id;
    ASSERT_int($id);
    
    $s = "";
    
    //===== Show Common Data
    $name = $this->row->name;
    $title = $this->row->title;
    $s .= "<h1>База данных: зал - $title</h1>";

    if( Motl::getParameter( "editmode" ) ) {
      $s .= $this->makeEditLink() . "<p>";
    }
    
    $s .= $this->row->info;

    //===== End Show Common Data

    //===== Show Competitions
    
    $s .= "<p style='margin-bottom:5'><b><i>Соревнования:</i></b></p>";
    
    // ASS $id
    $sql = "SELECT id,date,date1,title, organizer, (regOpen OR haveScores OR havePlaces) AS NotEmpty "
          ."FROM db_events WHERE idPlace=$id";
    $events = MotlDB::queryRows( $sql );  
    
    $s .= "<table border=1 cellspacing=0 cellpadding=5 class=Calendar>";
    
    $s .= "<tr class=CalendarTop> <td width=100>Дата</td>"
//                    ."<td>Название турнира<br>Статус турнира</td>"
                    ."<td>Название турнира<br><img src='' width=300 height=0></td>"
                    ."<td width=240>Организатор</td>"
               ."</tr>";
    
    $template = "<tr> <td>[editbar|edit| ][plainDateRange|date|date1]</td>"
//                    ."<td>[if|NotEmpty|<A href='db.php?id=e[plain|id]'>]<i>[plain|title]</i>[if|NotEmpty|</a>]<br>[plain|status]</td>"
                    ."<td>[if|NotEmpty|<A href='db.php?id=e[plain|id]'>][plain|title][if|NotEmpty|</a>]</td>"
                    ."<td>[plainBB|organizer]&nbsp;</td>"
               ."</tr>";
    
    foreach( $events as $row ) {
      $prototype = new DbEventProducer($row);
      $s .= $prototype->makeText( $row, $template );
    }
    $s .= "</table>";
    
    //===== End Show Competitions

    $s .= "&nbsp;<p>";

    return $s;
  }

  //----------------
}

//=====================
?>
