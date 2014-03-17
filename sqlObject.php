<?php
require_once "_layout.inc";
require_once "_dbproducer_factory.inc";
//=====================

MotlDB::connect();

$what = Motl::getParameter( "what" );

$object = DbProducerFactory::createObject( $what );

if( ! $object )
  motlDie( "Can't create: $what" );

$dbf_id = Motl::getParameter( "dbf_id" );

motlASSERT( "", !is_null($dbf_id) );

$object->loadFromDB( $dbf_id );

//=====================

function isYes( $s ) {
	switch( strtolower($s) ) {
		case 'yes':
		case 'true':
		case '1':
			return 1;
	}
	return 0;
}

function produceCodes( $object ) {
	$table = $object->getTableName();
	$id = $object->row->id;
	ASSERT_alnum($id);

	// COD $table
	$colRows = MotlDB::queryRows( "SHOW  COLUMNS FROM $table" ); 
//	print_r($colRows);
	
	// COD $table, ASS $id
	$dataRow = MotlDB::queryRow( "SELECT * FROM $table WHERE id='$id'" ); 
//	print_r($dataRow);


	$insFields = array();
	$insValues = array();
	$sets   = array();
	
	foreach( $colRows as $colRow ) {
		//print_r($colRow);continue;
		$name = $colRow->Field;
		$value = $dataRow->$name;
		$nullable = isYes($colRow->Null);
		
		if( strlen($value) > 0 ) {
			$sqlValue = "'".MRES( $value )."'";
		}
		else {
			$sqlValue = $nullable ? "null" : "''";
		}
		
		$insFields[] = $name;
		$insValues[] = $sqlValue;
		if( $name != "id" ) {
			$sets[] = "$name=$sqlValue";
		}
	}

	$sqlId = MRES( $object->row->id );

	$sinsFields = implode( ", ", $insFields );
	$sinsValues = implode( ", ", $insValues );
	$ssets      = implode( ", ", $sets );
	
	$sqlInsert = "INSERT INTO $table($sinsFields) VALUES($sinsValues);";
	$sqlUpdate = "UPDATE $table SET $ssets WHERE id=$sqlId;";
	
	$sqlInsertHtml = htmlspecialchars ( $sqlInsert );
	$sqlUpdateHtml = htmlspecialchars ( $sqlUpdate );
	
	$s = "";
	
	$s .= "<b>INSERT</b><br>";
	$s .= "<textarea style='width:800;height:200'>$sqlInsertHtml</textarea>";
	$s .= "<p>";	
	$s .= "<b>UPDATE</b><br>";
	$s .= "<textarea style='width:800;height:200'>$sqlUpdateHtml</textarea>";
	
	return $s;
}

//=====================

$form = produceCodes( $object );

$layout = new MotlLayout(); 
$layout->mainPane = $form;

MotlLayout::$noRight = true;
$layout->write();

//=====================
?>
