<?php

//=====================

class DbProducerFactory {

  static private $allowedClasses = 
      array(
			array( "PageContent", "pageContent.inc" ),
			array( "NewsItemProducer", "newsContent.inc" ),
			array( "AnonsItemProducer", "anonsContent.inc" ),
			array( "ForumItemProducer", "forumContent.inc" ),
			array( "GalleryItemProducer", "galleryContent.inc" ),

			array( "DbClubProducer", "dbAll.inc" ),
			array( "DbCityProducer", "dbAll.inc" ),
			array( "DbCountryProducer", "dbAll.inc" ), 
			array( "DbHallProducer", "dbAll.inc" ),
			array( "DbPersonProducer", "dbAll.inc" ),
			array( "DbCoupleProducer", "dbAll.inc" ),
			array( "DbEventProducer", "dbAll.inc" ),
			array( "DbEventProducerFull", "dbEvent.inc" ),
			array( "DbCoupleEventProducer", "dbAll.inc" ),
		);

  public static function createObject( $class ) {

	foreach( DbProducerFactory::$allowedClasses as $pair ) {
		if( $pair[0] == $class ) {
			require_once($pair[1]);
			return new $pair[0];
		}
	}
	
    return null;
  }

}

//=====================
?>

