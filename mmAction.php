<?php
require_once "_core.inc";
require_once "_motl_db.inc";
require_once "mediaManager.inc";
require_once "_dbproducer.inc";
//=====================

MotlDB::connect();

//=====================

function addItemToGallery( $idGallery, $idItem ) {
  ASSERT_int( $idGallery );
  ASSERT_int( $idItem );

  $nItems = MotlDB::queryValue( "SELECT count(*) FROM gallery WHERE pid='$idGallery'", 0 );  // ASS $idGallery

  echo "$idGallery, $idItem, $nItems<br>";

  $fields = array();
  $fields['pid']       = $idGallery;
  $fields['position']  = $nItems;
//  $fields['folders']   = 
//  $fields['idIcon']    = 
  $fields['idPicture'] = $idItem;
//  $fields['title']     = 

  MotlDB::saveRow( 'gallery', 'id',null, $fields );
}

//=====================

$nItems = Motl::getParameter( "nItems" );
$addToGallery = Motl::getParameter( "addToGallery" );

for( $k=0; $k<$nItems; $k++ ) {
  $ksel = isset( $_REQUEST["dbf_".$k."_sel"] );
  $kid  = Motl::getParameter( "dbf_".$k."_id" );

  if( $ksel ) {
    addItemToGallery( $addToGallery, $kid );
  }

}

Motl::redirect( "gallery.php?id=$addToGallery" );

//=====================
?>
