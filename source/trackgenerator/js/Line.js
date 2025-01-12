//start from back - first bit is most significant

const WEATHER_WIND           = 0b0000000000000001;
const WEATHER_RAIN           = 0b0000000000000010;
const WEATHER_LIGHTING       = 0b0000000000000100;
const WEATHER_FOG            = 0b0000000000001000;
const WEATHER_SNOW           = 0b0000000000010000; //add others after this
const WEATHER_SEASONS_SPRING = 0b0001000000000000;
const WEATHER_SEASONS_SUMMER = 0b0010000000000000;
const WEATHER_SEASONS_AUTUMN = 0b0100000000000000;
const WEATHER_SEASONS_WINTER = 0b1000000000000000;

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
let lineTrackList = []; //just visualizes line, isnt saved - stores ids of track

let lineFileFormatVersion = 0;

function lineRedrawSelected() {
	lineList.forEach((v) => {
		if(v.t == "n") nodeList[v.v].draw(SELECT_COLOR);
		else switchList[v.v].draw(SELECT_COLOR);
	});
	lineTrackList.forEach((v) => {
		trackList[v].draw(SELECT_COLOR);
	});
}

function lineUndo() {
	lineList.pop();
	lineTrackList.pop();

	if(lineList.length == 0) {
		document.getElementById("lineundo").disabled = true;
	}
}

function lineAddOnClick(aevent) {
	let p = getPoint(aevent);
	let x = p.x;
	let y = p.y;

	console.log("Line edit click!");

	let value = getColliding(nodeList, x, y);
	let isSwitch = false;
	if(value === -1) {
		value = getColliding(switchList, x, y);
		if(value === -1) {
			//nothing added - still redraw
			lineRedrawSelected();
			return;
		}
		console.log("LM - clicked switch");
		isSwitch = true;
	}
	else {
		console.log("LM - clicked node");
	}

	//find and add new track
	if(lineList.length >= 1) {
		//check tracks
		for(let i = 0; i < trackList.length; i++) {
			//if current node and last node have track between them
			if((
				(trackList[i].nodeIdFirst == value && trackList[i].firstIsSwitch == isSwitch) ||
				(trackList[i].nodeIdFirst == lineList.at(-1).v && trackList[i].firstIsSwitch == (lineList.at(-1).t == "s"))
			) && (
				(trackList[i].nodeIdSecond == value && trackList[i].secondIsSwitch == isSwitch) ||
				(trackList[i].nodeIdSecond == lineList.at(-1).v && trackList[i].secondIsSwitch == (lineList.at(-1).t == "s"))
			)) {
				//add point
				lineList.push({t: isSwitch ? "s" : "n", v: value});
				//add track
				lineTrackList.push(i);

				//we added something - we can undo
				document.getElementById("lineundo").disabled = false;
				break;
			}
		}	
	}
	else {
		//add first point
		lineList.push({t: isSwitch ? "s" : "n", v: value});
	}

	lineRedrawSelected();
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
	
	//line no/letter

	//is line special (overrides)
	//select box w/ options
	//values mapped to correct value M in file

	let lineSpecial = document.createElement("select");
	lineSpecial.id = "lineopt";

	//normal
	let option = document.createElement("option");
	option.textContent ="Normal";
	option.setAttribute("value", 0); 
	lineSpecial.appendChild(option);

	//Handling ride
	option = document.createElement("option");
	option.textContent ="Handling";
	option.setAttribute("value", 1); 
	lineSpecial.appendChild(option);

	//service ride
	option = document.createElement("option");
	option.textContent ="Service";
	option.setAttribute("value", 2); 
	lineSpecial.appendChild(option);

	//special ride
	option = document.createElement("option");
	option.textContent ="Special";
	option.setAttribute("value", 3); 
	lineSpecial.appendChild(option);

	//practice ride
	option = document.createElement("option");
	option.textContent ="Practice";
	option.setAttribute("value", 4); 
	lineSpecial.appendChild(option);

	//test ride
	option = document.createElement("option");
	option.textContent ="Test";
	option.setAttribute("value", 5); 
	lineSpecial.appendChild(option);

	canvasData.edit.appendChild(document.createTextNode("Line type "));
	canvasData.edit.appendChild(lineSpecial);
	canvasData.edit.appendChild(document.createElement("br"));

	//scenario start
	//scenario end calculated from start and timetable
	canvasData.edit.appendChild(document.createTextNode("Line start time: "));
	//ISO 8601 format - slice to remove milliseconds
	addInput("startdate", new Date().toISOString().slice(0, 16), "datetime-local");

	//line weather checkboxes

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
	serializeButton.addEventListener("click", lineFinalize);
	canvasData.edit.appendChild(serializeButton);

	canvasData.element.addEventListener("click", lineAddOnClick);
}

let loops = [];

function lineFinalize() {
	currentMode = mode.VIEW;
	canvasData.mode.textContent = "Finalizing new line...";

	//add menu of station tracks and 

	//button to add another loop

	lineSerialize();
}

function lineSerialize() {
	console.log(lineList);

	let numberValuesArray = [];

	//we use same utils from Map.js

	//add file signature
	numberValuesArray.push(...("ETSC".split('').map((v) => { return v.charCodeAt(0); })));

	numberValuesArray.push(...numberToByteArray(lineFileFormatVersion, 2)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 8)); //D - unix time in ms

	//S



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
	lineTrackList.length = 0;

	canvasRedraw(); //remove drawn selection
}