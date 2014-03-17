<?php
require_once "_core.inc";
require_once "_motl_db.inc";
//=====================
/*
	id
	hitDay
	hitTime
	script
	idparam
	ip
	idUser
----
SELECT DISTINCT hitDay FROM logtable
UPDATE logtable SET hitDay=substr(hitTime,1,10)
SELECT count(*) FROM (SELECT distinct ip,idUser FROM logtable where hitDay='2009-10-26') as dayUsers
----
*/

class LogDbContent extends Producer {

  //================

  function produce() {
    $day  = Motl::getParameter( "day" );
    $user = Motl::getParameter( "user" );
    $page = Motl::getParameter( "page" );

//    $list = Motl::getParameter( "day" );

    $s = "";

    if( !$day && !$user && !$page ) {

      $s .= $this->makeListDays();

    }
    else 
    if( $day && !$user && !$page ) {

      $users = $this->makeListDayUsers( $day );
      $pages = $this->makeListDayPages( $day );

      $s .= "<table><tr valign=top><td style='padding-left:20'>";
      $s .= "<h2>By user</h2><p>$users";
      $s .= "</td><td style='padding-left:40'>";
      $s .= "<h2>By page</h2><p>$pages";
      $s .= "</td></tr></table>";

    }
    else 
    if( $day && $user && !$page ) {

      $s .= $this->makeListDayUserPages( $day, $user );

    }
    else 
    if( $day && !$user && $page ) {

      $s .= $this->makeListDayPageUsers( $day, $page );

    }

    return $s;

//    return $this->produceRoot();
  }

  //================

  function makeListDays() {

    $script = $_SERVER['PHP_SELF'];

    $rows = MotlDB::queryRows( "SELECT DISTINCT hitDay FROM logTable" );

    $s = "<table border=1 cellspacing=0>\n";

    $s .= "<tr> <td class='HeaderCell'> Day </td>"
                ."<td class='HeaderCell'> nUsers </td>"
                ."<td class='HeaderCell'> nPages </td>"
         ."</tr>\n";

    for( $k=0; $k<count($rows); $k++ ) {

      $day = $rows[$k]->hitDay;
      $label = $day;

      $nUsers = MotlDB::queryValue( "SELECT count(*) FROM (SELECT distinct ip,idUser FROM logtable where hitDay='$day') as dayUsers", null );  // FLD $day date
      $nPages = MotlDB::queryValue( "SELECT count(*) FROM (SELECT distinct script,idparam FROM logtable where hitDay='$day') as dayUsers", null );  // FLD $day date

      $s .= "<tr>"
                ."<td class='DataCell'> <A href=$script?day=$day>$label</a> </td>"
                ."<td class='DataCell'> $nUsers </td>"
                ."<td class='DataCell'> $nPages </td>"
           ."</tr>\n";
    }

    $s .= "</table>\n";

    return $s;
  }

  //================

  function makeListDayUsers( $day ) {

    $script = $_SERVER['PHP_SELF'];

    $rows = MotlDB::queryRows( "SELECT DISTINCT ip,idUser FROM logTable WHERE hitDay='".MRES($day)."' ORDER BY ip,idUser" );  

    $s = "<table border=1 cellspacing=0>\n";

    $s .= "<tr> <td class='HeaderCell'> User </td>"
                ."<td class='HeaderCell'> nPages </td>"
         ."</tr>\n";

    for( $k=0; $k<count($rows); $k++ ) {

      $ip     = $rows[$k]->ip;
      $idUser = (int)$rows[$k]->idUser;

      $label = "$ip:$idUser";

      $acond = array();
      $acond[] = "ip='$ip'";
      $acond[] = $idUser ? "idUser='$idUser'" : "idUser Is Null";
      $cond = implode( " and ", $acond );

      // COD $cond
      $nPages = MotlDB::queryValue( "SELECT count(*) FROM (SELECT distinct script,idparam FROM logtable where hitDay='".MRES($day)."' and $cond) as dayUserPages", null );  

      $s .= "<tr>"
                ."<td class='DataCell'> <A href=$script?day=$day&user=$ip:$idUser>$label</a> </td>"
                ."<td class='DataCell'> $nPages </td>"
           ."</tr>\n";
    }

    $s .= "</table>\n";

    return $s;
  }

  //================

  function makeListDayPages( $day ) {

    $script0 = $_SERVER['PHP_SELF'];

    $rows = MotlDB::queryRows( "SELECT DISTINCT script,idparam FROM logTable WHERE hitDay".MRES($day)."' ORDER BY script,idparam" );  

    $s = "<table border=1 cellspacing=0>\n";

    $s .= "<tr> <td class='HeaderCell'> Page </td>"
                ."<td class='HeaderCell'> nUsers </td>"
         ."</tr>\n";

    for( $k=0; $k<count($rows); $k++ ) {

      $script  = $rows[$k]->script;
      $idparam = $rows[$k]->idparam;

      $label = "$script:$idparam";

      $acond = array();
      $acond[] = "script='".MRES($script)."'";
      $acond[] = $idparam ? "idparam='".MRES($idparam)."'" : "idparam Is Null";
      $cond = implode( " and ", $acond );

      $nUsers = MotlDB::queryValue( "SELECT count(*) FROM (SELECT distinct ip,idUser FROM logtable where hitDay='".MRES($day)."' and $cond) as dayUserPages", null );  // COD $cond

      $s .= "<tr>"
                ."<td class='DataCell'> <A title='qqq' href=$script0?day=$day&page=$script:$idparam>$label</a> </td>"
                ."<td class='DataCell'> $nUsers </td>"
           ."</tr>\n";
    }

    $s .= "</table>\n";

    return $s;
  }

  //================

  function makeListDayUserPages( $day, $user ) {

    $script0 = $_SERVER['PHP_SELF'];

    $acond = array();

    $acond[] = "hitDay='$day'";

    if( $user ) {
      $auser = explode( ":", $user );
      $acond[] = "ip='".MRES($auser[0])."'";
      $acond[] = $auser[1] ? "idUser='".MRES($auser[1])."'" : "idUser Is Null";
    }

    $cond = implode( " and ", $acond );

    $rows = MotlDB::queryRows( "SELECT DISTINCT substr(hitTime,12) as hitTime, script,idparam FROM logTable WHERE $cond ORDER BY hitTime" );  // COD $cond

    $s = "<table border=1 cellspacing=0>\n";

    $s .= "<tr> "
               ."<td class='HeaderCell'> Time </td>"
               ."<td class='HeaderCell'> Page </td>"
         ."</tr>\n";

    for( $k=0; $k<count($rows); $k++ ) {

      $hitTime = $rows[$k]->hitTime;
      $script  = $rows[$k]->script;
      $idparam = $rows[$k]->idparam;

      $label = "$script:$idparam";

      $s .= "<tr>"
                ."<td class='DataCell'> $hitTime </td>"
                ."<td class='DataCell'> $label </td>"
           ."</tr>\n";
    }

    $s .= "</table>\n";

    return $s;
  }

  //================

  function makeListDayPageUsers( $day, $page ) {

    $script0 = $_SERVER['PHP_SELF'];

    $acond = array();

    $acond[] = "hitDay='$day'";

    if( $page ) {
      $apage = explode( ":", $page );
      $acond[] = "script='".MRES($apage[0])."'";
      $acond[] = $apage[1] ? "idparam='".MRES($apage[1])."'" : "idparam Is Null";
    }

    $cond = implode( " and ", $acond );

    $rows = MotlDB::queryRows( "SELECT DISTINCT substr(hitTime,12) as hitTime, ip,idUser FROM logTable WHERE $cond ORDER BY hitTime" );  // COD $cond

    $s = "<table border=1 cellspacing=0>\n";

    $s .= "<tr> "
               ."<td class='HeaderCell'> Time </td>"
               ."<td class='HeaderCell'> User </td>"
         ."</tr>\n";

    for( $k=0; $k<count($rows); $k++ ) {

      $hitTime = $rows[$k]->hitTime;

      $ip     = $rows[$k]->ip;
      $idUser = $rows[$k]->idUser;

      $label = "$ip:$idUser";
	  if( $idUser ) {
	    $label .= $this->makeUserLabel( $idUser );
	  }

      $s .= "<tr>"
                ."<td class='DataCell'> $hitTime </td>"
                ."<td class='DataCell'> $label </td>"
           ."</tr>\n";
    }

    $s .= "</table>\n";

    return $s;
  }

  //================

  function makeList( $day, $user, $page, $list ) {
    //-----
    $aconds  = array();
    //-----

    switch( $list ) {
      case 'days': $fields = "hitDay"; break;
    }

    //-----

    // COD  $fields, $aconds
    $sql = "SELECT DISTINCT $fields FROM logTable";
    if( count($aconds) ) {
      $sql .= " WHERE " . implode( ' and ', $aconds );
    }

    $rows = MotlDB::queryValues(  $sql );  
  }


  //================

  private $usersById = null;
  
  function makeUserLabel( $uid ) {
    if( ! $usersById ) {
	  $usersById = array();
	  $rows = MotlDB::queryRows( "SELECT id,login FROM users" );
	  for( $k=0; $k<count($rows); $k++ ) {
	    $row = $rows[$k];
	    $usersById[ $row->id ] = $row->login;
	  }
	}
	
	if( isset($usersById[$uid]) ) {
	  $login = $usersById[$uid];
	  return " (<A href='profile.php?id=$uid'>$login</a>)";
	}
	
	return "";
  }
  
  //================
}

//=====================
//002184
?>
