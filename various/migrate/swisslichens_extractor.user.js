// ==UserScript==
// @name           SwissLichens Extractor
// @namespace      naturvielfalt
// @description    Extract all species and their classification from the webpage of swisslichens
// @include        http://merkur.wsl.ch/didado/swisslichens.map
// @include        file:///home/reto/Documents/Programmieren/eclipse-naturwerk/Naturvielfalt/external/master%20files/swisslichens/swisslichens.map

// ==/UserScript==

//alert("aryFart:" + document.getElementsByTagName('select')[0].childNodes.length);
var allLichens = new Array();
var speciesName2ClassificationNumber = new Array();
var classificationNumber2SpeciesName = new Array();

for(var i in unsafeWindow.aryFart) {
	var speciesName = unsafeWindow.aryFart[i][1];
	var classificationNumber = unsafeWindow.aryFart[i][0];

	if(!classificationNumber2SpeciesName[classificationNumber]) {
		classificationNumber2SpeciesName[classificationNumber] = new Array();
	}
	classificationNumber2SpeciesName[classificationNumber].push(speciesName);
}

var allClassifications = document.getElementsByTagName('option');
for(var i in allClassifications) {
	if(i == 0) {
		continue;
	}
	var classificationNumber = allClassifications[i].value;
	var classificationName = allClassifications[i].text;
	j++;
	allLichens[i] = {
		'classificationName' : classificationName,
		'classificationNumber' : classificationNumber2SpeciesName[classificationNumber] // is array
	};
}

var mydiv = document.createElement('div');
mydiv.setAttribute('id', 'swisslichens');
mydiv.setAttribute('style', 'z-index: 999999; position: absolute; top: 0; left: 0;  height: 100%; width: 100%; background-color: red; text-align: center; margin: 0 auto;');

var myButton = document.createElement('input');
myButton.setAttribute('type', 'button');
myButton.setAttribute('value', 'I saved this values into a .csv file and want to close this popup now!');
myButton.setAttribute('onclick', "document.getElementById('swisslichens').style.display = 'none';");

var myTextarea = document.createElement('textarea');
myTextarea.setAttribute('style', 'height: 90%; width: 500px');

var innerHTML = 'familyname, species\n';
for(var i in allLichens) {
	if(allLichens[i]['classificationName'] == 'unbekannte')
		continue;
	for(var j in allLichens[i]['classificationNumber']) {
		innerHTML += allLichens[i]['classificationName'] + ', ';
		innerHTML += allLichens[i]['classificationNumber'][j] + '\n';
	}
}
myTextarea.innerHTML = innerHTML;
mydiv.appendChild(myTextarea);
mydiv.appendChild(document.createElement('p'));
mydiv.appendChild(myButton);

var suchediv = document.getElementById('Suche');
document.body.appendChild(mydiv);
