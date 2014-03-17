<?php
require_once "_dbproducer.inc";
//=====================

class User extends DbProducer  {

  //---------------------

  public function getId() {
    return $this->row->id;
  }

  //---------------------

  public function getName() {
    return $this->row->login;
  }

  //---------------------

  public function getStatusText() {

    switch( $this->row->userState ) {

      case 'NotActivated':
        return '<i>Не активирован</i>';

      case 'Banned':
        return '<i>Забанен</i>';

      case 'active':
        if( $this->row->status )
          return $this->row->status;

        return 'Пользователь';

    }
    return null;
  }

  //---------------------

  public function checkRight( $right ) {
    $rights = "," . $this->row->rights . ",";

    if( is_int( strpos( $rights, ",admin," ) ) ) return true;

    if( is_int( strpos( $rights, ",".$right."," ) ) ) return true;

    return false;
  }

  //---------------------

  private static $theCurrentUserObject = null;
  
  public static function getCurrentUID() { // returns Safe int
    $cookieUid = isset($_COOKIE["motlUid"]) ? $_COOKIE["motlUid"] : null;
    ASSERT_int( $cookieUid );
    return is_null($cookieUid) ? null : (int)$cookieUid;
  }
  
  public static function getCurrentUser() {
    $cookieUid = isset($_COOKIE["motlUid"]) ? $_COOKIE["motlUid"] : null;
    if( $cookieUid ) {
      ASSERT_int( $cookieUid );
      if( User::$theCurrentUserObject ) {
        return User::$theCurrentUserObject;
      }
      else {
        $row = MotlDb::queryRow( "SELECT * FROM users WHERE id='$cookieUid'" );
        User::$theCurrentUserObject = $row ? new User($row) : null;
        return User::$theCurrentUserObject;
      }
    }
    return null;
  }

  //----------------

  public static function isAdmin() {
	if( Motl::getParameter( "simuser", 0 ) ) {
		return false;
	}
    return User::isAdminReal();
  }

  //----------------

  public static function isAdminReal() {
    $user = User::getCurrentUser();

    if( $user ) {
      if( $user->checkRight("admin") ) {
        return true;
      }
    }
    return false;
  }

  //----------------

  public static function isPresidium() {
    $user = User::getCurrentUser();

    if( $user ) {
      if( $user->checkRight("presidium") ) {
        return true;
      }
    }
    return false;
  }

  //----------------

  public static function canEdit() {
    $user = User::getCurrentUser();

    if( $user ) {
      if( $user->checkRight("editor") ) {
        return true;
      }
    }
    return false;
  }

  //----------------
  
  public function isUserMe() { // It's me
    $curUser = User::getCurrentUser();
    $itsMe = ( $curUser && $this->row && $curUser->row->id==$this->row->id );
    return $itsMe;
  }
  
  //----------------
  
  public function isUserEditable() { // It's me or I'm Admin
    $curUser = User::getCurrentUser();
    $itsMe = ( $curUser && $this->row && $curUser->row->id==$this->row->id );
    $canEdit = $itsMe || ( $curUser && $curUser->checkRight("admin") );
    return $canEdit;
  }
  
  //----------------

  public function getTableName() {
    return "users";
  }

  //----------------

  public function getFieldNames() {
    return array( "id", "login", "userState", "rights", "passHash", "registrationDate", "mail", "status", "avatar", "name", "surname", "activationCode" );
  }

  //----------------
}

//=====================
?>