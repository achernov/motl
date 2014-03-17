<?php
//=====================

function getLocalSetting( $name ) {

  $serverName = strtolower( $_SERVER['SERVER_NAME'] );
  $production = ($serverName=="chernov-pc"||$serverName=="chernov-pc1"||$serverName=="localhost") ? 0 : 1;

//----------

  switch( $name ) {

	case 'DISABLE_BASE_LINK': return 0;
	
    case 'SEND_ACTIVATION': return $production ? 1 : 0;

    case 'DB_NAME': return $production ? "pticekrolik_motl" : "motl";

    case 'DB_USER': return $production ? "pticekrolik" : "root";

    case 'DB_PASS': return $production ? "cj,frf" : "";

//    case 'VIDEO_URL': return $production ? "ftp://pticekrolik_dvid:rfrf@www.danceleague.ru" : Motl::getScriptsFolderUrl()."/gallery";
    case 'VIDEO_URL': return Motl::getScriptsFolderUrl()."/gallery";

//    case 'SWF_URL': return $production ? "" : "/Memorial";
    case 'SWF_URL': return ".";

    case 'LOG_PATH': return $production ? "/home/users/p/pticekrolik/logs/danceleague.ru_access.log" : "c:\\Sasha\\DanceSite\\logs\\danceleague.ru_access.log";

    default: die( "Unknown setting: '$name'" );
  }
}

//=====================
?>