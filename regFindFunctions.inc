<?php
require_once "_motl_db.inc";
require_once "_motl_user.inc";
require_once "baseColumn.inc";
require_once "dataUtil.inc";
//=====================

class RegFind {
	
	static $maxTipRows = 7;
	
	//-------
	
	function rowsToXML( $rows ) {
	
		$rowCnt = 0;
		$sitems = "";
		$moreItems = 0;
		
		foreach( $rows as $row ) {
			$item = "<item";
			foreach( $row as $key=>$value ) {
				$item .= " $key='$value'";
			}
			$item .= " />";

			$sitems .= $item;

			$rowCnt ++;
			if( $rowCnt >= RegFind::$maxTipRows ) {
				$moreItems = 1;
				break;
			}
		}

		return "<xml more='$moreItems'>$sitems</xml>";
	}
	
	//-------

	function loadPerson( $cond, $skipAge=false ) {
	
		$sql = "SELECT id, surname, name, birthDate "
			."FROM db_person "
			."WHERE $cond "
			."ORDER BY surname, name "
			."LIMIT ".(RegFind::$maxTipRows+1);
			
		$rows = MotlDB::queryRows( $sql ); 
		
		foreach( $rows as $row ) {
$skipAge=false;
			if( $skipAge ) {
				unset($row->birthDate);
			}
			else {
				$row->birthDate = DataUtil::dateDbToHumanNoTime($row->birthDate);
			}
		}
		
		return $rows;
	}

	//-------

	function loadCouple( $cond ) {

		$sql = "SELECT  Couple.id,Couple.country,Couple.city,Couple.club,Couple.teacher,"
				."Man.surname as msurname, Man.name as mname, Man.birthDate as mdate,"
				."Lady.surname as fsurname,Lady.name as fname, Lady.birthDate as fdate "
			."FROM db_couples AS Couple "
				."INNER JOIN db_person AS Man ON Couple.idMan=Man.id "
				."INNER JOIN db_person AS Lady ON Couple.idLady=Lady.id "
			."WHERE $cond "
			."ORDER BY msurname, mname, fsurname, fname "
			."LIMIT ".(RegFind::$maxTipRows+1);
			
		$rows = MotlDB::queryRows( $sql ); 

		$bcCountry = new BaseColumnCountry();
		$bcCity = new BaseColumnCity();
		$bcClub = new BaseColumnClub();
		$bcTeacher = new BaseColumnTeacher();

		foreach( $rows as $row ) {
			$row->mdate = DataUtil::dateDbToHumanNoTime($row->mdate);
			$row->fdate = DataUtil::dateDbToHumanNoTime($row->fdate);
			$row->country = $bcCountry->sidsToSvalues( $row->country );
			$row->city =    $bcCity->sidsToSvalues( $row->city );
			$row->club =    $bcClub->sidsToSvalues( $row->club );
			$row->teacher = $bcTeacher->sidsToSvalues( $row->teacher );
		}

		return $rows;
	}
	
	//-------
	
	function loadClub( $cond ) {
		$sql = "SELECT id, name, disambig, country, city, boss "
			."FROM db_club "
			."WHERE $cond "
			."ORDER BY name, disambig "
			."LIMIT ".(RegFind::$maxTipRows+1);
			
		$rows = MotlDB::queryRows( $sql );

		$bcCountry = new BaseColumnCountry();
		$bcCity = new BaseColumnCity();
		$bcBoss = new BaseColumnBoss();  

		foreach( $rows as $row ) {
			$row->country = $bcCountry->sidsToSvalues( $row->country );
			$row->city =    $bcCity->sidsToSvalues( $row->city );
			$row->boss =    $bcBoss->sidsToSvalues( $row->boss );
		}

		return $rows;
	}
	
	//-------
}

//=====================
?>
