//=========================================================

function spreadColumn( ctl, dance ) {

  for( var nRows=0; nRows<1000; nRows++ ) {
    var fid = 'dbf_'+nRows+'_d'+dance;
    if( document.all[fid] == undefined ) break;
  }
//alert(nRows)
  normalizeScores(ctl)
  var s = ctl.value;
  sa = s.split( '\n' );
  
  if( sa.length < nRows ) return
  //var nJudges = s.length/nRows
  
  for( var k=0; k<nRows; k++ ) {
    //var p = k*nJudges
    //var q = s.substr(p,nJudges)
    var q = sa[k]
    var fid = 'dbf_'+k+'_d'+dance;
    document.all[fid].value = q
  }
}

function spreadScores( ctl, rowNumber, nDances ) {
  normalizeScores(ctl)
  var s = ctl.value;
  if( s.length < nDances*5 ) return
  //alert( ctl.name )
  var nJudges = s.length/nDances
  //alert(nJudges);
  for( var k=0; k<nDances; k++ ) {
    var p = k*nJudges
    var q = s.substr(p,nJudges)
    //alert( document.all['dbf_'+rowNumber+'_d'+(k+1)].value  );
    document.all['dbf_'+rowNumber+'_d'+(k+1)].value = q
  }
  ctl.value = ""
  // 12345671234567123456712345671234567
  //alert( 'dbf_'+rowNumber+'_d'+nDances  )
}

function normalizeScores( ctl ) {
  var s = ctl.value;
  if( s =="") return
  var a = s.split( '\t' );
  var s1 = "";
  for( var k=0; k<a.length; k++ ) {
    var ai = a[k];
    s1 += (ai!="") ? ai : "-";
  }
  ctl.value = s1;
}

function toggleOneSel(name) {
  document.forms[0].elements[name].checked = ! document.forms[0].elements[name].checked
}

function toggleSels( prefix,suffix,start) {
  var name0 = prefix+start+suffix;
  var newstate = ! document.forms[0].elements[name0].checked
  for( var k=0; k<1000; k++ ) {
    var namei = prefix+(start+k)+suffix;
    if( document.forms[0].elements[namei] == undefined ) break;
    document.forms[0].elements[namei].checked = newstate
  }
}

function calcSoloAvg(nRows,nDances) {
  for( var r=0; r<nRows; r++ ) {
    calcSoloAvgRow(r,nDances);
  }
}

function calcSoloAvgRow(rowNumber,nDances) {
  var sum = 0;
  for( var k=0; k<nDances; k++ ) {
    var dScores = document.all['dbf_'+rowNumber+'_d'+(k+1)].value
    var dAvg = strAvg(dScores);
    //return;
    sum += 1.0*dAvg;
//   alert( "rowNumber="+rowNumber+", dance="+(k+1)+", dScores="+dScores+", dAvg="+dAvg )
  }
  
  var avg = tenDiv( sum, nDances );
  //alert( "rowNumber="+rowNumber+", nDances="+nDances+", avg="+avg )
  document.all['dbf_'+rowNumber+'_place'].value = formatTenths(avg); //avg
}

function formatTenths(v) {
  if( v == Math.floor(v) ) {
    v = Math.floor(v)+".0"
  }
  return v;
}

function strAvg(s) {
  var sum = 0;
  for( k=0; k<s.length; k++ ) {
//  alert(k+"@"+s.substr(k,1));
    sum += (1.0*s.substr(k,1));
  }
 // alert("sum="+sum)
  //return sum/s.length;
  return tenDiv(sum,s.length);
}

function tenDiv(sum,n) {
  var v = sum*10;
  var v2 = v*2;
  var x = (v2-v2%n)/n
  var y = (x>>1) + ((x&1)?1:0);
  return y/10;
}

function setScoreSpans(ids,ndances) {
	for( var k=0; k<ids.length; k++ ) {
		for( var i=0; i<ndances; i++ ) {
			var regId = ids[k];
			var t = "scores_"+regId+"_"+i;
			if( ! document.all[t] ) {
				return
			}
			var spanText = document.all[t].innerHTML;
			var s = ""
			for( var j=0; j<spanText.length; j++ ) {
				var score = spanText.substr(j,1);
				s += "<span style='cursor:default;' onmouseover='showScoreTip(this,"+regId+","+i+","+j+",\""+score+"\")'>"+score+"</span>"
			}
			document.all[t].innerHTML = s
		}
	}
}

//=========================================================

var hiliteSpan = null;

//---------------------------------------------------------

function showScoreTip( place, id, dance, judge, score ) {
	var scoreTip = document.all.scoreTip;
	
	var scoreHtml = "<nobr>"
		+"<span class='tipScore'>"+score+"</span>"
		+"<span class='tipJudge'>"+judges[judge]+"</span>"
		+"</nobr>";
		
	if( hiliteSpan ) {
		hiliteSpan.style.backgroundColor="white";
	}
	hiliteSpan = place
	hiliteSpan.style.backgroundColor="red";

	showTip( scoreTip, place, 10,-1, scoreHtml, onHideScoreTip );
//	showTip( scoreTip, place, 10,-20, scoreHtml, onHideScoreTip );
//	showTip( scoreTip, place, 10,-20, scoreHtml, function(){onHideScoreTip()} );
}

//---------------------------------------------------------

function onHideScoreTip() {
	if( hiliteSpan ) {
		hiliteSpan.style.backgroundColor="white";
	}
}

//=========================================================
