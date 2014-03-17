<?php
require_once "_dbproducer.inc";
require_once "forumTemplates.inc";
require_once "forumPoster.inc";
require_once "forumUtil.inc";
//=====================

class ForumContent extends Producer {

  public function produce() {

    $id = Motl::getParameter( "id", 0 );
	
	if( ! $id ) {
		$id = MotlDB::queryValue( "SELECT id "
					."FROM forum WHERE pid Is Null ORDER BY id LIMIT 1" );
		if( ! $id ) $id=0;
	}

    $s = "<script src='forum.js'></script>\n";

    $producer = new ForumItemProducer();
    $producer->loadFromDB( $id );

    if( ! $producer->checkViewRight() ) {
      return "";
    }

    $s .= $producer->produce( "full" );
    $s .= $this->makeZoomDiv();

    return $s;
  }
  //----------------
  
  private function makeZoomDiv() {
    
    $s = "";
    
//    $s .= "<div id='divZoom' style='display:none;position:absolute;padding:20;border:3px solid #0000c0; background-color:white; left:300; top:300; width:0; height:0;' >";
//    $s .= "<div id='divZoom' style='display:none;border:2px solid #000000;position:absolute;' >";

    $uhoStyle = "padding-bottom:1;padding-top:2;padding-left:10;padding-right:10;border:1px solid #000000;background-color:#e0ecf8;";

    $close = "<A href='' style='text-decoration:none;font-family:arial;font-weight:bold;font-size:14;color:#00008b' onclick='closeZoom();return false;'>Закрыть</a>";
    
    $s .= "<div id='divZoom' style='display:none;position:absolute;' >";
    $s .= "<table border=0 cellpadding=5><tr><td align=right><span style='$uhoStyle'>$close</span>";
    $s .= "<div id='divZoom1' style='border:2px solid #000000;' ></div>";
    $s .= "</td></tr></table>";
    $s .= "</div>";
    
    return $s;
  }
  
  //----------------
}

//=====================

class ForumItemProducer extends DbProducer {
  public $row;

  public $childrenRows;

  //----------------

  public function getTableName() {
    return "forum";
  }

  //----------------

  public function getFieldNames() {
    return array( "id", "pid", "position", "type", "exclusive", "title", "theText", "moderatorial",
                  "createdWhen", "createdWho", "updatedWhen", "updatedWho",
                  "lastPostId", "lastPostTitle",
                  "nTopics", "nPosts", "nViews", "PostNumber" );
  }

  //----------------

  public function killFromDB() {
    if( !isset($this->row->pid) || !$this->row->pid ) {
      $this->loadFromDB( $this->id );
    }
    $pid = $this->row->pid;

    $this->killRowFromDB();

    ForumUtil::cleanupOrphans();
    ForumUtil::updateLevel( $pid );
  }

  //----------------
  // SQLPARAM $types, $order
  
  public function loadChildren( $type, $order ) {

    $addNewchild = ( isset($this->aid[1]) && $this->aid[1]=='enew' );

    if( is_array($type) ) {
      $types = "'" . implode("', '",$type) . "'";
    }
    else {
      $types = "'$type'";
    }
    
    //-----

    $this->childrenRows = array();
    
    //-----

    if( $addNewchild && $this->row->type=='section' ) {
        $this->childrenRows[] = $this->newEmptyRow( 'topic', $this->row->id );
    }

    //-----

    if( ! $addNewchild ) {
      $thisId = $this->aid[0];
      ASSERT_int($thisId);
      
      // ASS $thisId
      // PAR $types, $order
      $sql = "SELECT * FROM forum WHERE pid='$thisId' and type In($types) ORDER BY $order";
      $rows = MotlDB::queryRows( $sql ); 
      
      for( $k=0; $k<count($rows); $k++ ) {

        $item = new ForumItemProducer( $rows[$k] );

        if( $item->checkViewRight() ) {
          $this->childrenRows[] = $item->row;
        }
      }
    }

    //-----
    
    if( $addNewchild && $this->row->type=='topic' ) {
        $this->childrenRows[] = $this->newEmptyRow( 'post', $this->row->id );
    }

  }

  //----------------

  private function newEmptyRow( $type, $pid ) {

    return (object)array( 'id'=>null, 'pid'=>$pid, 'position'=>null, 'type'=>$type, 
                          'title'=>null, 'theText'=>null,
                          'createdWho'=>User::getCurrentUser()->getId() );
  }

  //----------------

  public function checkViewRight() {

    $exclusive = $this->row->exclusive;
    if( ! $exclusive ) {
      return true;
    }

    $user = User::getCurrentUser();

    if( $user ) {
      if( $user->checkRight($exclusive) ) {
        return true;
      }
    }

    return false;
  }

  //----------------
  
  public function produceWap() {
    $s = "";
    $s .= $this->makePathWap();
    //echo($this->row->type);
    
    switch( $this->row->type ) {
      case 'section':
        $order = 'createdWhen DESC';
        break;
        
      case 'topic':
        $order = 'createdWhen ASC';
        break;
        
      default:
        $order = 'position';
        break;
    }

    $children = $this->loadChildren( array('board','section','root','topic','post'), $order ); // Sql Safe
    //print_r($this->childrenRows);

//    $curPostId = isset($this->aid[1]) ? $this->aid[1] : $this->childrenRows[0]->id;
    $curPostId = isset($this->aid[1]) ? $this->aid[1] : null;

    for( $k=0; $k<count($this->childrenRows); $k++ ) {
      $row = $this->childrenRows[$k];
      
      if( $row->type == 'post' ) {
      //print_r($row);
        $cutLen = 40;
        $cutLen = 20;
        
        $isLong = ( mb_strlen($row->title,"utf-8") + mb_strlen($row->theText,"utf-8") > $cutLen );
        
                                        
        $isSelected = ( $row->id == $curPostId );
        
        //echo "isSelected = $isSelected=<br>";
        
        if( $isSelected || !$isLong ) {
          $textTemplate = "[plain|title]<br/>[plain|theText]";
        }
        else {
          $row->theText = mb_substr($row->theText,0,$cutLen,"utf-8") . "...";
          $textTemplate = "<a href='wapForum.php?id=[plain|pid],[plain|id]'>[plain|title][plain|theText]</a>";
        }

        $text = $this->makeText( $row, "<p>[plainDate|createdWhen]&nbsp;<b>[nickWap|createdWho]</b>&nbsp;$textTemplate</p>", new ForumTemplator() );
        $filter = new WapFilter();
        $s .= $filter->filterWap($text);
        //$s .= WapFilter::filterWap( $text );
      }
      else {
        $s .= "<p><a href='wapForum.php?id=$row->id'>$row->title</a></p>";
      }
    }
    return $s;
  }
  
  //----------------

  public function produce( $format=null ) {
    if( ! $format ) $format = $this->format;

    $s = "";

    if( $format == 'full' ) {
      $s .= "<h1>Форум Московской Областной Танцевальной Лиги</h1>";

      $s .= $this->makePath();
    }

    //-----------
    
    $new = 0;
    if( isset($this->row->updatedWhen) ) {
      $t =  strtotime( $this->row->updatedWhen );
      if( $t && getPrevVisitTime() ) {
        if( $t > getPrevVisitTime() ) {
          $new = 1;
        }
      }
    }
    $this->row->new = $new;
    
    //-----------

    if( $format == "edit" ) {
      if( ! isset($this->row->id) && isset($this->row->pid) )
        $this->row->type = $this->getInittype();
      $s .= $this->makeEditForm( ForumTemplates::$editForm );
      return $s;
    }

    //-----------

    switch( $this->row->type .':'. $format ) {

      //-----------

      case 'root:full':
        $this->loadChildren( 'board', 'position' ); // Sql Safe
        $s .= $this->produceChildren( "shortWithChildren", "[editbar|edit,add]".ForumTemplates::$sectionTableHeader, ForumTemplates::$sectionTableFooter );
        break;
        return $s;

      //-----------

      case 'board:shortWithChildren':
            $s .= $this->makeText( $this->row, ForumTemplates::$boardTableRow, new ForumTemplator() );

            $this->loadChildren( 'section', 'position' ); // Sql Safe

            if( $this->countChildren() )
              $s .= $this->produceChildren( "short", "", "" );
            else
              $s .= ForumTemplates::$sectionTableRowNone;

            break;

      case 'board:full':
            $this->loadChildren( 'section', 'position' ); // Sql Safe
            if( $this->countChildren() )
              $s .= $this->produceChildren( "short", ForumTemplates::$sectionTableHeader, ForumTemplates::$sectionTableFooter );
            else
              $s .= $sectionTableHeader.$sectionTableRowNone.$sectionTableFooter;
            break;

      //-----------

      case 'section:short':
            $s .= $this->makeText( $this->row, ForumTemplates::$sectionTableRow, new ForumTemplator() );
            break;

      case 'section:full':
            $this->loadChildren( 'topic', 'updatedWhen DESC' ); // Sql Safe

            if( $this->countChildren() )
              $s .= $this->produceChildren( "short", ForumTemplates::$topicTableHeader, ForumTemplates::$topicTableFooter );
            else
              $s .= $this->makeText( $this->row, ForumTemplates::$topicTableHeader . ForumTemplates::$topicTableRowNone . ForumTemplates::$topicTableFooter, new ForumTemplator() );

            break;

      //-----------

      case 'topic:short':
            $p = Motl::splitId(Motl::getParameter("id",0));
            $e = isset($p[1]) ? $p[1] : "";
            $eid = (substr($e,0,1)=='e') ? substr($e,1) : null;

	    if( $eid=='new' && is_null($this->row->id) ) {
              $poster = new ForumPoster();
              $this->row->postCell = $poster->makePoster( $this->row->pid, $this->row );
              $s .= $this->makeText( $this->row, ForumTemplates::$topicTableNewRow, new ForumTemplator() );
            }
            else {
              $s .= $this->makeText( $this->row, ForumTemplates::$topicTableRow, new ForumTemplator() );
            }
            break;

      case 'topic:full':
            $this->loadChildren( array('post','killed'), 'position' ); // Sql Safe
            $s .= $this->produceChildren( "short", ForumTemplates::$postTableHeader, ForumTemplates::$postTableFooter );
            break;

      //-----------

      case 'killed:short':
            return $this->makeText( $this->row, ForumTemplates::$killedPostTableRow, new ForumTemplator() );

      //-----------

      case 'post:short':
            $user = User::getCurrentUser();

            if($user) {
              $this->row->canEdit = ($user->id==$this->row->createdWho)||$user->checkRight('admin');
            }

            $p = Motl::splitId(Motl::getParameter("id",0));
            $e = isset($p[1]) ? $p[1] : "";
            $eid = (substr($e,0,1)=='e') ? substr($e,1) : null;

            if( $eid=='new' && is_null($this->row->id) ) {
              $poster = new ForumPoster();
              $this->row->postCell = $poster->makePoster( $this->row->pid, $this->row );
            }
            else
            if( $eid == $this->row->id ) {
              $poster = new ForumPoster();
              $this->row->postCell = $poster->makePoster( null, $this->row );
            }
            else {
              $this->row->postCell = $this->makeText( $this->row, ForumTemplates::$postCell, new ForumTemplator() );
            }

            return $this->makeText( $this->row, ForumTemplates::$postTableRow, new ForumTemplator() );
            break;

      case 'post:full':
      case 'killed:full':
            $script = $_SERVER['PHP_SELF'];
            $id  = $this->row->id;
            $pid = $this->row->pid;
			Motl::redirect( "$script?id=$pid#post$id" );
            break;

      //-----------

      default: 
        motlDie( 'No case: '.$this->row->type .':'. $format );
    }

    //-----------

    return $s;
  }

  //----------------

  private static $childTypes = array( "root"=>"board",  "board"=>"section",  "section"=>"topic",  "topic"=>"post",  "post"=>null );

  private function getInittype() { // Init type from parent type
    $pid = $this->row->pid;
    ASSERT_int( $pid );

    $ptype = MotlDB::queryValue( "SELECT type FROM forum where id='$pid'", null );  // ASS $pid

    if( isset( ForumItemProducer::$childTypes[$ptype] ) )
      return ForumItemProducer::$childTypes[$ptype];

    return "";
  }

  //----------------
  private function countChildren() {
    if( ! $this->childrenRows ) return 0;
    return count( $this->childrenRows );
  }

  //----------------

  private function produceChildren( $format, $header, $footer ) {

    $s = "";

    $s .= $this->makeText( $this->row, $header, new ForumTemplator() );

    for( $k=0; $k<count($this->childrenRows); $k++ ) {
      $producer = new ForumItemProducer( $this->childrenRows[$k] );
      $s .= $producer->produce( $format );
    }

    $s .= $this->makeText( $this->row, $footer, new ForumTemplator() );

    return $s;
  }

  //----------------

  public function makePath( ) {
    return parent::makePath( null, "forum", "forum.php", "title" );
  }

  //----------------

  public function makePathWap( ) {
    return parent::makePathWap( "<a href='wap.php'>Главная</a> <b>/</b> ", "forum", "wapForum.php", "title" );
  }

  //----------------
}

//=====================
?>
