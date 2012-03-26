// ==UserScript==
// @name           SwissLichens Extractor
// @namespace      naturvielfalt
// @description    Extract all species and their classification from the webpage of swisslichens
// @include        http://merkur.wsl.ch/didado/swisslichens.map
// @include        file:///home/reto/Documents/Programmieren/eclipse-naturwerk/Naturvielfalt/external/master%20files/swisslichens/swisslichens.map

// ==/UserScript==

var allLichens = new Array();
var genusNumber2SubgenusNumbers = new Array();

// Take aryFart and build a cleaned up, nested array
for ( var i in unsafeWindow.aryFart) {
	var genusNumber = unsafeWindow.aryFart[i][0];
	var subgenusNumber = unsafeWindow.aryFart[i][2];

	if (subgenusNumber == -1) { // jump over 'alle Arten / infraspez. Taxa der
		// Gattung'
		continue;
	}

	if (!genusNumber2SubgenusNumbers[genusNumber]) { // create a new array if
		// needed
		genusNumber2SubgenusNumbers[genusNumber] = new Array();
	}
	genusNumber2SubgenusNumbers[genusNumber].push({
		'id' : subgenusNumber
	});
}

var allClassificationGenus = document.getElementsByTagName('option');
for ( var i in allClassificationGenus) {
	if (i == 0 || i == 604) { // jump over entries 'Gattung auswählen' and
		// 'unbekannte'
		continue;
	}
	var genusNumber = allClassificationGenus[i].value;
	var genusName = allClassificationGenus[i].text;
	allLichens[i] = {
		'id' : genusNumber,
		'name' : genusName,
		'subgenus' : genusNumber2SubgenusNumbers[genusNumber]
	// is array
	};
}

//var baseurl0 = 'http://merkur.wsl.ch/didado/swisslichens.map?';
//var regex = /<div id="Titel">\n<div class="subtitle">(.+)<\/div>\n/;
//for ( var i in allLichens) {
//	// fetch from http://merkur.wsl.ch/didado/swisslichens.map
//	// method: POST or GET
//	// parameter: fname=3&fartnr=8251
//	console.log("Done: " + i + " of " + allLichens.length + " genera");
//	var currentGenus = allLichens[i];
//	var baseurl1 = baseurl0 + 'fname=' + currentGenus.id + '&';
//	for ( var j in currentGenus.subgenus) {
//		var req = new XMLHttpRequest();
//		var currentSubgenus = currentGenus.subgenus[j];
//		var finalurl = baseurl1 + 'fartnr=' + currentSubgenus.id;
//		if (req) {
//			var answer = req.open('GET', finalurl, false);
//			req.send(null);
//			if (req.status === 200) {
//				var fullHtmlAnswer = req.responseText;
//				var extractedAnswer = regex.exec(fullHtmlAnswer)[1];
//				currentSubgenus['name'] = extractedAnswer;
//			} else {
//				console.error("Calling " + finalurl + " failed.");
//			}
//		} else {
//			alert(currentGenus.name + " failed");
//		}
//	}
//}

var mydiv = document.createElement('div');
mydiv.setAttribute('id', 'swisslichens');
mydiv.setAttribute(
				'style',
				'z-index: 999999; position: absolute; top: 0; left: 0;  height: 100%; width: 100%; background-color: red; text-align: center; margin: 0 auto;');

var myButton = document.createElement('input');
myButton.setAttribute('type', 'button');
myButton.setAttribute('value',
				'I saved this values into a .csv file and want to close this popup now!');
myButton.setAttribute('onclick',
		"document.getElementById('swisslichens').style.display = 'none';");

var myTextarea = document.createElement('textarea');
myTextarea.setAttribute('style', 'height: 90%; width: 500px');

var innerHTML = 'genus, genusid, subgenusid\n';
for ( var i in allLichens) {
	if (allLichens[i]['name'] == 'unbekannte')
		continue;
	var currentLichen = allLichens[i];
	for ( var j in currentLichen['subgenus']) {
		var currentSubgenus = currentLichen['subgenus'][j];

		innerHTML += currentLichen['name'] + ', ';
		innerHTML += currentLichen['id'] + ', ';
		innerHTML += currentSubgenus['id'] + '\n';
	}
}
myTextarea.innerHTML = innerHTML;
mydiv.appendChild(myTextarea);
mydiv.appendChild(document.createElement('p'));
mydiv.appendChild(myButton);

var suchediv = document.getElementById('Suche');
document.body.appendChild(mydiv);
