//--------------------------------------------
//-----  Утилиты для SELECT
//--------------------------------------------

function getListBoxText( obj ) {
  return obj.options[obj.selectedIndex].text
}

function setListBoxText( obj, text ) {

  for( var k=0; k<obj.options.length; k++ ) {
    if( obj.options[k].text == text ) {
      obj.selectedIndex = k;
      break;
    }
  }
}

//--------------------------------------------
//-----  Проверка заполнения
//--------------------------------------------

function testItem( f, name ) {
  return ( f.elements[name].value != '' );
}

function checkItem( f, name, fld ) {
  var ok = ( f.elements[name].value != '' );
  if( ! ok ) alert( "Не заполнено обязательное поле: "+fld )
  return  ok;
}

function checkListItem( f, name, fld ) {
  var ok = ( getListBoxText( f.elements[name] ) != '' );
  if( ! ok ) alert( "Не заполнено обязательное поле: "+fld )
  return  ok;
}

function checkFormCouple( f ) {

 if(  !checkItem(f,"msurname","фамилия партнера")||!checkItem(f,"mname","имя партнера")
//        ||!checkItem(f,"mbirthdate","дата рождения партнера")
      ||!checkItem(f,"fsurname","фамилия партнерши")||!checkItem(f,"fname","имя партнерши")
//        ||!checkItem(f,"fbirthdate","дата рождения партнерши")
      ||!checkListItem(f,"class","класс")
   ) {
    return false;
  }

  var q=false;
  for( k=0; k<checks.length; k++ ) { 
    if( f.elements[checks[k]].checked )
      q = true;
  }

  if( ! q ) {
    alert( 'Не выбрано ни одной программы' );
    return false;
  }

  return true;
}

function checkFormSolo( f ) {

 if(  !checkItem(f,"surname","фамилия")||!checkItem(f,"name","имя")

   ) {
    return false;
  }

  var q=false;
  for( k=0; k<checks.length; k++ ) { 
    if( f.elements[checks[k]].checked )
      q = true;
  }

  if( ! q ) {
    alert( 'Не выбрано ни одной программы' );
    return false;
  }

  return true;
}

//--------------------------------------------
//-----  Поиск подсказки в базе
//--------------------------------------------

var curValue = "";

//-----------------------

function onKeyUp( inp ) {
  if( inp.value != curValue ) {
    curValue = inp.value
    startTip( inp.name, escape(inp.value) )
  }
}

//-----------------------

function startTip( name, value ) {
  var url = 'regFind.php?fieldName='+name+'&fieldValue='+value
  ajaxGET( onFindResult, url );
}

//-----------------------

function onFindResult( t ) {
  document.all['found'].innerHTML = t
}

//-----------------------

function fillFormCouple( msurname,mname,mdate, fsurname,fname,fdate, country,city,club,teacher, class_ ) {

  document.forms.RegForm.elements.msurname.value = msurname
  document.forms.RegForm.elements.mname.value = mname
  document.forms.RegForm.elements.mbirthdate.value = mdate
  
  document.forms.RegForm.elements.fsurname.value = fsurname
  document.forms.RegForm.elements.fname.value = fname
  document.forms.RegForm.elements.fbirthdate.value = fdate
  
  document.forms.RegForm.elements.country.value = country
  document.forms.RegForm.elements.city.value = city
  document.forms.RegForm.elements.club.value = club
  document.forms.RegForm.elements.teacher.value = teacher
  
  if( !class_ ) class_=""
  setListBoxText( document.forms.RegForm.elements["class"], class_ )
}

//-----------------------

function fillFormSolo( surname,name,date, country,city,club,teacher ) {

  document.forms.RegForm.elements.surname.value = surname
  document.forms.RegForm.elements.name.value = name
  document.forms.RegForm.elements.birthdate.value = date
  
  document.forms.RegForm.elements.country.value = country
  document.forms.RegForm.elements.city.value = city
  document.forms.RegForm.elements.club.value = club
  document.forms.RegForm.elements.teacher.value = teacher
}

//-----------------------
