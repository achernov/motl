//-------------------------------------------------

function confirmKill() { 
  return confirm('Вы уверены, что хотите удалить этот объект?') 
}

function normCellGo(base,index) {
  var selector = document.all["sel"+index]
  ref = base + selector.options[selector.selectedIndex].value
  location.href = ref
}

function normCellSubmit( fieldValue, nCells ) {

  var s = "";
  var k
  for( k=0; k<nCells; k++ ) {
    var selector = document.all["sel"+k]
    v = selector.options[selector.selectedIndex].text
    if( s.length ) s += ", "
    s += v
  }
  
  document.forms['F'].elements['old_0'].value = fieldValue
  document.forms['F'].elements['new_0'].value = s
  document.forms['F'].submit();
}

function linkerCellSet( idNames, idIds, button, ids ) {
  document.all[idNames].value = button.value;
  document.all[idIds].value   = ids;
/*
  alert(idNames)
  alert(idIds)
  alert(button)
  alert(ids)
*/
}

function textIntoField( idText, idField ) {
  document.all[idField].value = document.all[idText].innerHTML
}

function useReplace( idSel, idField ) {
	var sel = document.all[idSel];
	document.all[idField].value = sel.options[sel.selectedIndex].text;
}

function saveForReplace( fieldName, textIn, textOut ) {
	if( textOut == "" ) {
		return;
	}
	
	url = "baseToolNormalizeField.php?saveForReplace=1";
	url += "&fieldName="+escape(fieldName);
	url += "&textIn="+escape(textIn);
	url += "&textOut="+escape(textOut);
	location.replace( url );
}

//-------------------------------------------------
