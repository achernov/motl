<?php 
require_once "_motl_db.inc";

require_once "testHistoryPaths.php";

//==============================================================

MotlDB::connect();

MotlDB::query( "DROP TABLE histFiles" );
MotlDB::query( "CREATE TABLE histFiles( "
		."`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,"
		."`path` VARCHAR(255) default NULL, "
		."`timeString` VARCHAR(255) default NULL, "
		."PRIMARY KEY (`id`) ) ENGINE = MyISAM;" );
		
MotlDB::query( "DROP TABLE histRecords" );
MotlDB::query( "CREATE TABLE  histRecords ( "
		."`id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,"
		."`idFile` int(11) default NULL, "
		."`tableName` VARCHAR(255) default NULL, "
		."`idRecord` VARCHAR(255) default NULL, "
		."`position` int(11) default NULL, "
		."`content` text, "
		."`md5` VARCHAR(40) default NULL, "
		."PRIMARY KEY (`id`) ) ENGINE=MyISAM DEFAULT CHARSET=utf8;" );

//==============================================================

//processFile( "D:/Sasha/DSite/data/pticekrolik_motl_20120110_1638.sql" );die('test');
//processFile( "F:/_Dance/!Backups/2/byDate/2009-11-26_12-01.sql" );die('test');

$paths = array(
"F:/_Dance/!Backups/pticekrolik_motl(2).sql",
//"F:/_Dance/!Backups/pticekrolik_motl(3).sql",
//"F:/_Dance/!Backups/pticekrolik_motl.sql"
);

foreach( $paths as $path ) {
	set_time_limit( 30 );
	processFile( $path );
}
die("<b>Done</b>");
//==============================================================

function processFile( $path ) {
	$s = file_get_contents( $path );
	
	$ts = getTimeString( $s );

	MotlDB::query( "INSERT INTO histFiles(path,timeString) VALUES('$path','$ts')" );
	
	$fid = MotlDB::queryValue( "SELECT id FROM histFiles WHERE path='$path'" );
echo "<b>$path</b><br>";
	processTable( $fid, $s, "anons", "processAnonsOrPageRecord" );
	processTable( $fid, $s, "staticpages", "processAnonsOrPageRecord" );
echo "<br>";
}

//==============================================================

function processTable( $fid, $raw, $tableName, $lineHandler ) {
echo "<u>$tableName</u><br>";flush();
	$records = getTable( $raw, $tableName );
	if( is_null($records) ) {
		echo "- table not found<br>";flush();
		return;
	}
	
	$cnt = 0;
	foreach( $records as $record ) {
		$cnt++;
//		echo "--&gt;$cnt<br>";
		$lineHandler( $fid, $tableName, $record );
	}
}

//==============================================================

function processAnonsOrPageRecord( $fid, $tableName, $rec ) {

	$a = strpos( $rec, "(" );
	$b = strpos( $rec, "," );
	//die($rec);
	$idRec = trim( substr( $rec, $a+1, $b-$a-1 ) );
	//die("=$idRec=");
	$c = strpos( $rec, ",", $b+1 );
	$x = trim( substr( $rec, $b+1, $c-$b-1 ) );
	//die("=$x=");
	if( substr($x,0,1) != "'" ) {
		$c = $b;
	}

	$d = strpos( $rec, ",", $c+1 );
	$pos = trim( substr( $rec, $c+1, $d-$c-1 ) );
	//die("=$pos=");
	
	$tail = trim( substr( $rec, $d+1 ) );
	//die("=$tail=");
	$tailLen = mb_strlen( $tail, "utf-8" );
	$content = mb_substr( $tail, 1,$tailLen-4,"utf-8" );
	//die("=$content=");
	////die( "$idRec, $pos, $content" );
	
	$sql = "INSERT INTO histRecords("
					."idFile,tableName,idRecord,position,content)"
			."VALUES($fid,'$tableName',$idRec,$pos,'$content')";
			
	//die($sql);
	MotlDB::query( $sql );
}

//==============================================================

function getTable( $raw, $tableName ) {

	$p = mb_strpos ( $raw, "INSERT INTO `$tableName` (", 0, "utf-8" );
	if( ! is_int($p) ) {
		return null;
		die( "Table not found" );
	}

	$p1 = mb_strpos ( $raw, ");\n\n", $p, "utf-8" );
	if( ! is_int($p1) ) {
		$p1 = mb_strpos ( $raw, ");\n/*", $p, "utf-8" );
	}
	if( ! is_int($p1) ) {
		return null;
		die( "End of table not found" );
	}
	
	$s = mb_substr( $raw, $p,$p1-$p+2,"utf-8" );
	$srcLines = explode( "\n", $s );
	//die( "$p $p1 $s" );
	$lines = array();
	
	for( $k=0; $k<count($srcLines); $k++ ) {
		$line = $srcLines[$k];
		if( substr($line,0,11) != "INSERT INTO" ) {
			$lines[] = $line;
		}
	}
	return $lines;
}

//==============================================================

function getTimeString( $raw ) {
	$hdr = mb_substr( $raw, 0,300,"utf-8" );
	$ahdr = explode( "\n", $hdr );
	foreach( $ahdr as $line ) {
		if( mb_strstr($line,"Время создания",false,"utf-8") ) {
			return $line;
		}
	}
	return null;
}

//==============================================================
?>
MOTL:
INSERT INTO `anons` (`id`,`ts_created`,`position`,`theText`) VALUES 
 (10,'0000-00-00 00:00:00',0,'<h2>Наши партнеры</h2><p>\r\n<table width=100% style=\'margin-left:-7\'><tr><td align=center>\r\n<A href=http://spdu.spb.ru/>[image|spts1.png|border=0 title=\'СПТС\']</a>\r\n</td><td align=center>\r\n<A href=http://idsa.com.ua/>[image|idsa1.png|border=0 title=\'IDSA\']</a>\r\n</td><td align=center>\r\n<A href=http://www.reiting-kachestva.ru/>[image|rk1.png|border=0 title=\'Рейтинг Качества\']</a>\r\n</td></tr></table>\r\n<p>\r\n'),
INSERT INTO `staticpages` (`id`,`ts_created`,`noRight`,`pageText`) VALUES 
 ('about','0000-00-00 00:00:00',NULL,'<big>\r\n<font color=red>\r\n\r\nВнимание!\r\n<p>\r\n\r\nВнесены изменения и дополнения в календаре МОТЛ на сезон 2010-2011 в разделе <A style=\'color:red\' href=\'calendar.php?id=events\'>\"Мероприятия МОТЛ в 2010/2011\"</a>\r\n\r\n<p>&nbsp;<p>\r\n\r\n<A style=\'color:red\' href=\'db.php?id=e558,r\'>Загружены на сайт результаты</a> Открытого Регионального и Классификационного турнира МОТЛ 22 января 2011г. в ТЦЗ \"Энтузиаст\".\r\n\r\n<p>&nbsp;<p>\r\n\r\nНачалась регистрация на Открытый Региональный и Классификационный турнир Московской Областной Танцевальной Лиги, который состоится 13 февраля в ТЦЗ \"Энтузиаст\".<br>\r\n<A style=\'color:red\' href=\'db.php?id=e560,f\'>Регистрация</a><br>\r\n<A style=\'color:red\' href=\'db.php?id=e560,c\'>Список участников</a><br>\r\n<A style=\'color:red\' href=\'db.php?id=e560,s\'>Расписание</a><br>\r\n<A style=\'color:red\' href=\'db.php?id=e560,w\'>Как проехать</a><br>\r\n\r\n<p>&nbsp;<p>\r\n\r\nНачалась регистрация на Открытый региональный и Классификационный турнир МОТЛ, Рейтинговый турнир РТС <b>\"Екатерининский бал\"</b>, который состоится 13 марта в Культурном Центре Вооруженных сил РФ.<br>\r\n<A style=\'color:red\' href=\'db.php?id=e564,f\'>Регистрация</a><br>\r\n<A style=\'color:red\' href=\'db.php?id=e564,c\'>Список участников</a><br>\r\n<!--A style=\'color:red\' href=\'db.php?id=e564,s\'>Расписание</a><br-->\r\n<A style=\'color:red\' href=\'db.php?id=e564,w\'>Как проехать</a><br>\r\n\r\n<p>&nbsp;<p>\r\n\r\n\r\nПродолжается регистрация на Открытый международный фестиваль латиноамериканского и европейского танца <A style=\'color:red\' href=\'http://russian-cup.ru/\'><b>Russian&nbsp;Cup</b></a>, который состоится 26-27 февраля 2011 в ДСЕ ЦСКА.<br>\r\nИнформация и онлайн-регистрация - на <A style=\'color:red\' href=\'http://russian-cup.ru/\'>сайте турнира</a>.\r\n\r\n</font></big>\r\n\r\n\r\n<!--big>\r\n<font color=red>\r\n\r\n<h1>Внимание!</h1>\r\n<big>\r\nПродолжается регистрация на Открытый международный фестиваль латиноамериканского и европейского танца <A style=\'color:red\' href=\'http://russian-cup.ru/\'><b>Russian&nbsp;Cup</b></a>, который состоится 26-27 февраля 2011 в ДСЕ ЦСКА.<br>\r\nИнформация и онлайн-регистрация - на <A style=\'color:red\' href=\'http://russian-cup.ru/\'>сайте турнира</a>.\r\n</big>\r\n\r\n\r\n\r\n<p>&nbsp;<p>\r\n\r\n<A style=\'color:red\' href=\'db.php?id=e555,r\'>Выложены результаты</a> Открытого классификационного турнира МОТЛ, состоявшегося 12&nbsp;декабря&nbsp;2010 в ДК&nbsp;\"Буревестник\".\r\n\r\n<p>&nbsp;<p>\r\n\r\nС 1 января все классификационные и рейтинговые турниры МОТЛ будут проводиться по правилам МОТЛ. Это касается, в частности, правил о костюмах и программы сложности\r\n\r\n</font></big-->\r\n'),

pticekrolik:
INSERT INTO `anons` (`id`, `ts_created`, `position`, `theText`) VALUES
(10, '0000-00-00 00:00:00', 0, '<h2>Наши партнеры</h2><p>\r\n<table width=100% style=''margin-left:-7''><tr><td align=center>\r\n<A href=http://spdu.spb.ru/>[image|spts1.png|border=0 title=''СПТС'']</a>\r\n</td><td align=center>\r\n<A href=http://idsa.com.ua/>[image|idsa1.png|border=0 title=''IDSA'']</a>\r\n</td><td align=center>\r\n<A href=http://www.reiting-kachestva.ru/>[image|rk1.png|border=0 title=''Рейтинг Качества'']</a>\r\n</td></tr></table>\r\n<p>\r\n'),
INSERT INTO `staticpages` (`id`, `ts_created`, `noRight`, `pageText`) VALUES
('about', '0000-00-00 00:00:00', NULL, '<big>\r\n<font color=red>\r\n<big>\r\n<h1>Внимание!</h1>\r\n</big>\r\n\r\n<b>15 января 2012 в ТЗ "Альфапластик" состоится Открытый классификационный турнир МОТЛ.</b><br>\r\n<A class=''redLink'' href=''db.php?id=e2157,f''>Регистрация</a><br>\r\n<A class=''redLink'' href=''db.php?id=e2157,c''>Список участников</a><br>\r\n<A class=''redLink'' href=''db.php?id=e2157,s''>Расписание</a><br>\r\n<A class=''redLink'' href=''db.php?id=e2157,p''>Наградной материал</a><br>\r\n<A class=''redLink'' href=''db.php?id=e2157,w''>Как проехать</a><br>\r\nГлавный судья Тудинов Сергей.<br>\r\nЗам. главного Шнырёв Владимер.<br>\r\nСудьи: Блинников Григорий, Бойко Леонид, Жупилов Сергей, Зайцева Елена, Сурма Виталий. '),
