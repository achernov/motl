<?php
//=====================

class FlashPlayer {

  public $src    = "player.swf";
  public $width  = 320;
  public $height = 240;

  public $bgcolor = "#000000";
  public $align = "middle";
  public $salign = "lt";

  public $allowFullScreen = "true";
  public $allowScriptAccess = "sameDomain";
  public $quality = "high";
  public $scale = "noscale";

  public $flashVars = "";

  //===========

  function __construct( $src, $width=null, $height=null ) {
    $this->src = $src;
    if( $width  ) $this->width  = $width;
    if( $height ) $this->height = $height;
  }

  //===========

  public function produce() {

    $sFlashVars = $this->packflashVars( $this->flashVars );

    return  "<object classid='clsid:d27cdb6e-ae6d-11cf-96b8-444553540000' codebase='http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=8,0,0,0' "
                  ."width='$this->width' height='$this->height' align='$this->align'>"

           ."  <param name='allowfullscreen' value='$this->allowFullScreen' />"
           ."  <param name='allowScriptAccess' value='$this->allowScriptAccess' />"
           ."  <param name='movie' value='$this->src' />"
           ."  <param name='FlashVars' value='$sFlashVars' />"
           ."  <param name='quality' value='$this->quality' />"
           ."  <param name='bgcolor' value='$this->bgcolor' />"
           ."  <param name='scale' value='$this->scale' />"
           ."  <param name='salign' value='$this->salign' />"
	
           ."  <embed src='$this->src' "
                  ."width='$this->width' "
                  ."height='$this->height' "
                  ."align='$this->align' "
                  ."quality='$this->quality' "
                  ."bgcolor='$this->bgcolor' "
                  ."scale='$this->scale' "
                  ."salign='$this->salign' "
                  ."FlashVars='$sFlashVars' "
                  ."allowfullscreen='$this->allowFullScreen' "
                  ."allowScriptAccess='$this->allowScriptAccess' "
                  ."type='application/x-shockwave-flash' pluginspage='http://www.macromedia.com/go/getflashplayer' />"

           ."</object>";
  }

  //===========

  private function packflashVars( $array ) {
    $a = array();
    foreach( $array as $key=>$val ) {
      $a[] = "$key=$val";
    }
    return implode( "&", $a );
  }

  //===========
}

//=====================
							
function makePlayer( $fileName, $previewFile, $width, $height ) {

  return makePlayerJW( $fileName, $previewFile, $width, $height );
//  return makePlayerAnyFLV( $fileName, $previewFile, $width, $height );
}

//=====================

function makePlayerJW( $fileName, $previewFile, $width, $height ) {

	if( !$width || !$height ) {
		$width  = 320;
		$height = 240;
	}
	
	$dh = 19+2;

  $player = new FlashPlayer( "player_jw/player.swf", $width, $height+$dh );

  $params["file"]            = getLocalSetting('VIDEO_URL') . "/$fileName";
  $params["image"]            = MediaManager::getBaseUrl()."/$previewFile";
/*  $params["ControlPanelColor"] = "0x6DAFEB";
  $params["buttonColor"]       = "0x000000";
  $params["ControlPanelPosition"] = "0";
  $params["showControlPanel"]     = "1";
  $params["ShowtimeColor"]        = "0xffffff";
  $params["bAutoPlay"]            = "1";
  $params["bAutoRepeat"]          = "0";
  $params["BufferTime"]           = "5";
  $params["tmeColor"]             = "0xffffff";
  $params["loaColor"]             = "0x666666";
  $params["GroundColor"]          = "0x000000";
  $params["conNum"]         = "5";
  $params["bShowtime"]      = "1";
  $params["startvolume"]    = "100";*/

  $player->flashVars = $params;

  return  $player->produce();
}

//=====================

function makePlayerAnyFLV( $fileName, $previewFile ) {

  $player = new FlashPlayer( "player.swf", 320, 274 );

  $params["flvURL"]            = getLocalSetting('VIDEO_URL') . "/$fileName";
  $params["ControlPanelColor"] = "0x6DAFEB";
  $params["buttonColor"]       = "0x000000";
  $params["ControlPanelPosition"] = "0";
  $params["showControlPanel"]     = "1";
  $params["ShowtimeColor"]        = "0xffffff";
  $params["bAutoPlay"]            = "1";
  $params["bAutoRepeat"]          = "0";
  $params["BufferTime"]           = "5";
  $params["tmeColor"]             = "0xffffff";
  $params["loaColor"]             = "0x666666";
  $params["GroundColor"]          = "0x000000";
  $params["conNum"]         = "5";
  $params["bShowtime"]      = "1";
  $params["startvolume"]    = "100";

  $player->flashVars = $params;

  return  $player->produce();
}

//=====================

function makeVideoTool( $file, $script, $autoTime=null ) {

  $player = new FlashPlayer( getLocalSetting('SWF_URL') . "/VideoTool.swf", 1000, 600 );

  $params["flvURL"] = getLocalSetting('VIDEO_URL') . "/$file";
  $params["script"] = urlencode( $script );
  $params["autoTime"] = $autoTime;

  $player->flashVars = $params;

  return  $player->produce();
}

//=====================
?>