<?php
//=====================
require_once "regTypeCouple.inc";
require_once "regTypeSolo.inc";
require_once "regTypeSoloComp.inc";
require_once "regTypeTeam.inc";
require_once "regTypeSeminar.inc";
//=====================
//die("regManager.inc");
//=====================

class RegManager {

  //===========
  
  private static $AllHandlers = null;
  
  private static function getAllHandlers() {
    if( ! RegManager::$AllHandlers ) {
      RegManager::$AllHandlers = array( new RegTypeHandlerCouple(), new RegTypeHandlerSolo(), new RegTypeHandlerSoloComp(), new RegTypeHandlerTeam(), new RegTypeHandlerSeminar() );
    }
    return RegManager::$AllHandlers;
  }

  //===========
  //Operation: "regform" / "reglist" / "results" / "accept"

  public static function getHandlerForCode( $code, $operation ) {
  
    $allHandlers = RegManager::getAllHandlers();

    for( $k=0; $k<count($allHandlers); $k++ ) {
      $handler = $allHandlers[$k]; // -> RegTypeHandler;
      if( $handler->codeIsMine($code,$operation) ) {
        return $handler;
      }
    }
    return null;
  }
  
  //===========
  
  public static function getHandlerArrayForCodes( $typeCodes, $operation ) {
  
    $usedHandlers = array();
    
    $allHandlers = RegManager::getAllHandlers();

    for( $k=0; $k<count($allHandlers); $k++ ) {
      $handler = $allHandlers[$k]; // -> RegTypeHandler;
      $needed = false;
      for( $i=0; $i<count($typeCodes); $i++ ) {
        if( $handler->codeIsMine($typeCodes[$i],$operation) ) {
          $needed = true;
        }
      }
      if( $needed ) {
        $usedHandlers[] = $handler;
      }
    }

    return $usedHandlers;
  }
  
  //===========
  
  public static function makeOptions( $typeCodes ) {
    $usedHandlers = RegManager::getHandlerArrayForCodes( $typeCodes, "regform" );
    $options = array();
    
    if( count($usedHandlers) == 1 ) {
      $options[] = (object)array( "label"=>null, "code"=>($usedHandlers[0]->getCanonicalCode()) );
    }
    else {
      for( $k=0; $k<count($usedHandlers); $k++ ) {
        $handler = $usedHandlers[$k];
        $options[] = (object)array( "label"=>$handler->getLabel(), 
                                    "code"=>($handler->getCanonicalCode()) );
      }
    }
    
    //print_r($options);
    
    return $options;
  }
  
  
  //===========
}

//=====================

?>
