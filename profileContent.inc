<?php
require_once "_core.inc";
require_once "_motl_user.inc";
require_once "forumTemplates.inc";
require_once "forumUtil.inc";
require_once "userGallery.inc";
//=====================

class ProfileProducer extends User {

  //================

  public function produce() {
    $s = "";
  
    $s .= "<h1>Форум Московской Областной Танцевальной Лиги</h1>";
  
    if( isset($this->aid[1]) ) {
      switch( $this->aid[1] ) {
        case 'm':
          $s .= $this->produceUserMessages();
          break;
      }
    }
    else {
      $s .= $this->produceUserInfo();
    }
    
    return $s;
  }
  
  //================
  
  private function produceUserMessages() {

    $id = (int) $this->row->id;
  
    $s = "";
  
    $s .= $this->makeText( $this->row, "<h2>Все сообщения пользователя [nick|id]:</h2>", new ForumTemplator() );
    
    $filter = ForumUtil::makeFilterCondition( User::getCurrentUser(), "post" );
    
    // COD $id, $filter 
    $rows = MotlDB::queryRows( 
              "SELECT post.id, post.createdWhen, post.title, post.theText, topic.title as topicTitle
              FROM forum as post INNER JOIN forum as topic ON post.pid=topic.id 
              WHERE post.createdWho=$id and post.type='post' and $filter 
              ORDER BY post.createdWhen DESC" );  

    $s .= "<table border=1 cellspacing=0 style='margin-left:20;margin-top:20'>\n";
    $s .= "<tr>"
            ."<td class='MsgListHeader'> Время написания </td>"
            ."<td class='MsgListHeader'> Тема </td>"
         ."</tr>";
    
    for( $k=0; $k<count($rows); $k++ ) {
      $s .= $this->makeText( $rows[$k], 
                "<tr>"
               ."<td class='MsgListCell'> <A href='forum.php?id=[plain|id]'>[plainDateTime|createdWhen]</a> </td>"
               ."<td class='MsgListCell'> [plain|topicTitle] </td>"
               ."</tr>" 
            );
    }
    
    $s .= "</table>\n";

    return $s;
  }
  
  //================

  private function produceUserInfo() {
  
    motlASSERT( $this->row, $this->row );
//    ASSERT_int( "thisrow" );
	
    $script = $_SERVER['PHP_SELF'];
    $id = (int) $this->row->id;
	
    $format = Motl::getParameter( "format" );

    $itsMe = $this->isUserMe();
    $canEdit = $this->isUserEditable();

    //-------

    if( $format == "loadavatar" ) {
      $avatar = $this->makeAvatarUploadForm();
    }
    else {

      if( $this->isUploading() ) {
        $loadError = $this->receiveAvatar();
        if( $loadError ) $loadError = "<br><font color=red>$loadError</font>";
      }
      else {
        $loadError = "";
      }

      $avatarLoadUrl = "$script?id=$id&format=loadavatar";
      $avatarLoad   = $canEdit ? "<br><A href='$avatarLoadUrl'>Загрузить</a>" : "";
      $avatarReload = $canEdit ? "<br><A href='$avatarLoadUrl'>Поменять</a>" : "";

      $avatar = "нет$loadError$avatarLoad";

      if( $this->row ) {
        $avPic = $this->row->avatar;
        if( $avPic ) {
          $picFolder = Motl::getPicturesFolder();
          $avatar = "<img src='$picFolder/$avPic'>$loadError$avatarReload";
        }
      }
    }

    //-------

    $filter = ForumUtil::makeFilterCondition( User::getCurrentUser() );
    $countMessages  = MotlDB::queryValue( "SELECT count(*) FROM forum WHERE createdWho='$id' AND type='post' AND $filter", 0 );  // COD $id, $filter

    $lastMessageRow = MotlDB::queryRow( "SELECT * FROM forum WHERE createdWho='$id' AND type='post' AND $filter ORDER BY id DESC LIMIT 1" );  // COD $id, $filter
    $lastMessage = DbProducer::makeText( $lastMessageRow, "<A href=forum.php?id=[plain|id]>[plainDateTime|createdWhen]</a>" );

    //-------

    $status = "не существует";

    if( $this->row ) {
      $status = $this->getStatusText();
    }

    $meMessage = $itsMe ? " &nbsp;- это Вы" : "";

    $s = "";

    $templateA    = "<table width='700' cellspacing=0 cellpadding=5 border=1>"
                       ."<tr><td colspan=2 class='ProfileHeader'>Пользователь: [plain|login|class='Nick'] $meMessage</td></tr>"
                       ."<tr valign=top><td width='350' class='ProfileBody'>";

    $templateB    = "</td><td width='350' class='ProfileBody'>";

    $templateC    = "</td></tr>"
                   ."</table>";

    $templateInfo  = "<table cellspacing=0 border=0 class='pcGroup'>";
    $templateInfo .=     "<tr valign=top><td class='pcl'>Аватар: </td><td class='pcr'> $avatar</td></tr>";
      $templateInfo .=     "<tr style='height:10'><td></td></tr>";
    $templateInfo .=     "<tr><td class='pcAl'>Статус: </td><td class='pcAr'>$status&nbsp;</td></tr>";
    $templateInfo .=     "<tr><td class='pcCl'>Дата регистрации: </td><td class='pcCr'> [plainDate|registrationDate]&nbsp;</td></tr>";
    if( $canEdit ) {
      $templateInfo .=     "<tr style='height:10'><td></td></tr>";
      $templateInfo .=     "<tr bgcolor=#56A0E2><td class='pcAl'><small>Личные данные:</small><td class='pcBr'><input class='pcBtn' type=button value='изменить'></td> </tr>";
      $templateInfo .=     "<tr><td class='pcBl'>Имя: </td><td class='pcBr'> [plain|name]&nbsp;</td></tr>";
      $templateInfo .=     "<tr><td class='pcCl'>Фамилия: </td><td class='pcCr'>[plain|surname]&nbsp;</td></tr>";
    }
    $templateInfo .= "</table>";

    $templateStat  = "<table>";
    $templateStat .= "<tr><td> Всего сообщений:  </td><td> $countMessages </td></tr>";
    $templateStat .= "<tr><td> Последнее сообщение:  </td><td> $lastMessage </td></tr>";
	if( $countMessages ) {
      $templateStat .= "<tr><td colspan=2> <A href='profile.php?id=$id,m'>Найти все сообщения</a> </td></tr>";
	}
    $templateStat .= "</table>";

    $albumText = "";
    if( UserGallery::userPicturesEnabled() ) {
      $userGallery = new UserGallery( $this->row );
      $albumText = $userGallery->produceSummary();
    }

    $s .= $this->makeText( $this->row, $templateA . $templateInfo . $templateB . $templateStat . $albumText . $templateC );

    return $s;
  }

  //================

  private function makeAvatarUploadForm() {
    $script = $_SERVER['PHP_SELF'];
    $id = $this->row->id;
    $s = "";
    $s .= "<form enctype='multipart/form-data' action='$script' method='POST'>\n";
    $s .= "    <input type='hidden' name='script' value='$script' />\n";
    $s .= "    <input type='hidden' name='id'     value='$id' />\n";
    $s .= "    <input type='hidden' name='MAX_FILE_SIZE' value='3000000' />\n";
    $s .= "    <input name='userfile' type='file' /><p>\n";
    $s .= "    <input type='submit' value='Загрузить' />\n";
    $s .= "</form>\n";
    return $s;
  }

  //================

  private function isUploading() {
    return isset( $_FILES['userfile'] );
  }

  //================

  private function receiveAvatar() { // returns error string or null if ok

    $script = $_SERVER['PHP_SELF'];
    $id = $this->row->id;

    $name = $_FILES['userfile']['name'];
    $ext = strtolower( strrchr( $name, "." ) );

    $maxForm = Motl::getParameter( 'MAX_FILE_SIZE' );
    $maxIni  = ini_get( 'upload_max_filesize' );

    if( $_FILES['userfile']['error'] ) {
      switch( $_FILES['userfile']['error'] ) {
        case UPLOAD_ERR_INI_SIZE: 
        case UPLOAD_ERR_FORM_SIZE:
          return "Не удалось загрузить файл - превышен допустимый&nbsp;размер&nbsp;$maxForm&nbsp;байт";

        default: 
          return "Не удалось загрузить файл";
      }
    }

    $image = "avatar_$id$ext";
//print_r($_FILES);
    $source = $_FILES['userfile']['tmp_name'];

    $target = Motl::getFilesystemPicturesFolder()."/".$image;

//    $moved = move_uploaded_file( $source, $target );
    $moved = $this->makeThumbnail( $source, $target, 100, 100 );

    if( $moved ) {
      $this->row->avatar = $image;
      $this->saveToDB();
    }
  }

  //================

  private function makeThumbnail( $source, $target, $maxw, $maxh ) {
    $size = getimagesize( $source );

    $scale = min( $maxw/$size[0], $maxh/$size[1] );

    if( $scale >=1 ) {
      return move_uploaded_file( $source, $target );
    }

    $w = (int)($size[0]*$scale);
    $h = (int)($size[1]*$scale);

    $sim = null;
    switch( $size[2] ) {
      case IMAGETYPE_GIF:  $sim=imagecreatefromgif($source);  break;
      case IMAGETYPE_JPEG: $sim=imagecreatefromjpeg($source); break;
      case IMAGETYPE_PNG:  $sim=imagecreatefrompng($source);  break;
    }
    if( ! $sim ) return false;

    $im = imagecreatetruecolor( $w, $h );

    imagecopyresampled( $im , $sim, 0,0, 0,0, $w , $h , $size[0],$size[1] );


    $ok = 0;
    switch( $size[2] ) {
      case IMAGETYPE_GIF:  $ok=imagegif($im,$target);  break;
      case IMAGETYPE_JPEG: $ok=imagejpeg($im,$target); break;
      case IMAGETYPE_PNG:  $ok=imagepng($im,$target);  break;
    }

    imagedestroy($im);

    return $ok;
  }

  //================
}

//=====================
?>
