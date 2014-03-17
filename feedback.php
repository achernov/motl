<?php
require_once "forumContent.inc";
require_once "_layout.inc";
//=====================

MotlDB::connect();

//=====================

class FeedbackProducer extends Producer {

  //===========

  private function getLabel( $who ) {
    switch( $who ) {
      case 'admin':      return 'Александр Чернов - админ';

      case 'klyushin':      return 'Николай Клюшин - Президент МО ТЛ';
      case 'belov':         return 'Александр Белов';
      case 'belyak':        return 'Юлия Беляк';
      case 'blinnikov':     return 'Григорий Блинников';
      case 'zolotarevskiy': return 'Алексей Золотаревский';
      case 'kasper':        return 'Денис Каспер';
      case 'mikhalkov':     return 'Игорь Михальков';
      case 'osadchiy':      return 'Василий Осадчий';
      case 'pokrovskaya':   return 'Яна Покровская';
      case 'pozharinskiy':  return 'Сергей Пожаринский';
      case 'shnyrev':       return 'Владимир Шнырев';
    }
    return null;
  }

  //===========

  private function getMail( $who ) {
    switch( $who ) {
      case 'admin':      return 'admin@danceleague.ru';
//      case 'admin':      return 'hryush@mail.ru';

      case 'klyushin':      return 'klyushin63@mail.ru';
      case 'belov':         return 'aksbelov@list.ru';
      case 'belyak':        return 'juliadance@mail.ru';
      case 'blinnikov':     return 'fallaway62@mail.ru';
      case 'zolotarevskiy': return 'zolotoaleks@yandex.ru';
      case 'kasper':        return 'deniskasper@yandex.ru';
      case 'mikhalkov':     return 'igor-m@inbox.ru';
      case 'osadchiy':      return 'osadchiyvasiliy@mail.ru';
      case 'pokrovskaya':   return 'yanikkoz@list.ru';
      case 'pozharinskiy':  return 'SergioPo@bk.ru';
      case 'shnyrev':       return 'v_shnyrev@mail.ru';
    }
    return null;
  }

  //===========

  public function produce() {

    if( Motl::getParameter( "result" ) ) {
      return $this->produceResult();
    }
    else
    if( Motl::getParameter( "from" ) ) {
      return $this->sendIt();
    }
    else {
      return $this->produceForm();
    }

  }

  //===========

  public function produceResult() {

    $result = Motl::getParameter( "result" );
    $s  = "";

    $s .= "<h1>Обратная связь</h1>";

    if( $result!="error" ) {
      $s .= "Ваше сообщение отправлено";
    }
    else {
      $s .= "Не удалось отправить";
    }

    return $s;
  }

  //===========

  public function produceForm() {

    $script = $_SERVER['PHP_SELF'];

    $recipient = Motl::getParameter( "recipient" );

    $label = $this->getLabel( $recipient );

    $s  = "";

    $s .= "<h1>Обратная связь</h1>";

    $s .= "<form name='F' method='POST' action='$script' onsubmit='return checkSubmit()'>";
    $s .= "<input type='hidden' name='recipient' value='$recipient'>";
    
    $s .= "<table>";

    $s .= "<tr><td class='MainText'> Ваше имя: </td>"
             ."<td class='MainText'> <input type='text' style='width:320' name='name' > </td></tr>";

    $s .= "<tr><td class='MainText'> Ваш E-Mail: </td>"
             ."<td class='MainText'> <input type='text' style='width:320' name='from' > </td></tr>";

    $s .= "<tr><td class='MainText'> Тема сообщения: </td>"
             ."<td class='MainText'> <input type='text' style='width:320' name='subj' > </td></tr>";

    $s .= "<tr><td class='MainText'> Получатель: </td>"
             ."<td class='MainText'> <select style='width:320' ><option>$label</select> </td></tr>";

    $s .= "<tr><td class='MainText'> Сообщение: </td>"
             ."<td class='MainText'> <textarea style='width:320; height:150' name='text' ></textarea> </td></tr>";

    $s .= "<tr> <td></td> <td class='MainText'> <input type='submit' value='Отправить' > </td>";

    $s .= "</table>";

    $s .= "</form>";

    return $s;
  }

  //===========
  
  private function qpEncode( $s ) {
    $out = "";
    for( $k=0; $k<strlen($s); $k++ ) {
      $out .= $this->qpEncodeChar( $s[$k] );
    }
    return $out;
  }
  
  private function qpEncodeChar( $c ) {
    $code = strtoupper( dechex( ord($c) ) );
    return "=$code";
  }
  
  //===========

  public function sendIt() {
    $recipient = Motl::getParameter( "recipient" );

    $mail = $this->getMail( $recipient );

    $name = Motl::getParameter( "name" );
    $from = Motl::getParameter( "from" );
    $subj = Motl::getParameter( "subj" );
    $text = Motl::getParameter( "text" );
//$subj = quoted_printable_encode ( $subj );
//$subj = $this->qpEncode( $subj );

$encodeParams = array(
    "input-charset" => "UTF-8",
    "output-charset" => "UTF-8",
    "line-length" => 76,
    "line-break-chars" => "\n"
);

$subj1 = iconv_mime_encode( "Subject", $subj, $encodeParams );
$subj2 = substr( $subj1, 9 );
//die($subj1);
    $headers = 
      "Content-Type: text/plain; charset=utf-8"
//      ."\r\nSubject: $subj"
//      ."\r\n$subj1"
      ."\r\nFrom: $from";
    
//    $res = mail( $mail, $subj, $text, $headers );
    $res = mail( $mail, $subj2, $text, $headers );
    $res = $res?"ok":"error";

    Motl::redirect( "feedback.php?result=$res" );
  }

  //===========
}

//=====================

$content = new FeedbackProducer();

$layout = new MotlLayout(); 
$layout->mainPane = $content;

$layout->write();


//=====================
?>

<script>

function checkSubmit() {
  if( emptyItem("name")||emptyItem("from")||emptyItem("subj")||emptyItem("text") ) {
    alert("Заполнены не все поля формы");
    return false;
  }
  return true;
}

function emptyItem( item ) {
  return ( document.forms.F.elements[item].value == "" )
}

</script>

