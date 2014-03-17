<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_dbproducer_factory.inc";
require_once "initEventData.inc";
//=====================
header( "Content-Type: text/html; charset=utf-8" );
//=====================

//$templates = array( $compCurrent, $compVita/*$compEkat, $compImperia*/ );

//=====================

$id = Motl::getParameter( "id" );
$returl  = Motl::getParameter( "returl" );
$variant = Motl::getParameter( "variant" );

echo filterForHtml( makeLinks( $id ) );

switch( Motl::getParameter( "action" ) ) {
	case 'init':
		MotlDB::connect();
		initEvent( $id );
		Motl::redirectToRetUrl();
		break;
		
	case 'schedule':
		MotlDB::connect();
		$schedule = makeSchedule( $templates[$variant] );
		echo filterForHtml( $schedule );
		break;
		
	case 'realschedule':
		MotlDB::connect();
		$schedule = makeRealSchedule( $id );
		echo filterForHtml( $schedule );
		break;
		
	case 'links':
		echo filterForHtml( makeLinks( $id ) );
		break;
		
	default:
		echo makeMainForm();
		break;
}


//=====================

function makeMainForm() {
	global $templatesBase;
	$id = Motl::getParameter( "id" );
	$returl  = Motl::getParameter( "returl" );
	
	$s = "";
    
	$hrefSchedule = "initEvent.php?action=realschedule&id=$id&returl=$returl";
    $s .= "<A style='color:green;font-weight:bold' href='$hrefSchedule'>real Schedule</a>";
    
    $s .= "<p>&nbsp;<p>";
    
	$s .= "<form action='initEvent.php'>";
	$s .= "<table border=1 cellspacing=0 cellpadding=10>";
    $s .= "<input type=hidden name='action' value='init'> </td>";
    $s .= "<input type=hidden name='id' value='$id'> </td>";
    $s .= "<input type=hidden name='returl' value='$returl'> </td>";
    for( $k=0; $k<count($templatesBase); $k++ ) {
        $s .= "<tr>\n";
        $s .= "<td> <input type=checkbox name=templateNumbers[] value='tchk$k'> </td>";
        $s .= "<td> " . showTemplate($templatesBase[$k]) . " </td>";
        $s .= "</tr>\n";
    }
	$s .= "</table>";
    $s .= "<input type=submit value='Init!'> </td>";
	$s .= "</form>";

/*	
	$s .= "<table>";
	foreach( $templates as $key=>$variant ) {
		$label = $key;
		if( $variant[0][0] = "info" ) {
			$label = $variant[0][2];
		}
		
		$confirmStatement = "return confirm(\"Все содержимое будет заменено! Вы уверены?\")";
		$hrefInit = "initEvent.php?action=init&id=$id&variant=$key&returl=$returl";
		$hrefSchedule = "initEvent.php?action=schedule&variant=$key&returl=$returl";
		
		$s .= "<tr>";
		$s .= "<td style='padding-right:50px;font-weight:bold'>$label</td>";
		$s .= "<td><A style='color:red;font-weight:bold' onclick='$confirmStatement' "
					."href='$hrefInit'>Init!</a></td>";
		$s .= "<td style='padding-left:50px;'>"
				."<A style='color:green;font-weight:bold' "
					."href='$hrefSchedule'>makeShedule</a></td>";
		$s .= "</tr>";
	}
	$s .= "</table>";
    $s .= "<p>";

	$hrefLinks = "initEvent.php?action=links&id=$id&returl=$returl";
    $s .= "<A style='color:green;font-weight:bold' href='$hrefLinks'>makeLinks</a><p>";

	$hrefSchedule = "initEvent.php?action=realschedule&id=$id&returl=$returl";
    $s .= "<A style='color:green;font-weight:bold' href='$hrefSchedule'>real Schedule</a><p>";
*/
	return $s;
}

//=====================

function showTemplate( $template ) {
    $s = "";
    for( $k=0; $k<count($template); $k++ ) {
        $row = $template[$k];
        
        switch( $row[0].$row[1] ) {
        
            case 'separatorB':
                $s .= "<b><big>$row[2]</big></b><br>";
                break;
                
            case 'separatorC':
                $s .= "<b><i>$row[2]</i></b><br>";
                break;
                
            default:
                $s .= "$row[2]<br>";
                break;
        }
    }
    return $s;
}

//=====================

function initEvent( $id ) {
    global $templatesBase;
    //print_r( $_REQUEST); die();


  ASSERT_int( $id );

  MotlDB::query( "DELETE FROM db_events WHERE pid='$id'" );  // ASS $id

    $position = 0;
  
//	foreach( $template as $item ) {
	for( $tk=0; $tk<count($templatesBase); $tk++ ) {
        
        $template = $templatesBase[$tk];
        //print_r($template); die();
        for( $k=0; $k<count($template); $k++ ) {
            $item = $template[$k];
            $type  = $item[0];
            $code  = $item[1];
            $title = $item[2];
            
            // ASS $id
            // CODE $k
            // COD:PAR $type, $code, $title
            switch( $type ) {
                case "separator":
                    $level = $code ? "'$code'" : "null";
                    $sql = "INSERT INTO db_events( pid, position, type, ratingLevel, title ) "
                        ."VALUES( '$id', $position, 'separator', $level, '$title' )";
                    $position ++;
                    //die( $sql );
                    MotlDB::query( $sql );  
                break;
                
                case "info":
                case "reg":
                    // Just skip it
                    break;
                    
                default:
                    $sql = "INSERT INTO db_events( pid, position, type, regType, groupCode, title ) "
                        ."VALUES( '$id', $position, 'competition', '$type', '$code', '$title' )";
                    //die( $sql );
                    $position ++;
                    MotlDB::query( $sql );  
                    break;
            }
          
        }
    }
}

//=====================

function filterForHtml( $scheduleText ) {
	$s = htmlspecialchars( $scheduleText );
	$s = str_replace( "\n", "<br>", $s );
	return $s;
}

//=====================

function makeLinks( $id ) {
	$s = "";
	$s .= makeLink( $id, 'f', 'Регистрация' );
	$s .= makeLink( $id, 'c', 'Список участников' );
	$s .= makeLink( $id, 's', 'Расписание' );
	$s .= makeLink( $id, 'w', 'Как проехать' );
	$s .= makeLink( $id, 'p', 'Наградной материал' );
	$s .= makeLink( $id, 'r', 'Результаты турнира' );
	return $s;
}

//-------

function makeLink( $id, $letter, $title ) {
	return "<A class='redLink' href='db.php?id=e$id,$letter'>$title</a><br>\n";
}

//=====================

function makeRealSchedule( $id ) {
	$s = "";
    
	$s .= "<style>
.partHH { font-size:larger; font-weight:bold }
.partH { margin-top:5; margin-bottom:3; margin-left:0; text-decoration:underline; }
.partB { margin-top:0; margin-bottom:10; margin-left:20; }
</style>\n\n";

    $rows = MotlDB::queryRows( "SELECT type,ratingLevel,title FROM db_events WHERE pid='$id' ORDER BY position" );
    foreach( $rows as $row ) {
        $type = $row->type;
        $level = $row->ratingLevel;
        $title = $row->title;
        
        switch( "$type$level" ) {
        
            case 'separatorA': 
                $s .= "\n<p class=partHHH>$title</p>\n\n"; break;
                
            case 'separatorB': 
                $s .= "\n<p class=partHH>$title</p>\n\n"; break;
                
            case 'separatorC': 
                $s .= "\n<p class=partH>$title</p>\n\n"; break;
            
            case 'competition': 
            case 'competitionA': 
            case 'competitionB': 
            case 'competitionC': 
                $s .= "<p class=partB>$title</p>\n"; break;
                
            default: 
                $s .= "<p>$title</p>\n"; break;
        }
    }
  
    return $s;
}

//=====================

function makeSchedule( $template ) {
	$s = "";
	
	$s = "<style>
.partH { margin-top:5; margin-bottom:3; margin-left:0; text-decoration:underline; }
.partB { margin-top:0; margin-bottom:10; margin-left:20; }
</style>";
	
	foreach( $template as $item ) {
		$type  = $item[0];
		$code  = $item[1];
		$label = $item[2];
		
		switch( $type ) {
			case 'separator':
				if( $code == 'C' ) {
					$s .= "<p class=partH>$label</p>\n";
				}
				else {
					$s .= "\n<big><b>$label</b></big><br><br>\n\n";
				}
				break;
				
			case 'reg':
				$s .= "<p class=partH>$label</p>\n";
				break;
				
			case 'solo':
			case 'solocomp':
			case 'couple':
				$s .= "<p class=partB>$label</p>\n";
				break;
		}
	}
	
	return $s;
}

//=====================
?>
