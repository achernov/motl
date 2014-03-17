<?php
//=====================

class ForumUtil {

  //================

  public static function makeFilterCondition( $user, $tableAlias=null ) { // returns Safe SQL
  
    ASSERT_alnum($tableAlias);
  
    $tableAlias = $tableAlias ? $tableAlias : "forum";
    if( !$user || !$user->row || !$user->row->rights ) {
      return "($tableAlias.exclusive Is Null)";
    }
    
    if( $user->checkRight("admin") ) {
        return "true";
    }

    $arights = explode( ",", $user->row->rights );
    
    $brights = array();
    for( $k=0; $k<count($arights); $k++ ) {
      $brights[] = MRES( $arights[$k] );
    }
    
    $exclusive = "'" . implode( "','", $brights ) . "'";
    $cond = "($tableAlias.exclusive Is Null or $tableAlias.exclusive In($exclusive))";
    return $cond;
  }
  
  //================

  public static function updateLevel( $id, $currentDepth=0 ) {
    if( $currentDepth > 5 ) return;

    ASSERT_alnum( $id );
    $row = MotlDB::queryRow( "SELECT * FROM forum WHERE id=$id" );

    if( ! $row ) return;
    if( ! isset($row->type) ) return;

    switch( $row->type ) {
      case 'topic':
        ForumUtil::updateTopic( $row ); break;

      case 'section':
        ForumUtil::updateSection( $row ); break;
    }

    if( isset($row->pid) ) {
      $pid = $row->pid;
      if( $pid ) {
        ForumUtil::updateLevel( $pid, $currentDepth+1 );
      }
    }

  }

  //----------------

  private static function updateTopic( $row ) {
    $id = $row->id;
Q( "updateTopic $id" );
    ASSERT_alnum( $id );

    $stat = MotlDB::queryRow( "SELECT count(*) AS nPosts, Max(id) as idLast  FROM forum WHERE pid=$id" );

    $row->nPosts  = $stat->nPosts;
    
    if( $stat->idLast ) {
      $lastPost = MotlDB::queryRow( "SELECT * FROM forum WHERE id=" . $stat->idLast );

      $row->updatedWhen = $lastPost->createdWhen;
      $row->updatedWho  =	$lastPost->createdWho;
      $row->lastPostId  =	$lastPost->id;
      $row->lastPostTitle = $lastPost->title;
    }
    else {
      $row->updatedWhen = null;
      $row->updatedWho  = null;
      $row->lastPostId  = null;
      $row->lastPostTitle = null;
    }

    $object = new ForumItemProducer( $row );
    $object->saveToDB();//saveRowToDB();

  }

  //================

  private static function updateSection( $row ) {
    $id = $row->id;
Q( "updateSection $id" );
    ASSERT_alnum( $id );

    $stat = MotlDB::queryRow( "SELECT count(*) AS nTopics, Sum(nPosts) AS nPosts, Max(lastPostId) AS lastPostId FROM forum WHERE pid=$id" );  // ASS $id

    $lastPost = MotlDB::queryRow( "SELECT * FROM forum WHERE id=" . $stat->lastPostId );  // FLD $stat->lastPostId

    $lastTopic = MotlDB::queryRow( "SELECT * FROM forum WHERE id=" . $lastPost->pid );  // FLD $lastPost->pid

    $row->updatedWhen = $lastPost->createdWhen;
    $row->updatedWho  =	$lastPost->createdWho;
    $row->lastPostId  =	$lastPost->id;
    $row->lastPostTitle = $lastTopic->title;
    $row->nPosts  = $stat->nPosts ? $stat->nPosts : 0;  // replace null with 0
    $row->nTopics = $stat->nTopics;

    $object = new ForumItemProducer( $row );
    $object->saveToDB();//saveRowToDB();
  }

  //================

  public static function cleanupOrphans() {
// SELECT fl.id,fl.pid,fh.id FROM forum AS fl LEFT JOIN forum AS fh ON fl.pid=fh.id WHERE fh.id Is Null AND fl.type<>'root'

    for( $k=0; $k<100; $k++ ) {

      $orphans = MotlDB::queryValues( "SELECT fl.id FROM forum AS fl LEFT JOIN forum AS fh ON fl.pid=fh.id WHERE fh.id Is Null AND fl.type<>'root' LIMIT 100", 0 );

      $ids = implode( ",", $orphans );

      if( $orphans ) {
        MotlDB::query( "DELETE FROM forum WHERE id in($ids)" );  // FLD $ids
      }
      else {
        return;
      }
    }
  }

  //================

}
?>
