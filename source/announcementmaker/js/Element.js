const baseNames = [
	"Jingle",
	"Next stop",
	"Next stop is on request",
	"This stop is on request",
	"Exit to road",
	"Terminus",
	"Terminus/EN",
	"Line",
	"Transfer to metro",
	"Transfer to rail",
	"Line route change",
	"Warning",
	"The metro station...",
	"...is closed",
	"Train is going to the depot...",
	"From the station...",
	"...is changed to line...",
	"Direction",
	"Funicular closure",
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
	container.classList.add("element");
	container.setAttribute("id", "e"+uniqueId);

	let stationCodeField = document.createElement("input");
	stationCodeField.classList.add("scode");
	stationCodeField.type = "text";
	stationCodeField.placeholder = "XXXX";
	stationCodeField.size = 4;

	let stationNameField = document.createElement("input");
	stationNameField.classList.add("sname");
	stationNameField.type = "text";
	stationNameField.placeholder = "Station name";
	stationNameField.size = 15;

	let stationsLinesField = document.createElement("input");
	stationsLinesField.classList.add("slines");
	stationsLinesField.type = "text";
	stationsLinesField.placeholder = "Lines";
	stationsLinesField.size = 20;

	let stationSpecialLinesField = document.createElement("input");
	stationSpecialLinesField.classList.add("ssplines");
	stationSpecialLinesField.type = "text";
	stationSpecialLinesField.placeholder = "Letter lines";
	stationSpecialLinesField.size = 20;

	let stationAudioField = document.createElement("input");
	stationAudioField.classList.add("sfile");
	stationAudioField.type = "text";
	stationAudioField.placeholder = "Audio file";
	stationAudioField.size = 20;

	let stationOnRequest = document.createElement("input");
	stationOnRequest.classList.add("srequest");
	stationOnRequest.type = "checkbox";

	let labelOnRequest = document.createElement("label");
	labelOnRequest.appendChild(document.createTextNode("Request stop?"));

	let stationHasMetro = document.createElement("input");
	stationHasMetro.classList.add("smetro");
	stationHasMetro.type = "checkbox";

	let labelHasMetro = document.createElement("label");
	labelHasMetro.appendChild(document.createTextNode("Has metro?"));

	let stationHasRail = document.createElement("input");
	stationHasRail.classList.add("srail");
	stationHasRail.type = "checkbox";

	let labelHasRail = document.createElement("label");
	labelHasRail.appendChild(document.createTextNode("Has rail?"));

	let stationMetroClosed = document.createElement("input");
	stationMetroClosed.classList.add("snometro");
	stationMetroClosed.type = "checkbox";

	let labelMetroClosed = document.createElement("label");
	labelMetroClosed.appendChild(document.createTextNode("Is metro closed?"));

	let stationFunicularFail = document.createElement("input");
	stationFunicularFail.classList.add("sfunicular");
	stationFunicularFail.type = "checkbox";

	let labelFunicularFail = document.createElement("label");
	labelFunicularFail.appendChild(document.createTextNode("Announce funicular closure?"));

	let removeButton = document.createElement("button");
	removeButton.setAttribute("id", "b"+uniqueId);
	removeButton.addEventListener("click", (e) => {
		removeElement(e.target.getAttribute("id").slice(1));
	});
	removeButton.appendChild(document.createTextNode("Remove"));
	
	container.appendChild(stationCodeField);
	container.appendChild(stationNameField);
	container.appendChild(stationsLinesField);
	container.appendChild(stationSpecialLinesField);
	container.appendChild(stationAudioField);
	container.appendChild(document.createTextNode(".wav "));
	container.appendChild(document.createElement("br"));
	
	container.appendChild(stationOnRequest);
	container.appendChild(labelOnRequest);
	container.appendChild(stationHasMetro);
	container.appendChild(labelHasMetro);
	container.appendChild(stationHasRail);
	container.appendChild(labelHasRail);
	container.appendChild(stationMetroClosed);
	container.appendChild(labelMetroClosed);
	container.appendChild(stationFunicularFail);
	container.appendChild(labelFunicularFail);
	container.appendChild(document.createElement("br"));
	
	container.appendChild(removeButton);

	container.appendChild(document.createElement("hr"));
	document.getElementById("list").appendChild(container);

	uniqueId++;

	//return for editing
	return [
		stationCodeField,
		stationNameField,
		stationAudioField, 
		stationsLinesField,
		stationSpecialLinesField,
		stationOnRequest,
		stationHasMetro,
		stationHasRail,
		stationMetroClosed,
		stationFunicularFail
	];
}

function removeElement(aId) {
	console.debug("Removing "+aId);
	document.getElementById("e"+aId).remove();
}

function resetElements() {
	uniqueId = 0;
	document.getElementById("list").replaceChildren(); //replace with nothingness
}