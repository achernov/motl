<?php
//=====================

class FileUtil {

  //================

  function getDirSubdirs( $path ) {

    $dir = opendir( "$path" );

    $out = array();

    while( ! is_bool( $file=readdir($dir) ) ) {
      if( $file=="." || $file=="..") continue;

      if( is_dir("$path/$file") ) {
        $out[] = $file;
      }
    }

    return $out;
  }

  //================

  function getDirFiles( $path ) {

    $dir = opendir( "$path" );

    $out = array();

    while( ! is_bool( $file=readdir($dir) ) ) {
      if( $file=="." || $file=="..") continue;

      if( ! is_dir("$path/$file") ) {
        $out[] = $file;
      }
    }

    return $out;
  }

  //================
}
?>
