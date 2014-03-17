<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "_motl_db.inc";
require_once "baseUtils.inc";
//=====================

header( "Content-Type: text/html; charset=utf-8" );

MotlDB::connect();

//=====================

$base0 = Motl::getParameter( "base0" );
$base1 = Motl::getParameter( "base1" );

if( $base0 && $base1 ) {
	$comparator = new BaseComparator();
	$comparator->comments = true;
	//$comparator->allTables = true;
	echo 	"<pre>" . $comparator->compareBase( $base0, $base1 );
}
else {
	makeForm();
}

//=====================

function makeForm() {
	$script = $_SERVER['PHP_SELF'];

	$bases = MotlDB::queryValues( "show databases" );
	$options = "<option>" . implode( "<option>", $bases );
	$optionsNew = str_replace( "<option>motl<", "<option selected>motl<", $options );
	$optionsOld = str_replace( "<option>motl0<", "<option selected>motl0<", $options );
 
	echo "<form action='$script' method=POST>";
	echo "<table>";
	echo "<tr><td>'Новая' база</td> <td><select name=base0>$optionsNew</select></td></tr>";
	echo "<tr><td>'Старая' база</td><td><select name=base1>$optionsOld</select></td></tr>";
	echo "</table>";
	echo "<input type=submit>";
	echo "</form>";
}

//=====================
?>
