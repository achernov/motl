<?php
require_once "_core.inc";
require_once "_menu.inc";
require_once "loginForm.inc";
require_once "miniConsole.inc";
require_once "anonsContent.inc";
require_once "logger.inc";
//=====================

class MotlLayout extends Layout {
  public static $noRight = false;
  public $bannerPane;
//  public $menuPane;
  public $mainPane;
  public $rightPane;

  public $noLog = 0;

  function __construct() {
//    $this->bannerPane = "MOTL";
//    $this->menuPane = new MotlMenu();
  }

  public function produce( $format ) {
    if( ! $format )  $format = "normal";
    if( Motl::getParameter("printVersion") ) {
      return $this->producePrintVersion();
    }
    //--------------------------

    $picfolder = Motl::getSysPicturesFolder();

    $rdu = "<A href='http://www.rdu.ru'><img style='padding-top:3px;padding-bottom:7px' border='0' src='$picfolder/blockRDU1.png'></a>";
    $wdc = "<A href='http://www.wdcdance.com'><img style='padding-bottom:7px' border='0' src='$picfolder/blockWDC1.png'></a>";
    $wdcal = "<A href='http://www.wdcamateurleague.com/'><img style='padding-bottom:7px' border='0' src='$picfolder/blockWDC_AL1.png'></a>";
    $idu   = "<A href='http://www.interdanceunion.org/'><img style='padding-bottom:7px' border='0' src='$picfolder/blockIDU1.png'></a>";
    $naskt   = "<A href='http://www.naskt.ru/'><img border='0' src='$picfolder/blockNASKT1.png'></a>";

    $topTable = "<table style='padding-right:10' border='0' width=100% height=100%><tr>"
                   ."<td></td>"
                   ."<td valign=top align=right>$rdu<br>$wdc<br>$idu<br>$naskt</td>"
                   ."</tr></table>";

    //---------

    $feedback = "<div class='RegLogin' style='margin-top:30'><b>Обратная связь</b><br>Со всеми проблемами, вопросами и предложениями, связанными с сайтом, пишите: <A href=mailto:admin@danceleague.ru>admin@danceleague.ru</a></div>";

    //---------

    $pageProducer = new ThreeColumnPage();


    $pageProducer->title = "Московская Областная Танцевальная Лига";

    $pageProducer->topPaneProducer   = $topTable;

    $pageProducer->leftPaneProducer  = new VBox( array( new MotlMenu(), new LoginForm("short"), new MiniConsole(), $feedback ) );

    $pageProducer->mainPaneProducer  = $this->useProducer( $this->mainPane ); // Run it now, so it can customize everything else

    if( !MotlLayout::$noRight )
      $pageProducer->rightPaneProducer = new AnonsPaneProducer();
    else
      $pageProducer->rightPaneProducer = null;

    $s = "";

    $s .= $pageProducer->produce();

    if( ! $this->noLog )
      writeLog();

    return $s;
  }

  //--------------------------

  function producePrintVersion() {
    $s = "";
    $s .= "<link href='motlPrint.css' type='text/css' rel='stylesheet'>";
    $s .= "<body class=MainText>";
    $s .= $this->useProducer( $this->mainPane );
    return $s;
  }

}

//=====================

function setUrlParameter( $url, $param, $value ) {
  //--- Remove parameter, if present
  $arr = explode( "&", $url );
  $len = strlen($param);
  $out = array();
  foreach( $arr as $p ) {
    if( $p!=$param && substr($p,0,$len+1)!="$param=" ) {
      $out[] = $p;
    }
  }
  $cleanUrl = implode( "&", $out );
  
  //--- Add parameter
  if( is_int(strpos($cleanUrl,"?")) ) { // If have "?"
    return $cleanUrl."&$param=$value";
  }
  else { // If no "?"
    return $cleanUrl."?$param=$value";
  }
  
}

//=====================

class ThreeColumnPage extends Producer {
  public $title;
  public $topPaneProducer;
  public $leftPaneProducer;
  public $mainPaneProducer;
  public $rightPaneProducer;

  public function produce() {

    $s = "";
    $s .= "<html><head><title>$this->title</title>"
             ."<link rel=\"icon\" href=\"http://www.danceleague.ru/favicon.ico\" type=\"image/x-icon\">"
             ."<link rel=\"shortcut icon\" href=\"http://www.danceleague.ru/favicon.ico\" type=\"image/x-icon\"></head>";

    $s .= "<link href='motl.css' type='text/css' rel='stylesheet'>";

    $s .= "<link href='motl.css' type='text/css' rel='stylesheet'>";

    $s .= "<body>";

    $s .= $this->makeCommonScripts();

    $s .= "<table border=0 cellspacing=0 cellpadding=0 width=100% height=100%><tr><td colspan=3 class='BannerPane'>\n\n";

    $s .= $this->useProducer( $this->topPaneProducer );

    $s .= "\n\n</td></tr><tr valign=top><td class='MenuPane' >\n\n";

    $s .= $this->useProducer( $this->leftPaneProducer );

    $s .= "\n\n</td><td width=100%>\n\n";
    //== Main and Right panes ====

    $url = $_SERVER["REQUEST_URI"];
    $printVersionUrl = setUrlParameter($url,"printVersion",1);
    $printVersionBlock = 
        "<table border=0 width=210><tr><td style='padding-top:10;'>"
       ."<A style='font-family:arial;font-size:12;color:#808080' href='$printVersionUrl'>версия для печати</a>"
       ."</td></tr></table>";
    
    $s .= "<table border=0 bgcolor=white cellspacing=0 width=100% height=100%>"
         ."<tr><td colspan=3 class='MainTopPane' valign=top align=right >\n\n"
         .$printVersionBlock;
    $s .= "\n\n</td></tr><tr valign=top><td class='MainPane' >\n\n";

    $s .= $this->useProducer( $this->mainPaneProducer );

    if( $this->rightPaneProducer ) {
      $s .= "\n\n</td><td class='RightPane' >\n\n";
      $s .= $this->useProducer( $this->rightPaneProducer );
    }
    $s .= "\n\n</td></tr></table>\n\n";


    //== Main and Right panes end ====
    $s .= "\n\n</td></tr></table>\n\n";

    $s .= "</body>";
    return $s;
  }

  private function makeCommonScripts() {
    $script = $_SERVER['PHP_SELF'];
    $id = Motl::getParameter( "id" );

    $s = "\n\n";

    $s .= "<script src='debug.js'></script>\n";
    $s .= "<script src='core.js'></script>\n";

    return $s;
  }

}

//=====================

?>
