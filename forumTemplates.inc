<?php
require_once "mediaManager.inc";
//=====================

class ForumTemplates {

    //=================

    public static $boardTableRow = 

"<tr>
<td colspan=4 class='ForumTableRow_board'>[editbar||&nbsp;]<b>[plain|title]</b></td>
</tr>\n";

    //=================

    public static $sectionTableHeader = 
"<table width='100%' border=1 cellpadding=3 cellspacing=0 lass='ForumTable'>
<tr>
<td style='width:500' class='ForumTableHeader_board'>Раздел</td>
<td style='width:300' class='ForumTableHeader_board'>Последнее сообщение</td>
<td style='width:100' class='ForumTableHeader_board'>Тем</td>
<td style='width:100' class='ForumTableHeader_board'>Сообщений</td>
</tr>\n";

    //-----------------

    public static $sectionTableRow = 
"<tr>
<td style='width:500' class='ForumTableRow_section'>[editbar||&nbsp;]<b><A class='ForumTableRow_section_title[plain|new]' href=forum.php?id=[plain|id]&[editmode]>[plain|title]</a></b><br>[plain|theText]</td>
<td style='width:300' class='ForumTableRow_section'>[if|lastPostId&updatedWho| <b><A href=forum.php?id=[plain|lastPostId]&[editmode]>[plain|lastPostTitle]</a></b><br>[plainDateTime|updatedWhen] от [nick|updatedWho]|<small>нет сообщений</small>] </td>
<td style='width:100' class='ForumTableRow_section' align=center >[plain|nTopics]&nbsp;</td>
<td style='width:100' class='ForumTableRow_section' align=center >[plain|nPosts]&nbsp;</td>
</tr>\n";

    //-----------------

    public static $sectionTableRowNone     = 
"<tr><td colspan=4 class='ForumTableRow_section'>раздел пуст</td></tr>\n";

    //-----------------

    public static $sectionTableFooter = 
"</table>\n";

    //=================

    public static $topicTableHeader = 
"<table width='100%' border=1 cellpadding=3 cellspacing=0 lass='ForumTable'>
<tr>
<td style='width:500' class='ForumTableHeader_topic' >
    <table border=0 width='100%'><tr><td>Тема/Автор</td><td>&nbsp;</td><td align=right>[newTopicButton]</td></tr></table>
</td>
<td style='width:300' class='ForumTableHeader_topic' >Последнее сообщение</td>
<td style='width:100' class='ForumTableHeader_topic' >Сообщений</td>
<!--td style='width:100' class='ForumTableHeader_topic' >Просмотров</td-->
</tr>\n";

    //-----------------

    public static $topicTableNewRow = 
"<tr>
<td colspan='4' class='ForumTableRow_topic'>[plain|postCell]</td>
</tr>\n";

    //-----------------

    public static $topicTableRow = 
"<tr>
<td style='width:500' class='ForumTableRow_topic'>[editbar||&nbsp;]<b><A class='ForumTableRow_topic_title[plain|new]' href=forum.php?id=[plain|id]&[editmode]>[plain|title]</a></b><br>[nick|createdWho]</td>

<td style='width:300' class='ForumTableRow_topic'>[if|lastPostId&updatedWho| <b><A href=forum.php?id=[plain|lastPostId]&[editmode]>[plainDateTime|updatedWhen]</a></b><br>от [nick|updatedWho]|<small>нет сообщений</small>]</td>

<td style='width:100' class='ForumTableRow_topic' align=center>[plain|nPosts]</td>
<!--td style='width:100' class='ForumTableRow_topic' align=center>[plain|nPosts]</td-->
</tr>\n";

    //-----------------

    public static $topicTableRowNone = 
"<tr><td colspan=4 class='ForumTableRow_topic'>нет ни одной темы</td></tr>\n";

    //-----------------

    public static $topicTableFooter = 
"</table>\n";

    //=================

    public static $postTableHeader = 
"<table width='100%' border=1 cellpadding=3 cellspacing=0 lass='ForumTable'>";

    //-----------------

    public static $killedPostTableRow = 
    "<tr style='height:10;' ><td colspan=2 style='padding:0'>
        <table width='100%' border=0 cellpadding=3 cellspacing=0><tr>\n
            <td style='height:10; padding-left:3' class='ForumTableRow_post1'><A name='post[plain|id]'></a>[plainDateTime|createdWhen|style='font-size:12;font-weight:bold']</td>
            <td align=right style='height:10; padding-left:3' class='ForumTableRow_post1'>[postNumber|style='font-size:12;font-weight:bold']
        </tr></table>\n
    </td></tr>\n
    <tr >
        <td style='width:70;' class='ForumTableRow_post2' valign=top>
            [editbar|edit,del]<br>
        </td>
        <td style='width:100%;' valign=top class='ForumTableRow_post3'>
            <center><i>Сообщение удалено</i></center>
        </td>
    </tr>\n";

    //-----------------

    public static $postTableRow = 
    "<tr style='height:10;' ><td colspan=2 style='padding:0'>
        <table width='100%' border=0 cellpadding=3 cellspacing=0><tr>\n
            <td style='height:10; padding-left:3' class='ForumTableRow_post1'><A name='post[plain|id]'></a>[plainDateTime|createdWhen|style='font-size:12;font-weight:bold']</td>
            <td align=right style='height:10; padding-left:3' class='ForumTableRow_post1'>[postNumber|style='font-size:12;font-weight:bold']
        </tr></table>\n
    </td></tr>\n
    <tr style='height:120;' >
        <td style='width:70;' class='ForumTableRow_post2' valign=top>
            [user|createdWho]<br>
            [editbar|edit,del]<br>
        </td>
        <td style='width:100%;' valign=top class='ForumTableRow_post3'>[plain|postCell]</td>
    </tr>\n";

    //-----------------

    public static $postCell = 
       "<table border=0 width=100% height=100%><tr><td class='ForumTableRow_post3'>
           <b>[plain|title]</b><p>
           [plainBB|theText]<br>
           [plain|moderatorial|class='moderatorial']
[if|updatedWho|<br><i>Сообщение изменено пользователем [nick|updatedWho] [plainDateTime|updatedWhen]</i>]

        </td></tr><tr><td valign=bottom align=right style='padding-right:50'  class='ForumTableRow_post3'>
           [if|canEdit|
               <A href=forum.php?id=[plain|pid],e[plain|id]#post[plain|id]>изменить</a>
               <A href=forumReceiver.php?kill=1&dbf_id=[plain|id] onclick='return confirmDeleteMessage()'>удалить</a>
           ]
        </td></tr></table>";

    public static $postTableFooter = 
"</table>\n [newPostButton]";

    //=================

    public static $editForm = "
 <table><tr>
   <td width=100><b>id<br>[plain|id]&nbsp;</td>
   <td><b>ts_created<br>[text|ts_created]</td>
   <td><b>pid<br>[text|pid]</td>
   <td><b>position<br>[text|position]</td>
   <td><b>type<br>[text|type]</td>
   <td><b>exclusive<br>[text|exclusive]</td>
 </tr></table>

 <b>PostNumber<br>[text|PostNumber]<p>
 <b>title<br>[text|title|style='width:100%']<p>
 <b>theText<br>[textarea|theText|style='width:100%; height:300']<p>
 <b>moderatorial<br>[textarea|moderatorial|style='width:100%; height:100']<p>

 <table><tr>
   <td><b>createdWho<br>[text|createdWho]</td>
   <td><b>createdWhen<br>[text|createdWhen]</td>
 </tr><tr>
   <td><b>updatedWho<br>[text|updatedWho]</td>
   <td><b>updatedWhen<br>[text|updatedWhen]</td>
 </tr><tr>
   <td><b>lastPostId<br>[text|lastPostId]</td>
   <td><b>lastPostTitle<br>[text|lastPostTitle|style='width:500']</td>
 </tr></table>

 <table><tr>
   <td><b>nTopics<br>[text|nTopics]</td>
   <td><b>nPosts<br>[text|nPosts]</td>
   <td><b>nViews<br>[text|nViews]</td>
 </tr></table>";

    //-----------------
}

//=====================

class ForumTemplator extends DbProducerTemplator {

  private function getPostLink() {

    $id = Motl::getParameter( "id", 0 );
    if( strchr($id,",") ) return null;

    if( User::getCurrentUser() ) {
//      return "forumPost.php?id=$id";
      return "forum.php?id=$id,enew#post";
    }
    else {
      return "loginPage.php?script=forum.php&id=$id,enew#post";
    }

  }

  //-----------

  function processTag_plainBB( $tagParts ) {
    $value = $this->getFValue($tagParts);
    $value = str_replace( "\n", "<br>", $value );
    
    $postTemplator = new PostTemplator();
    $value = $postTemplator->process( $value );
    
    $value = $this->filterSpan( $value, $tagParts );
    return $value;
  }

  //-----------

  function processTag_editmode( $tagParts ) {
    $editmode = Motl::getParameter( "editmode", 0 );
    if( $editmode ) 
      return "editmode=$editmode";
    else
      return "";
  }

  //-----------

  function processTag_postNumber( $tagParts ) {
    $pos = $this->row->position;
    $value = "#".($pos+1);
    if( isset($tagParts[1]) ) {
      $value = strlen($value) ? "<span $tagParts[1]>$value</span>" : "";
    }
    return $value;
  }

  //-----------

  function processTag_newTopicButton( $tagParts ) {
    $postLink = $this->getPostLink();
    if( is_null($postLink) ) return "";
    return "<A style='font-size:12' href='$postLink'>Начать новую тему</a>";
  }

  //-----------

  function processTag_newPostButton( $tagParts ) {
    $postLink = $this->getPostLink();
    if( is_null($postLink) ) return "";
    return "<A style='font-size:14' href='$postLink'>Написать новое сообщение</a>";
  }

  //-----------

  function processTag_nick( $tagParts ) {

    $uid = $this->getFValue($tagParts);
    ASSERT_int( $uid );

    $user = new User();
    $user->loadFromDB( $uid );

    if( ! $user->row ) return "<small>????</small>";

    //-----

    $template = "<A class='Nick' href='profile.php?id=[plain|id]'>[plain|login]</a>";

    return $user->MakeText( $user->row, $template );
  }

  //-----------

  function processTag_nickWap( $tagParts ) {

    $uid = $this->getFValue($tagParts);
    ASSERT_int( $uid );

    $user = new User();
    $user->loadFromDB( $uid );

    if( ! $user->row ) return "<small>????</small>";

    //-----

    return $user->row->login;
  }

  //-----------

  function processTag_user( $tagParts ) {

    $uid = $this->getFValue($tagParts);
    ASSERT_int( $uid );

    $user = new User();
    $user->loadFromDB( $uid );

    if( ! $user->row ) return "<center><small>????</small></center>";

    //-----

    $picfolder  = Motl::getPicturesFolder();

    $template = "<A class='Nick' href='profile.php?id=[plain|id]'>[plain|login]</a><br>"
               ."[if|avatar|<img src=$picfolder/[plain|avatar]><br>]"
               ."<small><b>[plain|status]</b></small><br>";

    return $user->MakeText( $user->row, $template );
  }

}

//=====================

class PostTemplator extends DbProducerTemplator {

  //===========

  function processTag_gallery( $tagParts ) {

    $idItem = (int)$tagParts[1];
    $item = MediaManager::getItem( $idItem ); // SAFE
    $picture = $item->getPicture(100,100);
    $pictureHtml = $picture->getHtml();
    return "<A title='Нажмите, чтобы увеличить' href='' onclick=\"zoomPicture($idItem);return false;\">$pictureHtml</a>";
  }
 
  //===========
}

//=====================
?>
