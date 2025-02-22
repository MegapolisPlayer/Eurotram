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

function lineAddOnClick(aEvent) {
	let p = getPoint(aEvent);
	let x = p.x;
	let y = p.y;

	let value = getColliding(nodeList, x, y);
	let isSwitch = false;
	if(value === -1) {
		value = getColliding(switchList, x, y);
		if(value === -1) {
			//nothing added - still redraw
			lineRedrawSelected();
			return;
		}
		isSwitch = true;
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

//LINE ADDING STRUCTURE
// add -> review -> serialize -> end
//    <-------/

function newLineCreate() {
	//unhide canvas
	showCanvas();
	document.getElementById("hidemap").disabled = false;

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
	canvasData.edit.appendChild(document.createTextNode("Line number "));
	addInput("lineno", 0, "text");

	let lineInfo = document.createElement("em");
	lineInfo.textContent = "Line input supports either a number (smaller than 256) or a 2-letter combination (e.g. XC). Line type settings may override this value.";
	canvasData.edit.appendChild(lineInfo);
	canvasData.edit.appendChild(document.createElement("br"));

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
	option.textContent = "Handling";
	option.setAttribute("value", 1); 
	lineSpecial.appendChild(option);

	//service ride
	option = document.createElement("option");
	option.textContent = "Service";
	option.setAttribute("value", 2); 
	lineSpecial.appendChild(option);

	//special ride
	option = document.createElement("option");
	option.textContent = "Special";
	option.setAttribute("value", 3); 
	lineSpecial.appendChild(option);

	//practice ride
	option = document.createElement("option");
	option.textContent = "Practice";
	option.setAttribute("value", 4); 
	lineSpecial.appendChild(option);

	//test ride
	option = document.createElement("option");
	option.textContent = "Test";
	option.setAttribute("value", 5); 
	lineSpecial.appendChild(option);

	canvasData.edit.appendChild(document.createTextNode("Line type "));
	canvasData.edit.appendChild(lineSpecial);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Line order number "));
	addInput("lineon", 0, "number");

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
	serializeButton.addEventListener("click", () => {
		canvasData.element.removeEventListener("click", lineAddOnClick);
		lineFinalize();
	});
	canvasData.edit.appendChild(serializeButton);

	canvasData.element.addEventListener("click", lineAddOnClick);
}

let loops = [];
let combinedTrackList = [];
let stationTimes = [];

function lineFinalize() {
	//get all inputs

	let lineno = document.getElementById("lineno").value;
	let lineon = document.getElementById("lineon").value;
	let lineType = Number(document.getElementById("lineopt").value);

	//clear
	canvasData.edit.replaceChildren();
	canvasData.canvasmenu.replaceChildren();
	document.getElementById("hidemap").disabled = true;

	console.log("Line manager");
	currentMode = mode.VIEW;
	canvasData.mode.textContent = "Reviewing line...";

	mapHidden = false;
	toggleMap(); //hide map
	//hide canvas
	hideCanvas();

	let header = document.createElement("h2");
	header.textContent = "Finalize line";
	canvasData.canvasmenu.appendChild(header);
	canvasData.canvasmenu.appendChild(document.createElement("hr"));

	//description

	let em = document.createElement("em");
	em.textContent = "Add time (in minutes) in which the tram should drive from the previous to this station."
	canvasData.canvasmenu.appendChild(em);
	canvasData.canvasmenu.appendChild(document.createElement("hr"));

	//inputs

	canvasData.canvasmenu.appendChild(document.createTextNode("Line scenario name: " ));
	addInputPlaceholder("linename", "", "text", "Name of line scenario", canvasData.canvasmenu);
	canvasData.canvasmenu.appendChild(document.createElement("br"));

	//add list of stations and time offset

	combinedTrackList.push(...lineTrackList);
	//t - time from prev, m - time in station, c - is control point
	lineTrackList.filter(v => trackList[v] instanceof StationTrack).forEach(
		v => stationTimes.push({sc: trackList[v].stationCode, t: 1, m: 1, c: false})
	);

	let df = new DocumentFragment();
	stationTimes.forEach((v, i, a) => {
		//for each station

		df.appendChild(document.createTextNode("Station (code " + v.sc + ") "));
		
		df.appendChild(document.createTextNode("Mins from prev: "));
		let minsFromPrev = document.createElement("input");
		minsFromPrev.class = "minsfp";
		minsFromPrev.type = "number";
		minsFromPrev.size = 1;
		minsFromPrev.min = 0;
		minsFromPrev.setAttribute("value", v.t);
		minsFromPrev.setAttribute("_id", i);
		minsFromPrev.addEventListener("change", (aEvent) => {
			stationTimes[Number(aEvent.target.getAttribute("_id"))].t = Number(aEvent.target.value);
		});
		df.appendChild(minsFromPrev);
		df.appendChild(document.createElement("br"));

		df.appendChild(document.createTextNode("Mins in station: "));
		let minsInStation = document.createElement("input");
		minsInStation.class = "minsis";
		minsInStation.type = "number";
		minsInStation.size = 1;
		minsInStation.min = 0;
		minsInStation.setAttribute("value", v.m);
		minsInStation.setAttribute("_id", i);
		minsInStation.addEventListener("change", (aEvent) => {
			stationTimes[Number(aEvent.target.getAttribute("_id"))].m = Number(aEvent.target.value);
		});
		df.appendChild(minsInStation);
		df.appendChild(document.createElement("br"));

		df.appendChild(document.createTextNode("Is control stop? "));
		let controlPoint = document.createElement("input");
		controlPoint.class = "iscontrol";
		controlPoint.type = "checkbox";
		if(v.c) {
			controlPoint.setAttribute("checked", "");
		}
		controlPoint.setAttribute("_id", i);
		controlPoint.addEventListener("change", (aEvent) => {
			stationTimes[Number(aEvent.target.getAttribute("_id"))].c = aEvent.target.checked;
		});
		df.appendChild(controlPoint);
		df.appendChild(document.createElement("br"));

		df.appendChild(document.createElement("hr"));
	});
	canvasData.canvasmenu.append(df);

	canvasData.canvasmenu.appendChild(document.createTextNode("Total duration: "));
	let totdur = document.createElement("span");
	totdur.id = "totdur";
	canvasData.canvasmenu.appendChild(totdur);

	let totdurrbtn = document.createElement("button");
	totdurrbtn.textContent = "Recalculate";
	totdurrbtn.addEventListener("click", (aEvent) => {
		document.getElementById("totdur").textContent = String(
			stationTimes.map(v =>v.t).reduce((p, c) => p + c) +
			stationTimes.map(v => v.m).reduce((p, c) => p + c) 
		) + " minutes";
	});
	totdurrbtn.click();
	canvasData.canvasmenu.appendChild(totdurrbtn);
	canvasData.canvasmenu.appendChild(document.createElement("hr"));

	//scenario start
	//scenario end calculated from start and timetable
	canvasData.canvasmenu.appendChild(document.createTextNode("Line start time: "));
	//ISO 8601 format - slice to remove milliseconds
	addInput("startdate", new Date().toISOString().slice(0, 16), "datetime-local", canvasData.canvasmenu);

	//line weather checkboxes

	canvasData.canvasmenu.appendChild(document.createTextNode("Wind?"));
	addInputCheckbox("iswind", false, canvasData.canvasmenu);
	canvasData.canvasmenu.appendChild(document.createTextNode("Rain?"));
	addInputCheckbox("israin", false, canvasData.canvasmenu);
	canvasData.canvasmenu.appendChild(document.createTextNode("Lighting?"));
	addInputCheckbox("islighting", false, canvasData.canvasmenu);
	canvasData.canvasmenu.appendChild(document.createTextNode("Fog?"));
	addInputCheckbox("isfog", false, canvasData.canvasmenu);
	canvasData.canvasmenu.appendChild(document.createTextNode("Snow?"));
	addInputCheckbox("issnow", false, canvasData.canvasmenu);

	//seasons

	let seasonSelect = document.createElement("select");
	seasonSelect.id = "seasonselect";

	//spring
	let option = document.createElement("option");
	option.textContent ="Spring";
	option.setAttribute("value", 0); 
	seasonSelect.appendChild(option);

	//summer
	option = document.createElement("option");
	option.textContent ="Summer";
	option.setAttribute("value", 1); 
	seasonSelect.appendChild(option);

	//autumn
	option = document.createElement("option");
	option.textContent = "Autumn";
	option.setAttribute("value", 2); 
	seasonSelect.appendChild(option);

	//winter
	option = document.createElement("option");
	option.textContent ="Winter";
	option.setAttribute("value", 3); 
	seasonSelect.appendChild(option);

	canvasData.canvasmenu.appendChild(document.createTextNode("Season "));
	canvasData.canvasmenu.appendChild(seasonSelect);
	canvasData.canvasmenu.appendChild(document.createElement("br"));

	canvasData.canvasmenu.appendChild(document.createElement("hr"));

	//button to add another loop and serialize
	let addLineButton = document.createElement("button");
	addLineButton.textContent = "Add new loop";
	addLineButton.addEventListener("click", () => {
		let lastPoint = lineList.at(-1);

		//clear
		loops.push({
			line: lineno,
			lineId: lineon,
			lineType: lineType,
			nodes: structuredClone(lineList),
			tracks: structuredClone(lineTrackList),
		});

		lineList.length = 0;
		lineTrackList.length = 0; 

		//add last point as new starting point
		lineList.push(lastPoint);

		//reset and add new loop
		canvasRedraw();
		lineRedrawSelected();
		lineVisualUnmake();
		newLineCreate();
	});
	canvasData.canvasmenu.appendChild(addLineButton);

	//serialize button
	let serializeButton = document.createElement("button");
	serializeButton.textContent = "Finish";
	serializeButton.addEventListener("click", () => {
		loops.push({
			line: lineno,
			lineId: lineon,
			lineType: lineType,
			nodes: structuredClone(lineList),
			tracks: structuredClone(lineTrackList),
		});
		let blob = lineSerialize();
		lineSave(blob);
		lineEnd(); //end line mode before returning
	});
	canvasData.canvasmenu.appendChild(serializeButton);

	//exit button
	let lineButton = document.createElement("button");
	lineButton.textContent = "Exit";
	lineButton.addEventListener("click", lineEnd);
	canvasData.canvasmenu.appendChild(lineButton);

	canvasData.canvasmenu.appendChild(document.createElement("br"));
	let info = document.createElement("em");
	info.textContent = "Everything until the first station will be ignored on save!";
	canvasData.canvasmenu.appendChild(info);
}

function lineSerialize() {
	let numberValuesArray = [];

	//we use same utils as in Map.js

	//add file signature
	numberValuesArray.push(...("ETSC".split('').map((v) => { return v.charCodeAt(0); })));

	numberValuesArray.push(...numberToByteArray(lineFileFormatVersion, 2)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 8)); //D - unix time in ms

	//header numbers

	//S - starting date
	let startDate = document.getElementById("startdate").value;
	numberValuesArray.push(...numberToByteArray(Math.trunc(startDate/1000), 8)); //S - start time in ms

	//W - weather flags
	let weatherFlags = 0;

	if(document.getElementById("iswind").checked) {
		weatherFlags |= WEATHER_WIND;
	}
	if(document.getElementById("israin").checked) {
		weatherFlags |= WEATHER_RAIN;
	}
	if(document.getElementById("islighting").checked) {
		weatherFlags |= WEATHER_LIGHTING;
	}
	if(document.getElementById("isfog").checked) {
		weatherFlags |= WEATHER_FOG;
	}
	if(document.getElementById("issnow").checked) {
		weatherFlags |= WEATHER_SNOW;
	}

	switch(document.getElementById("seasonselect").value) {
		case(0):
			weatherFlags |= WEATHER_SEASONS_SPRING;
			break;
		case(1):
			weatherFlags |= WEATHER_SEASONS_SUMMER;
			break;
		case(2):
			weatherFlags |= WEATHER_SEASONS_AUTUMN;
			break;
		case(3):
			weatherFlags |= WEATHER_SEASONS_WINTER;
			break;
	}

	numberValuesArray.push(...numberToByteArray(weatherFlags, 2));

	//N - amount of loops
	numberValuesArray.push(loops.length);

	//J - name string
	numberValuesArray.push(...document.getElementById("linename").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//A - author string
	numberValuesArray.push(...document.getElementById("authorname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	let canAddPoints = false; //switch cannot be first

	//for each loop
	loops.forEach((v, i, a) => {
		console.log(v.nodes, v.tracks);

		//header

		//normal line
		if(v.lineType == 0) {
			//numbered (13, 23...)
			numberValuesArray.push(Number(v.line)); //L
			numberValuesArray.push(0); //M
		}
		else if(v.lineType >= 6) {
			//lettered (XC, X6...)
			numberValuesArray.push(v.line[0]); //L
			numberValuesArray.push(v.line[1]); //M
		}
		else {
			//not normal
			numberValuesArray.push(0); //L value ignored
			numberValuesArray.push(v.lineType); //M
		}

		//I
		numberValuesArray.push(...numberToByteArray(v.lineId, 2));

		let stationAmount = v.tracks.map(v => trackList[v]).filter((x) => {
			return x instanceof StationTrack;
		}).length;
		console.log("Loop station amount ", stationAmount, " out of ", v.tracks.length);
		let switchAmount = v.nodes.filter((x) => {
			return x.t == "s";
		}).length;
		console.log("Loop switch amount ", switchAmount, " out of ", v.nodes.length);

		//N
		numberValuesArray.push(...numberToByteArray(stationAmount, 2));

		//F - first node, T - its type
		numberValuesArray.push(...numberToByteArray(v.nodes[0].v, 4));
		console.log("First node: ", v.nodes[0]);
		numberValuesArray.push(v.nodes[0].t.codePointAt());

		//objects
		
		let inputIndex = 0;

		for(let i = 0; i < v.tracks.length; i++) {
			//n + 1 nodes, n tracks

			if(trackList[v.tracks[i]] instanceof StationTrack) {
				numberValuesArray.push('S'.codePointAt());

				//I
				numberValuesArray.push(...numberToByteArray(v.tracks[i], 4));

				//C
				numberValuesArray.push(...stationCodeToArray(trackList[v.tracks[i]].stationCode));

				//T
				numberValuesArray.push(...numberToByteArray(Number(stationTimes[inputIndex].t), 2));
			
				//M
				numberValuesArray.push(...numberToByteArray(Number(stationTimes[inputIndex].m), 2));
				
				//K
				numberValuesArray.push(Number(stationTimes[inputIndex].c));

				//N
				//if node immediately after is switch
				let amountSwitchesToNext = Number(v.nodes[i + 1].t == 's');
				for(let j = i + 1; j < v.tracks.length; j++) {
					if(trackList[v.tracks[j]] instanceof StationTrack) {
						break;
					}
					if(v.nodes[j+1].t == 's') {
						amountSwitchesToNext++;
					}
				}
				numberValuesArray.push(amountSwitchesToNext);

				inputIndex++;

				canAddPoints = true; //adding points after first station
			}

			if(v.nodes[i + 1].t == "s" && canAddPoints) {
				console.log("Switch ", v.nodes[i + 1].v, " ", i+1);

				numberValuesArray.push('W'.codePointAt());
				numberValuesArray.push(...numberToByteArray(v.nodes[i + 1].v, 4)); //I

				//switch is last
				if(!v.nodes[i + 2].v) { 
					numberValuesArray.push(0b11111111);
				}
				//next is front
				else if(switchList[v.nodes[i + 1].v].frontId == v.nodes[i + 2].v) {
					numberValuesArray.push(0);
				}
				//next is left
				else if(switchList[v.nodes[i + 1].v].leftId == v.nodes[i + 2].v) {
					numberValuesArray.push(1);
				}
				//next is right
				else if(switchList[v.nodes[i + 1].v].rightId == v.nodes[i + 2].v) {
					numberValuesArray.push(2);
				}
				//next is back - different direction
				else if(switchList[v.nodes[i + 1].v].beforeId == v.nodes[i + 2].v) {
					numberValuesArray.push(3);
				}
				//not set
				else {
					numberValuesArray.push(0b11111111);
					console.warn("Switch ids not set!");
				}
			}
		}
	});

	//convert to blob

	let binaryArray = new Uint8Array(numberValuesArray.length);
	binaryArray.forEach((v, i, a) => {
		a[i] = numberValuesArray[i];
	})

	//blobs are immutable
	let blob = new Blob([binaryArray], {type: "application/octet-stream"}); //arbitrary binary data

	return blob;
}

function lineVisualUnmake() {
	//unhide canvas
	showCanvas();
	mapHidden = true;
	toggleMap(); //unhide map
	document.getElementById("hidemap").disabled = false;
}

function lineEnd() {	
	lineVisualUnmake();

	currentMode = mode.VIEW;
	canvasData.mode.textContent = "View";

	canvasData.element.removeEventListener("click", lineAddOnClick);
	let nlist = document.getElementsByClassName("menubtn");
	for(let i = 0; i < nlist.length; i++) {
		nlist.item(i).disabled = false;
	}
	canvasData.edit.replaceChildren();
	canvasData.canvasmenu.replaceChildren();
	lineList.length = 0;
	lineTrackList.length = 0;
	loops.length = 0;
	combinedTrackList.length = 0;
	stationTimes.length = 0;

	canvasRedraw(); //remove drawn selection
}