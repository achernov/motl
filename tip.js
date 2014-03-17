//=========================================================

var tipDiv = null;
var tipTimer = null;
var tipOnHide = null;

//---------------------------------------------------------

function showTip( div, place, dx,dy, html, onHide, delay ) {

	var px = getX(place)+dx;
	var py = getY(place)+dy;
	var pw = place.clientWidth;
	var ph = place.clientHeight;

	var maxX = document.body.scrollLeft + document.body.clientWidth;
	var maxY = document.body.scrollTop + document.body.clientHeight;
	
	tipDiv = div;
	tipDiv.style.visibility = 'hidden';
	tipDiv.style.display = 'block';
	tipDiv.innerHTML = html;
	var tw = tipDiv.clientWidth;
	var th = tipDiv.clientHeight;
	
	var tx = px// + ( (px>=0) ? pw : -tw );
	var ty = py + ( (dy>=0) ? ph : -th );
	
	if( tx+tw >= maxX ) {
		tx = maxX-tw-5;
	}
	
	if( ty+th >= maxY ) {
		ty = py-th;
	}
	
	tipDiv.style.left = tx;
	tipDiv.style.top = ty;
	tipDiv.style.visibility = 'visible';
	
	if( tipTimer ) {
		clearTimeout(tipTimer); 
	}
	
	if( ! delay ) delay = 1000;
	tipTimer = setTimeout( 'hideTip()', delay );
	tipOnHide = onHide
	//place.style.backgroundColor='red'
}

//---------------------------------------------------------

function hideTip() {
	tipDiv.style.display = 'none';
	if( tipOnHide ) {
		tipOnHide()
	}
}

//=========================================================
