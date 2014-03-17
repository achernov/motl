<?php
//=====================

class Tabs extends producer {

  private $tabs;

  public $current;
  public $tableProperties;

  function addTab( $id, $label, $url, $jscript=null, $more=null ) {
    $this->tabs[] = (Object)array( "id"=>$id, "label"=>$label, "url"=>$url, "jscript"=>$jscript, "more"=>$more );
  }

  function produce() {

    $folder = Motl::getSysPicturesFolder();

    $s = "\n<table cellspacing=0 cellpadding=0 $this->tableProperties><tr valign=top>\n";

    $sel = -1;

    for( $k=0; $k<count($this->tabs); $k++ ) {
      if( $this->tabs[$k]->id == $this->current ) {
        $sel = $k;
      }
    }

    $lastLevel = 0;

    $bg  = "$folder/tabB0.png";
    $s .= "<td background=$bg style='width:10;padding-top:2;font-family:arial;font-size:12;background-repeat:repeat-x;'>&nbsp;</td>";

    for( $k=0; $k<count($this->tabs); $k++ ) {
      $tab = $this->tabs[$k];

      $curLevel = ($k==$sel) ? 2 : 1;

      $id    = $tab->id;
      $label = $tab->label;
      $url   = $tab->url;
      $js    = $tab->jscript;
      $more  = $tab->more;

      if( $k != $sel ) {
        if( $url ) {
          $label =  "<A href='$url'>$label</a>";
        }
        else
        if( $js ) {
          $label =  "<A href='' onclick=\"$js;return false;\">$label</a>";
        }
      }
      
      if( $more ) {
        $label =  "$more$label";
      }

      $sep = "$folder/tabC$lastLevel$curLevel.png";
      $bg  = "$folder/tabB$curLevel.png";

      $s .= "<td><img src=$sep></td>";

      $s .= "<td background=$bg style='padding-top:2;font-family:arial;font-size:12;background-repeat:repeat-x;'>$label</td>\n";

      $lastLevel = $curLevel;
    }
    $sep = "$folder/tabC$lastLevel"."0.png";
    $s .= "<td><img src=$sep></td>\n";

    $bg  = "$folder/tabB0.png";
    $s .= "<td background=$bg style='width:30;padding-top:2;font-family:arial;font-size:12;background-repeat:repeat-x;'>&nbsp;</td>";

    $s .= "\n</tr></table>\n";

    return $s;
  }
}

//=====================
?>