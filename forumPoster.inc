<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "forumContent.inc";
require_once "tabs.inc";
require_once "userGallery.inc";
//=====================

class ForumPoster {

  //----------------

  function __construct() {
    $this->dbf_id  = Motl::getParameter("dbf_id");
    $this->dbf_pid = Motl::getParameter("dbf_pid");
    $this->kill    = Motl::getParameter("kill");
    $this->dbf_title   = $this->filterPostTitle( Motl::getParameter("dbf_title") );
    $this->dbf_theText = $this->filterPostText( Motl::getParameter("dbf_theText") );
    $this->dateNow = date( "Y-m-d H:i:s", time() );
    $this->user = User::getCurrentUser();
    $this->uid  = $this->user ? $this->user->getId() : null;
  }

  //----------------

  public function makePoster( $pid, $row ) {
    $script = $_SERVER['PHP_SELF'];
    $id     = $pid ? $pid : $row->id;

    if( $row ) {
      $dbf_id    = $row->id;
      $dbf_title = $row->title;
      $dbf_text  = $row->theText;
    }
    else {
      $dbf_id    = "";
      $dbf_title = "";
      $dbf_text  = "";
    }

    $s = "";
    
    $s .= $this->makePictureAdder();
    
    $s .= "<form name='PostForm' method='POST' action='forumReceiver.php'>";

    $s .= "<input type=hidden name='back' value='$script?id=$id'>";
    $s .= "<input type=hidden name='dbf_pid' value='$pid'>";
    $s .= "<input type=hidden name='dbf_id' value='$dbf_id'>";

    $s .= "<table width=100%><tr><td class='ForumPosterOuter'>";
    $s .= "<table  class='ForumPoster' width=100%><tr><td class='ForumPoster'>";
    $s .= "<table cellpadding=0 cellspacing=0 border=0 width=100% ><tr><td>Заголовок: </td><td width=100%><input name='dbf_title' value='$dbf_title' style='width:100%'></td></tr></table><p>"; 
    $s .= "<textarea id='taTheText' name='dbf_theText' rows=8 style='width:100%'>$dbf_text</textarea><p>";
    $s .= "<table width=100%><tr><td>";
      $s .= "<input class='ForumPosterButton' type=submit value='Сохранить'>";
      $s .= "<input class='ForumPosterButton' type=button value='Отменить' onclick='location.replace(document.forms.PostForm.back.value)'>";
    $s .= "</td><td align=right>";
    if( UserGallery::userPicturesEnabled() ) {
      $uid = (int)$this->uid;
      $s .= "<input class='ForumPosterButton' style='width:150' type=button value='Вставить картинку' onclick='openPictureInserter($uid,taTheText)'>";
    }
    $s .= "</td></tr></table>";
    $s .= "</td></tr></table>";
    $s .= "</td></tr></table>";

    $s .= "</form>";

    return $s;
  }

  //----------------
  
  private function makePictureAdder() {
    $s = "";
    
    $s .= "<div id='popupDiv' style='display:none;position:absolute;padding:20;border:3px solid #0000c0; background-color:white; left:300; top:300; width:600; height:500;'>";
    
    //=======

    $user = User::getCurrentUser();
    $userGallery = new UserGallery( $user->row );

    //=======
    
    $haveAlbum = $userGallery->countAlbums() ? 1 : 0;
    
    //=======

    $labelAlb = "Выбрать из фотоальбома";
    $labelNew = "Загрузить новую картинку";
    
    $tabs = new Tabs();
    $tabs->tableProperties = "style='margin-bottom:20'";
    if( $haveAlbum ) {
      $tabs->addTab( "alb",   $labelAlb, null, "document.all.divAlbum.style.display='block';document.all.divUpload.style.display='none';" );
    }
    $tabs->addTab( "new",   $labelNew, null, "document.all.divAlbum.style.display='none';document.all.divUpload.style.display='block';" );

    $tabs->current = "alb";
    $tabsAlbum = $tabs->produce();
    $tabs->current = "new";
    $tabsUpload = $tabs->produce();

    //=======

    $varHeight = 210;
    
    //=======
    
    $startDisplayAlb = $haveAlbum ? 'block' : 'none';
    $startDisplayNew = $haveAlbum ? 'none'  : 'block';
    
    //=======
    
    $s .= "<div id='divAlbum' style='height:$varHeight;margin-bottom:20;display:$startDisplayAlb' >";
    $s .= $tabsAlbum;
    $s .= "Ваши альбомы:<br>";

    $s .= $userGallery->makeAlbumSelector( false, null, "id='selAlbum' name='idAlbum' onchange='onSelAlbum()' style='width:400;margin-bottom:10'" );
    
    $s .= "<iframe id='frmAlbum' style='padding:3' width=100% height=100 src='forumMedia.php'></iframe>";
    $s .= "</div>";
    
    $s .= "<script>onSelAlbum()</script>";
    
    //=======
    
    $s .= "<div id='divUpload' style='height:$varHeight;margin-bottom:20;display:$startDisplayNew' >";
    $s .= $tabsUpload;
    $s .= "<iframe width=100% height=150 src='forumMedia.php'></iframe>";
    $s .= "</div>";
         
    //=======

    $s .= "<table width='100%' >";
    $s .= "<tr><td>Предварительный просмотр:</td></tr>";
    $s .= "<tr valign=top><td>";
    $s .= "<div id='divPreview' style='width:400;height:200;border:1px solid black'></div>";
    $s .= "</td><td>";
    $s .= "<input id='btnInsertPic' type=button value='Вставить' onclick='onInsert()'><p>";
    $s .= "<input type=button value='Отменить' onclick='onCancel()'>";
    $s .= "</td></tr></table>";

    //=======
    
    $s .= "</div>";
    
    return $s;
  }
  
  //----------------

  public function save() {

    if( $this->dbf_id ) { 
      if( Motl::getParameter("kill") ) {
        ForumPoster::killPost();
      }
      else {
        ForumPoster::saveEditedPost();
      }
    }
    else {
      if( $this->dbf_pid ) {
        ForumPoster::saveNewPostOrTopic();
      }
    }
  }

  //----------------

  public function saveEditedPost() {

    MotlDB::saveRow( "forum", "id",$this->dbf_id, 
        array( "title"=>$this->dbf_title, "theText"=>$this->dbf_theText, "updatedWhen"=>$this->dateNow, "updatedWho"=>$this->uid ) );

    Motl::redirect( "forum.php?id=$this->dbf_id" );
  }

  //----------------

  public function saveNewPostOrTopic() {

    //--- get parent info

    ASSERT_int($this->dbf_pid);
    $pinfo = MotlDB::queryRow( "SELECT type,exclusive FROM forum WHERE id=$this->dbf_pid", "" );  // ASS $this->dbf_pid
    $ptype     = $pinfo->type;
    $exclusive = $pinfo->exclusive;

    $idNewPost = 0;

    switch( $ptype ) {

      case 'section': // New Topic
        $idNewTopic = $this->writeNewTopic( $this->dbf_pid, $exclusive );
        $idNewPost = $this->writeNewPost( $idNewTopic, $exclusive );
        ForumUtil::updateLevel( $idNewTopic );
        break;

      case 'topic': // New Post
        $idNewPost = $this->writeNewPost( $this->dbf_pid, $exclusive );
        ForumUtil::updateLevel( $this->dbf_pid );
        break;
    }


    Motl::redirect( "forum.php?id=$idNewPost" );
  }

  //----------------

  private function writeNewTopic( $pid, $exclusive ) {

    $obj = new ForumItemProducer();
    $obj->loadFromQuery();

    $obj->row->type = "topic";
    $obj->row->theText = "";

    $obj->row->pid = $pid;
    $obj->row->exclusive = $exclusive;
    $obj->row->createdWhen = $this->dateNow;
    $obj->row->createdWho  = $this->uid;

    $obj->saveToDB();
    return $obj->row->id;
  }

  //----------------

  private function writeNewPost( $pid, $exclusive ) {

    $obj = new ForumItemProducer();
    $obj->loadFromQuery();

    $obj->row->type = "post";
    $obj->row->exclusive = $exclusive;
    $obj->row->createdWhen = $this->dateNow;
    $obj->row->createdWho  = $this->uid;

    $obj->row->pid = $pid;

    ASSERT_int($pid);
    $nPosts = MotlDB::queryValue( "SELECT count(*) FROM forum WHERE pid=$pid", 0 );  // ASS $pid
    $obj->row->position = $nPosts;

    $obj->saveToDB();
    return $obj->row->id;
  }

  //----------------

  public function killPost() {
    MotlDB::saveRow( "forum", "id",$this->dbf_id, 
        array( "type"=>'killed', "updatedWhen"=>$this->dateNow, "updatedWho"=>$this->uid ) );

    Motl::redirect( "forum.php?id=$this->dbf_id" );
  }

  //----------------
  
  private function filterPostText( $text ) { // UNFSAFE ?? (не DB)
    return strip_tags( $text );
  }
  
  //----------------
  
  private function filterPostTitle( $text ) { // UNFSAFE ?? (не DB)
    return strip_tags( $text );
  }
  
  //----------------
};

//=====================
?>