const baseNames = [
	"Next stop",
	"On request",
	"Exit to road",
	"Terminus",
	"Terminus/EN",
	"Transfer to metro",
	"Transfer to rail",
	"Warning",
	"The metro station...",
	"...is closed",
	"Train is going to the depot...",
	"...is changed to line...",
	"From the station...",
	"Direction",
	"Cablecar closure",
];

const BASE_NAMES_AMOUNT = baseNames.length;

function makeBase() {
	let staticElem = document.getElementById("static");
	baseNames.forEach((v, i, a) => {
		let ielem = document.createElement("input");
		ielem.type = "text";
		ielem.name = "b"+String(i);
		ielem.placeholder = "Filename (no extension)";
		ielem.setAttribute("id", "b"+String(i));

		staticElem.appendChild(document.createTextNode(baseNames[i]+": "));
		staticElem.appendChild(ielem);
		staticElem.appendChild(document.createElement("br"));
	});
	staticElem.appendChild(document.createElement("hr"));
}

//converted to list only when serializing

let uniqueId = 0;

function newElement() {
	let container = document.createElement("div");
	container.setAttribute("class", "element");
	container.setAttribute("id", "e"+uniqueId);

	let stationCodeField = document.createElement("input");
	stationCodeField.class = "scode";
	stationCodeField.type = "text";
	stationCodeField.placeholder = "XXXX";
	stationCodeField.size = 4;

	let stationNameField = document.createElement("input");
	stationCodeField.class = "sname";
	stationNameField.type = "text";
	stationNameField.placeholder = "Station name";
	stationNameField.size = 15;

	let stationsLinesField = document.createElement("input");
	stationCodeField.class = "slines";
	stationsLinesField.type = "text";
	stationsLinesField.placeholder = "Lines";
	stationsLinesField.size = 20;

	let stationAudioField = document.createElement("input");
	stationAudioField.class = "sfile";
	stationAudioField.type = "text";
	stationAudioField.placeholder = "Audio file";
	stationAudioField.size = 10;

	let stationHasMetro = document.createElement("input");
	stationHasMetro.class = "smetro";
	stationHasMetro.type = "checkbox";

	let labelHasMetro = document.createElement("label");
	labelHasMetro.appendChild(document.createTextNode("Has metro?"));

	let stationHasRail = document.createElement("input");
	stationHasRail.class = "srail";
	stationHasRail.type = "checkbox";

	let labelHasRail = document.createElement("label");
	labelHasRail.appendChild(document.createTextNode("Has rail?"));

	let cablecarFail = document.createElement("input");
	cablecarFail.class = "scable";
	cablecarFail.type = "checkbox";

	let labelCablecarFail = document.createElement("label");
	labelCablecarFail.appendChild(document.createTextNode("Announce cablecar closure?"));
	
	let removeButton = document.createElement("button");
	removeButton.setAttribute("id", "b"+uniqueId);
	removeButton.addEventListener("click", (e) => {
		removeElement(e.target.getAttribute("id").slice(1));
	});
	removeButton.appendChild(document.createTextNode("Remove"));
	
	container.appendChild(stationCodeField);
	container.appendChild(stationNameField);
	container.appendChild(stationsLinesField);
	container.appendChild(stationAudioField);
	container.appendChild(document.createTextNode(".wav "));
	container.appendChild(document.createElement("br"));
	
	container.appendChild(stationHasMetro);
	container.appendChild(labelHasMetro);
	container.appendChild(stationHasRail);
	container.appendChild(labelHasRail);
	container.appendChild(cablecarFail);
	container.appendChild(labelCablecarFail);
	container.appendChild(document.createElement("br"));
	
	container.appendChild(removeButton);

	container.appendChild(document.createElement("hr"));
	document.getElementById("list").appendChild(container);

	uniqueId++;
}

function removeElement(aid) {
	console.debug("Removing "+aid);
	document.getElementById("e"+aid).remove();
}

function resetElements() {
	uniqueId = 0;
	document.getElementById("list").replaceChildren(); //replace with nothingness
}