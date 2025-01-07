//point in timetable which the tram must follow
//can be different for each line
//time as second offset from beginning of scenario
function ControlPoint(acode, atime) {
	this.point = acode;
	this.time = atime;
	return this;
}

class Line {
	stationCodes = [];
	controlpoints = [];
	switches = []; //how must switches be set

	loops = 0; //loops of line

	startStation = []; //start station

	maintenanceRouteStart = []; //if starting at depot
	maintenanceRouteEnd = []; //go to depot at end

	constructor() {}
};

let lineList = [];

let fileFormatVersion = 0;

function lineUndo() {
	lineList.pop();
}	

function lineAddOnClick() {
	console.log("Line edit click!");

	//we added something - we can undo
	document.getElementById("lineundo").disabled = false;
}

//TODO remove innerHTMLs!!!!
function newLineCreate() {
	currentMode = mode.VIEW;
	canvasData.mode.innerHTML = "Adding new line...";

	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "<em>Click on all of the nodes where the tram will go.</em><br>";
	
	let nlist = document.getElementsByClassName("menubtn");
	for(let i = 0; i < nlist.length; i++) {
		nlist.item(i).disabled = true;
	}

	canvasData.element.addEventListener("click", lineAddOnClick);

	canvasData.edit.innerHTML += "<button onclick='lineEnd()'>Exit</button>";
	
	let undoButton = document.createElement("button");
	undoButton.disabled = true;
	undoButton.onclick = lineUndo;
	undoButton.appendChild(document.createTextNode("Undo"));
	canvasData.edit.appendChild(undoButton);

	canvasData.edit.innerHTML += "<button onclick='lineSerialize()'>Done</button>";
}

function lineSerialize() {
	let numberValuesArray = [];

	//use same utils from Map.js

	//TODO

	//convert to blob

	let binaryArray = new Uint8Array(numberValuesArray.length);
	binaryArray.forEach((v, i, a) => {
		a[i] = numberValuesArray[i];
	})

	//blobs are immutable
	let blob = new Blob([binaryArray], {type: "application/octet-stream"}); //arbitrary binary data

	return blob;
}

function lineEnd() {
	canvasData.element.removeEventListener("click", lineAddOnClick);
	let nlist = document.getElementsByClassName("menubtn");
	for(let i = 0; i < nlist.length; i++) {
		nlist.item(i).disabled = false;
	}
	canvasData.edit.innerHTML = "";
}