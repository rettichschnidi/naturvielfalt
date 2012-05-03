// ==UserScript==
// @name           SwissLichens Extractor
// @namespace      naturvielfalt
// @description    Extract all species and their classification from the webpage of swisslichens
// @include        http://merkur.wsl.ch/didado/swisslichens.map
// @include        file:///home/reto/Documents/Programmieren/eclipse-naturwerk/Naturvielfalt/external/master%20files/swisslichens/swisslichens.map

// ==/UserScript==

var allLichens = new Array();
var genusNumber2speciesNumbers = new Array();

// Take aryFart and build a cleaned up, nested array
for ( var i in unsafeWindow.aryFart) {
	var genusNumber = unsafeWindow.aryFart[i][0];
	var speciesName = unsafeWindow.aryFart[i][1];
	var speciesNumber = unsafeWindow.aryFart[i][2];

	if (speciesNumber == -1) { // jump over 'alle Arten / infraspez. Taxa der
		// Gattung'
		continue;
	}

	if (!genusNumber2speciesNumbers[genusNumber]) { // create a new array if
		// needed
		genusNumber2speciesNumbers[genusNumber] = new Array();
	}
	genusNumber2speciesNumbers[genusNumber].push({
		'id' : speciesNumber,
		'name': speciesName
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
		'species' : genusNumber2speciesNumbers[genusNumber]
	// is array
	};
}

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

var innerHTML = 'genus, genusid, species, speciesid\n';
for ( var i in allLichens) {
	if (allLichens[i]['name'] == 'unbekannte')
		continue;
	var currentLichen = allLichens[i];
	for ( var j in currentLichen['species']) {
		var currentSpecies = currentLichen['species'][j];

		innerHTML += currentLichen['name'] + ', ';
		innerHTML += currentLichen['id'] + ', ';
		innerHTML += currentSpecies['name'] + ', ';
		innerHTML += currentSpecies['id'] + '\n';
	}
}
myTextarea.innerHTML = innerHTML;
mydiv.appendChild(myTextarea);
mydiv.appendChild(document.createElement('p'));
mydiv.appendChild(myButton);

var suchediv = document.getElementById('Suche');
document.body.appendChild(mydiv);
