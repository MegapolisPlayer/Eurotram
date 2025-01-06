const METRO_STATE = {
	NONE: 0,
	NORMAL: 1,
	ISSUE: 2
};

class StationEntry {
	stationCode = "";
	stationName = "";
	stationAudioFile = "";

	hasMetro = false;
	metroClosed = false;

	railTransfer = false;

	announceCablecarFail = false;

	requestStop = false;
};

//converted to list only when serializing

function newElement() {
	let container = document.createElement("div");
	container.setAttribute("class", "element");

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
	stationHasMetro.class = "scable";
	stationHasMetro.type = "checkbox";

	let labelcablecarFail = document.createElement("label");
	labelHasMetro.appendChild(document.createTextNode("Announce cablecar closure?"));

	let playButton = document.createElement("button");
	playButton.appendChild(document.createTextNode("Current stop"));
	
	let nextButton = document.createElement("button");
	nextButton.appendChild(document.createTextNode("Next stop"));
	
	let removeButton = document.createElement("button");
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
	container.appendChild(document.createElement("br"));
	
	container.appendChild(playButton);
	container.appendChild(nextButton);
	container.appendChild(removeButton);

	container.appendChild(document.createElement("hr"));
	document.getElementById("list").appendChild(container);
}

function removeElement(aid) {

}

function resetElement() {

}

function updateList() {

}

//TODO add "test announcement" and "test next stop announcement"