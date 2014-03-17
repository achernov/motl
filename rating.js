//=========================================================

var ratingTipHilite = null
var ratingTipOldColor = 'white';

//==============================

function ratingShowTip( place, dx,dy, html ) {

	if( ratingTipHilite ) {
		ratingTipHilite.style.backgroundColor = ratingTipOldColor;
		ratingTipHilite = null;
	}
	ratingTipHilite = place;
	ratingTipOldColor = ratingTipHilite.style.backgroundColor;
	ratingTipHilite.style.backgroundColor = "red";

	var tip = document.all.ratingTip;

	showTip( tip, place, dx,dy, html, onHideRatingTip, 3000 );
}

//==============================

function onHideRatingTip() {
	if( ratingTipHilite ) {
		ratingTipHilite.style.backgroundColor = ratingTipOldColor;
		ratingTipHilite = null;
	}
}

//=========================================================

function onRatingMouse( place, idCouple, idEvent ) {
	//return;
	
	var tip = document.all.ratingTip;
	
	var idCell = idCouple+'_'+idEvent;
	
	var html = "<nobr>"
	
				+ "<b>" + ratingCoupleJSTable[idCouple].title + "</b><p>"
				
				+ "<i>" + ratingEventJSTable[idEvent].title + "</i><p>"
				
				+ "коэффициент: " + ratingEventJSTable[idEvent].coeff + "<p>"
				
				+ "место " + ratingCoupleEventJSTable[idCell].place
					+ " из " + ratingEventJSTable[idEvent].ncouples + "<p>"
					
				+ "Вклад в рейтинг: " + ratingCoupleEventJSTable[idCell].term + "<p>"
					
				+ "</nobr>";

	ratingShowTip( place, 50,0, html )
}

//=========================================================

function onCoupleMouse( place, idCouple ) {
	//return;
	
	var tip = document.all.ratingTip;

	var data = ratingCoupleJSTable[idCouple];
	
	var html = "<nobr>"
	
				+ "<b>" + data.title + "</b><p>"
				
				+ "<b>Клуб:</b> " + data.club + "<p>"
				
				+ "<b>Тренер(ы):</b>&nbsp;<nobr>" + data.teacher + "</nobr><p>"
				
				+ "</nobr>";

	ratingShowTip( place, 50,0, html )
}

//=========================================================

function onEventMouse( place, idEvent ) {
	//return;
	
	var tip = document.all.ratingTip;

	var data = ratingEventJSTable[idEvent];
	
	var html = "<nobr>"
	
				+ "<b>" + data.title + "</b><p>"
				
				+ "<i> " + data.date + "</i><p>"
				
				//+ "коэффициент: " + data.coeff + "<p>"
				
				+ "</nobr>";

	ratingShowTip( place, 50,0, html )
}

//=========================================================
