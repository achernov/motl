<?php
//======================================================================
//==  BaseComparator
//======================================================================

class BaseComparator {

	function __construct(){
	
		$this->comments = false;
		$this->allTables = false;
		$this->newline = "\r\n";
		
		$this->skipTables = array( 
			"captchas", 
			"chat_who",
			//"db_link", 
			"errors", 
			"logtable" 
		);
	}
	
	//----------------
	
	function compareBase( $base0, $base1 ) {
	
		//-----
		
		$tables0 = MotlDB::queryValues( "show tables from ".MRES($base0) );
		$tables1 = MotlDB::queryValues( "show tables from ".MRES($base1) );
		
		$tables = array_intersect( $tables0, $tables1 );
		
//die("qqq $base0, $base1");
		//-----
		
		$s = "";

		$nl = $this->newline;
		
		$dashLine = "--------------------------------";
		
		foreach( $tables as $table ) {
		
			if( !in_array($table,$this->skipTables) ) {
				if( $this->hasId($table,$base0) ) {
				
					$t = $this->compareTable( $table, $base0, $base1 );
					
					if( $t || $this->allTables ) {
						if( $this->comments ) {
							$s .= "$nl$nl-- $dashLine$nl";
							$s .= "--\t\t$table $nl";
							$s .= "-- $dashLine$nl";
						}
						$s .= $t;
					}
				}
				else {
					if( $this->comments ) {
						$s .= "$nl$nl-- $dashLine$nl";
						$s .= "--\t\tWARNING: "
								."table '$table' has no id's $nl";
						$s .= "-- $dashLine$nl";
					}
				}
			}
		}
		
		return $s;
	}
		
	//----------------

	function hasId( $table, $base ) {

		return $this->getIdFields( $table, $base ) ? true : false;
	}

	//----------------

	function getIdFields( $table, $base ) {

		$rows = MotlDB::queryRows( "SHOW COLUMNS FROM ".MRES($table)." FROM ".MRES($base) );

		$fields = array();
		foreach( $rows as $row ) {
			if( $row->Key == 'PRI' ) {
				$fields[] = $row->Field;
			}
		}
		//echo "$table ";	print_r($fields);
		return $fields;
	}

	//----------------

	function compareTable( $table, $base0, $base1 ) {

		$fields = array();
		$rows = MotlDB::queryRows( "SHOW COLUMNS FROM ".MRES($table)." FROM ".MRES($base0) );
		foreach( $rows as $row ) {
			$fields[] = $row->Field;
		}
		
		$idsD = $idsA = $idsU = "";
		
		$s = "";
		
		$s .= $this->compareDel	 ( $table, $base0, $base1, $idsD );
		$s .= $this->compareAdd	 ( $table, $base0, $base1, $idsA, $fields );
		$s .= $this->compareUpdate( $table, $base0, $base1, $idsU, $fields );
		
		$h = "";
		if( $idsD || $idsA || $idsU ) {
			$nl = $this->newline;
			$h .= "-- Deleted: $idsD$nl";
			$h .= "-- Added:	 $idsA$nl";
			$h .= "-- Updated: $idsU$nl$nl";
		}
		
		if( $this->comments ) {
			$s = $h.$s;
		}
		
		return $s;
	}

	//----------------

	function makeSelectFields( $table, $fields ) {
		$a = array();
		for( $k=0; $k<count($fields); $k++ ) {
			$a[] = "$table.$fields[$k]";
		}
		$s = implode( ", ", $a );
		return $s;
	}

	//----------------

	function makeJoinOn( $table0, $table1, $fields ) {
		$a = array();
		for( $k=0; $k<count($fields); $k++ ) {
			$a[] = "$table0.$fields[$k]=$table1.$fields[$k]";
		}
		$s = implode( " AND ", $a );
		return $s;
	}

	//=====================

	function makeIdCond( $row, $fields ) {
		$a = array();
		for( $k=0; $k<count($fields); $k++ ) {
			$name = $fields[$k];
			$value = $row->$name;
			$a[] = "$name='".MRES($value)."'";
		}
		$s = implode( " AND ", $a );
		return $s;
	}

	//----------------

	function makeIdCoords( $row, $fields ) {
		$a = array();
		for( $k=0; $k<count($fields); $k++ ) {
			$a[] = $row->$fields[$k];
		}
		$s = implode( ":", $a );
		return $s;
	}

	//----------------

	function compareDel( $table, $base0, $base1, &$out_ids ) {
		ASSERT_name($base0);
		ASSERT_name($base1);
		ASSERT_name($table);
		
		$idFields = $this->getIdFields( $table, $base0 );
		$joinOn = $this->makeJoinOn( "a", "b", $idFields );
		$sqlIdFields = $this->makeSelectFields( "b", $idFields );
		$simple = (count($idFields)==1);
		
		// ASS $base0,$base1,$table
		$sql = "SELECT $sqlIdFields "
				."FROM $base0.$table AS a RIGHT JOIN $base1.$table AS b ON $joinOn "
				."WHERE a.$idFields[0] is null";
		//die($sql);
		
		$rows = MotlDB::queryRows( $sql );
		//print_r($rows);	die();
	//die( $simple ? "1" : "0" );

		$s = "";
		$nl = $this->newline;
		
		$ids = array();
		foreach( $rows as $row ) {
			$ids[] = $this->makeIdCoords( $row, $idFields );
			
			if( ! $simple ) {
				$idCond = $this->makeIdCond( $row, $idFields );
				
				$s .= "DELETE FROM $table WHERE $idCond;$nl";
			}
		}
		
		$out_ids = implode(",",$ids);

		if( $simple ) {
			if( count($ids) > 0 ) {
				// ASS $table
				// FLD $sids
				$sids = "'".implode( "','", $ids )."'";
				return "DELETE FROM $table WHERE $idFields[0] In($sids);$nl";
			}
		}
		else {
			return $s;
		}
		
		return null;
	}

	//----------------

	function compareAdd( $table, $base0, $base1, &$out_ids, $fields ) {
		ASSERT_name($base0);
		ASSERT_name($base1);
		ASSERT_name($table);
		
		$idFields    = $this->getIdFields( $table, $base0 );
		$joinOn      = $this->makeJoinOn( "a", "b", $idFields );
		$sqlIdFields = $this->makeSelectFields( "a", $idFields );
		
		// ASS $base0,$base1,$table
		$sql = "SELECT a.* "
					."FROM $base0.$table AS a LEFT JOIN $base1.$table AS b ON $joinOn "
					."WHERE b.$idFields[0] is null";
		//die($sql);
	/*	$sql = "SELECT a.* "
					."FROM $base0.$table AS a LEFT JOIN $base1.$table AS b ON a.id=b.id "
					."WHERE b.id is null";
		*/
		$s = "";
		
		$rows = MotlDB::queryRows( $sql );
		
		$ids = array();
		
		foreach( $rows as $row ) {
		
			$ids[] = $this->makeIdCoords( $row, $idFields ); //$row->id;
			
			$fs = array();
			$vs = array();
			
			//print_r( $row );
			
			//echo "<hr>";print_r($row);
			foreach( $fields as $field ) {
			//echo "$table.$field<br>";
				$value = $row->$field;
				$fs[] = $field;
				$vs[] = $this->valueForMySql( $value );
			}
			$sfs = implode( ",", $fs );
			$svs = implode( ",", $vs );
			
			// ASS $table
			// COD $sfs, $svs
			$nl = $this->newline;
			$s.= "INSERT INTO $table($sfs) VALUES($svs);$nl";
		}
		
		$out_ids = implode(",",$ids);

		return $s;
	}

	//----------------
	// SQLPARAM $base0,$base1,$table,$fields

	function compareUpdate( $table, $base0, $base1, &$out_ids, $fields ) { // UNSAFE ?

		$idFields = $this->getIdFields( $table, $base0 );
		$joinOn   = $this->makeJoinOn( "a", "b", $idFields );

		//--- Prepare select
		$prefix = "wodiwejocsndjs";
		
		$sfields = "a.".implode( ", a.", $fields );
		$sfields = "a.*";
		
		$conds = array();
		foreach( $fields as $field ) {
			if( ! in_array($field,$idFields) ) {
				$sfields .= ", Not(binary a.$field<=>binary b.$field) AS $prefix$field";
				$conds[] = "Not(binary a.$field<=>binary b.$field)";
			}
		}
		$sconds = implode( " Or ", $conds );
		
		$sql = "SELECT $sfields "
	//				."FROM $base0.$table AS a INNER JOIN $base1.$table AS b ON a.id=b.id "
					."FROM $base0.$table AS a INNER JOIN $base1.$table AS b ON $joinOn "
					."WHERE $sconds";
		//die($sql);
		//--- Process data
		
		$rows = MotlDB::queryRows( $sql );
		
		$s = "";
		
		$ids = array();
		
		foreach( $rows as $row ) {

		//print_r( $row );
		
			$us = array();
			
			$id = $row->id;
			$ids[] = $this->makeIdCoords( $row, $idFields ); //$id;
			
			foreach( $fields as $field ) {
				if( ! in_array($field,$idFields) ) {
					$value = $row->$field;
					$flagname = $prefix.$field;
					$flag	= $row->$flagname;
					if( $flag ) {
						$us[] = $field."=".$this->valueForMySql( $value );
					}
				}
			}
			$sus = implode( ", ", $us );
			
			$idCond = $this->makeIdCond( $row, $idFields );
		
			$nl = $this->newline;
			$s.= "UPDATE $table SET $sus WHERE $idCond;$nl";
		}
		
		$out_ids = implode(",",$ids);

		return $s;
	}

	//----------------

	function valueForMySql( $value ) {
		if( is_null($value) ) return "null";
		
		$v = mysql_real_escape_string( $value, MotlDB::$db );
		return "'$v'";
	}

	//----------------
}

//======================================================================
//==  BaseDuplicator
//======================================================================

class BaseDuplicator {

	function __construct(){
	
		$this->source = null;
		$this->destination = null;
		
		$this->skipTables = array( 
			"captchas", 
			"chat_who",
			"logtable" 
		);
	}

	//----------------
	
	function duplicateBase() {
	
		$tablesSrc = MotlDB::queryValues( "show tables from "
											. MRES($this->source) );
										
		$tablesDest = MotlDB::queryValues( "show tables from "
											. MRES($this->destination) );
											
		foreach( $tablesDest as $table ) {
			$deleteSQL = "DROP TABLE IF EXISTS {$this->destination}.$table";
			MotlDB::query( $deleteSQL );
		}
		
		foreach( $tablesSrc as $table ) {
			if( !in_array($table,$this->skipTables) ) {
				$this->duplicateTable( $table );
			}
		}
	}
	
	//----------------
	
	function duplicateTable( $table ) {
	
		//----- Get creation statement
		
		$row = MotlDB::queryRow( "SHOW CREATE TABLE {$this->source}.$table" );
		$createSQL = $row->{"Create Table"};
		
		//----- Adjust creation statement
		
		if( substr($createSQL,0,13) != "CREATE TABLE " ) {
			die( "FATAL - wrong creating srtatement: $createSQL" );
		}
		
		//----- Execute creation statement
		
		$createSQL = "CREATE TABLE " 
					. $this->destination . "." 
					. substr($createSQL,13);
		MotlDB::query($createSQL);
		
		//----- Make and Execute copy statement
		
		$copySQL = "INSERT INTO {$this->destination}.$table "
					."SELECT * FROM {$this->source}.$table";
					
		MotlDB::query($copySQL);
		
//		die($copySQL);
	}
	
	//----------------
}

//======================================================================

class BaseInfo {

	function creationTime( $base ) {
		$rows = MotlDB::queryRows( "SHOW TABLE STATUS FROM $base" );
		$maxt = $rows[0]->Create_time;
		foreach( $rows as $row ) {
			$t = $row->Create_time;
			$maxt = max( $t, $maxt );
		}
		return $maxt;
	}
	
}

//======================================================================
?>