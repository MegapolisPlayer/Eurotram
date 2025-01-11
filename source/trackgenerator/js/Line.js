//control point in timetable which the tram must follow
//can be different for each line
//time as second offset from beginning of scenario
function LineStation(acode, atime, acontrolPoint) {
	this.point = acode;
	this.time = atime;
	this.controlPoint = acontrolPoint;
	return this;
};



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



	if(lineList.length == 0) {
		document.getElementById("lineundo").disabled = true;
	}
}	

function lineAddOnClick() {
	console.log("Line edit click!");

	//TODO click on nodes/switches, find tracks depending on that
	//lineList is list of tracks of line

	//we added something - we can undo
	document.getElementById("lineundo").disabled = false;
}

function newLineCreate() {
	currentMode = mode.VIEW;
	canvasData.mode.textContent = "Adding new line...";

	canvasData.edit.replaceChildren();
	
	let em = document.createElement("em");
	em.textContent = "Click on all of the nodes where the tram will go."
	canvasData.edit.appendChild(em);

	canvasData.edit.appendChild(document.createElement("br"));
	
	let nlist = document.getElementsByClassName("menubtn");
	for(let i = 0; i < nlist.length; i++) {
		nlist.item(i).disabled = true;
	}
	
	//whether or not next click is control point
	addInputCheckbox("nextcp", false);

	//scenario start
	//scenario end calculated from start and timetable

	let lineButton = document.createElement("button");
	lineButton.textContent = "Exit";
	lineButton.addEventListener("click", lineEnd);
	canvasData.edit.appendChild(lineButton);
	
	let undoButton = document.createElement("button");
	undoButton.id = "lineundo";
	undoButton.disabled = true;
	undoButton.addEventListener("click", lineUndo);
	undoButton.textContent = "Undo";
	canvasData.edit.appendChild(undoButton);

	let serializeButton = document.createElement("button");
	serializeButton.textContent = "Done";
	serializeButton.addEventListener("click", lineSerialize);
	canvasData.edit.appendChild(serializeButton);

	canvasData.element.addEventListener("click", lineAddOnClick);
}

function lineSerialize() {
	console.log(lineList);

	let numberValuesArray = [];

	//we use same utils from Map.js

	//add file signature
	numberValuesArray.push(...("ETSC".split('').map((v) => { return v.charCodeAt(0); })));

	numberValuesArray.push(...numberToByteArray(fileFormatVersion, 2)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 8)); //D - unix time in ms

	//S, E

	//convert to blob

	let binaryArray = new Uint8Array(numberValuesArray.length);
	binaryArray.forEach((v, i, a) => {
		a[i] = numberValuesArray[i];
	})

	//blobs are immutable
	let blob = new Blob([binaryArray], {type: "application/octet-stream"}); //arbitrary binary data

	lineEnd(); //end line mode before returning

	return blob;
}

function lineEnd() {
	canvasData.element.removeEventListener("click", lineAddOnClick);
	let nlist = document.getElementsByClassName("menubtn");
	for(let i = 0; i < nlist.length; i++) {
		nlist.item(i).disabled = false;
	}
	canvasData.edit.replaceChildren();
	lineList.length = 0;
}