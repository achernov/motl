<?php
require_once "_core.inc";
//=====================

$script = Motl::getParameter( "script" );
$id     = Motl::getParameter( "id" );

$image = $_POST["image"];
$target = Motl::getFilesystemPicturesFolder()."/".$image;
$source = $_FILES['userfile']['tmp_name'];

$moved = move_uploaded_file( $source, $target );

echo $moved ? "Файл успешно загружен<p>" : "Не удалось загрузить<p>";

echo "<A href=uploadImage.php?image=$image&id=$id&script=$script>Загрузить заново</a><p>";

echo "<A href=$script?id=$id>Вернуться к редактированию</a><p>";

die();

//=====================

Motl::redirect( "$script?id=$id" );

//=====================
?>
