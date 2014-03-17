<?php
//=====================

class MotlMenu extends Producer {

  private $items;

  function __construct() {
    $editmode = Motl::getParameter( "editmode" );

    $this->items = array();
    $this->items[] = new MenuItem( 1, "about",		"Главная", "./" );
    $this->items[] = new MenuItem( 1, "org",		"Об организации", "" );
    $this->items[] = new MenuItem( 2, "presidium",		"Президиум", "page.php?id=presidium" );
    $this->items[] = new MenuItem( 2, "docs",			"Официальные документы", "page.php?id=docs" );

    $this->items[] = new MenuItem( 1, "news",		"Новости", "news.php" );

    $this->items[] = new MenuItem( 1, "comps",		"Соревнования", "" );

    $this->items[] = new MenuItem( 2, "cald",			"Календарь", "calendar.php" );
    if( User::isPresidium() ) {
//      $this->items[] = new MenuItem( 2, "caldtest",			"Календарь (тест)", "calendar1.php?id=201009" );
    }

    $this->items[] = new MenuItem( 2, "reg",			"Онлайн-регистрация", "db.php?id=f" );
    $this->items[] = new MenuItem( 2, "res",			"Результаты", "db.php?id=r" );
	
    //if( User::isPresidium() ) {
		$this->items[] = new MenuItem( 1, "rating",		"Кубок сезона", "" );
		$this->items[] = new MenuItem( 2, "ratingrating",	"Рейтинговый 2013/2014", "rating.php?part=rating" );
		$this->items[] = new MenuItem( 2, "ratingclass",	"Класификационный 2013/2014", "rating.php?part=class" );
		$this->items[] = new MenuItem( 2, "rating2012",	"Сезон 2011/2012", "page.php?id=rating2012" );
		$this->items[] = new MenuItem( 2, "rating2011",	"Сезон 2010/2011", "page.php?id=rating2011" );
		$this->items[] = new MenuItem( 2, "rating2010",	"Сезон 2009/2010", "page.php?id=rating2010" );
	/*}
	else {
		$this->items[] = new MenuItem( 1, "rating",		"Рейтинг сезона", "" );
	}*/
	
    //if( User::isAdmin() ) {
      $this->items[] = new MenuItem( 1, "base",		"База данных", "base.php" );
    //}
    
    $this->items[] = new MenuItem( 1, "forum",		"Форум", "forum.php" );
    if( User::isPresidium() ) {
      $this->items[] = new MenuItem( 1, "chat",		"Чат (закрытый)", "chat.php" );
    }
    
    $this->items[] = new MenuItem( 1, "gallery0",	"Галерея", "" );
    $this->items[] = new MenuItem( 2, "gallery",			"Фото и видео", "gallery.php" );
  }

  public function produce() {

    $picfolder = Motl::getSysPicturesFolder();

    $s = "";



    $s .= "<div class='MenuDiv'>\n";
    $s .= "<table class='MenuTable' border=0 cellpadding=0 cellspacing=0><tr><td>\n";
    $s .= "<tr><td><img src='$picfolder/whitePlashka1.png'></td></tr>\n";

    for( $k=0; $k<count($this->items); $k++ ) {
       $item = $this->items[$k];
       $selected = ( $item->rubr == Motl::$Rubric );
       $sel = $selected ? "s" : "";
       $class = "MenuItem" . $item->level . $sel;

       $label = $item->label;

       if( $item->url ) {
         $label = "<A class='$class' href='$item->url'>$label</a>";
       }

       if( $k>0 && $item->level==1 )
         $s .= "<tr><td><img src='$picfolder/MenuDivider.png'></td></tr>\n";

       $s .= "<tr><td class='$class'><nobr>$label</nobr></td></tr>\n";
    }

    $s .= "<tr><td><img src='$picfolder/whitePlashka2.png'></td></tr>\n";
    $s .= "</td></tr></table>\n";
    $s .= "</div>\n";

    return $s;
  }
}

//=====================

class MenuItem {
  function __construct( $level, $rubr, $label, $url=null ) {
    $this->level = $level;
    $this->rubr  = $rubr;
    $this->label = $label;
    $this->url   = $url;
  }
}

//=====================

?>
