<?php
require_once "_motl_user.inc";
//=====================

class MiniConsole extends Producer {

  //---------------------

  public function produce() {

    $user = User::getCurrentUser();
    if( !$user ) {
      return "";
    }

    $admin  = $user->checkRight("admin");
    $editor = $user->checkRight("editor");

    if( !$admin && ! $editor ) {
      return "";
    }

    $s = "";

    $picfolder = Motl::getSysPicturesFolder();
    $s .= "<div class='MiniConsoleDiv'>\n";
    $s .= "<table class='MenuTable' border=0 cellpadding=0 cellspacing=0><tr><td>\n";
    $s .= "<tr><td background='$picfolder/whitePlashka1.png' style='font-family:arial;font-size:14;font-weight:bold;padding-left:30'>Admin console</td></tr>\n";
    $s .= "<tr><td  class='MiniConsoleDiv' bgcolor=white style='padding-left:10;padding-top:10'>\n";

    $edit = $this->makeEditSwitch();
	$sim  = $this->makeSimulateSwitch();
	$s .= "<table border=0 cellpadding=0><tr><td width=70>$edit</td>"
			."<td align=right>$sim</td></tr></table>";
    $s .= "<p>";

    $s .= "Log: &nbsp; <A class='editLink' href='logReader.php'>Apache</a>"
	         ." &nbsp; <A class='editLink' href='logReaderDB.php'>DB</a>"
	         ." &nbsp; <A class='editLink' href='logReaderErrors.php'>Errors</a><p>";
			 
    $s .= $this->makePageLogCmd();

    //$s .= "<A class='editLink' href='wap.php'>WAP version</a>";

    //$s .= "&nbsp;&nbsp;&nbsp;";

    $s .= "<A class='editLink' href='page.php?id=tmp'>tmp</a>";

    $s .= "</td></tr>\n";
    $s .= "<tr><td><img src='$picfolder/whitePlashka2.png'></td></tr>\n";
    $s .= "</table>\n";
    $s .= "</div>\n";

    return $s;
  }

  //---------------------
  
  function makePageLogCmd() {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );

    $day = date( "Y-m-d", time() );
    $url = "logReaderDB.php?day=$day&page=$script:$id";

    $label = "This page today";
	
	if( MotlDB::isConnected() ) {
  
	  $idCond = is_null($id) ? "idparam Is Null" : "idparam='".MRES($id)."'";
    // COD $day, $idCond
	  $sql = "SELECT count(*) FROM (SELECT DISTINCT ip FROM logtable WHERE hitDay='$day' and script='$script' and $idCond) as q";
    
	  //$label .= " $sql";
	  $cnt = MotlDB::queryValue( $sql, 0 );  
	  $label .= " <b>$cnt</b>";
	}

    return "<A class='editLink' href='$url'>$label</a><p>";
  }

  //---------------------

  function makeEditSwitch() {
  
    //$editmode = Motl::getCookieFlag( "editmode" );
	$editmode = Motl::getParameter( "editmode", 0 );
    
    if( $editmode ) {
      return "<A class='editLink' href='' onclick='setCookie(\"editmode\",0);return false;'>"
          ."end edit</a>";
    }
    else {
      return "<A class='editLink' href='' onclick='setCookie(\"editmode\",1);return false;'>"
          ."Edit</a>";
    }
  }

  //---------------------

  function makeSimulateSwitch() {
  
    //$editmode = Motl::getCookieFlag( "editmode" );
	$curSim = Motl::getParameter( "simuser", 0 ) ? 1 : 0;
	$newSim = $curSim ? 0 : 1;
    
	$cross = $curSim ? 'X' : '&nbsp;';
	$mark = "<font style='font-family:Courier New;font-weight:bold'>[$cross]</font>";
	
	$linkStyle = $curSim ? 'font-weight:bold' : '';
	
    return "<A class='editLink' style='$linkStyle' href='' "
			."onclick='setCookie(\"simuser\",$newSim);return false;'>"
          ."$mark &nbsp; Simulate user</a>";
  }

  //---------------------
}

//=====================
?>
