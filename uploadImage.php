<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "_layout.inc";
//=====================

class UploadFormProducer extends Producer {

  public function produce() {

    $image  = Motl::getParameter( "image" );
    $script = Motl::getParameter( "script" );
    $id     = Motl::getParameter( "id" );

    $s = "";
    $s = "<h1>Загрузка недостающего изображения</h1>";
    $s .= "<form enctype='multipart/form-data' action='uploadImageReceiver.php' method='POST'>\n";
    $s .= "    Нужен файл: $image<p>\n";
    $s .= "    <input type='hidden' name='image'  value='$image' />\n";
    $s .= "    <input type='hidden' name='script' value='$script' />\n";
    $s .= "    <input type='hidden' name='id'     value='$id' />\n";
    $s .= "    <input type='hidden' name='MAX_FILE_SIZE' value='300000' />\n";
    $s .= "    Выберите его: <input name='userfile' type='file' /><p>\n";
    $s .= "    <input type='submit' value='Send File' />\n";
    $s .= "</form>\n";
    return $s;
  }
}

//=====================

MotlDB::connect();

$layout = new MotlLayout(); 
$layout->mainPane = new UploadFormProducer();

$layout->write();

//=====================
?>
