function addBasicEditInputs(aobject) {
	let editxinput = document.createElement("input");
	editxinput.type = "number";
	editxinput.id = "editxinput";
	editxinput.name = editxinput.id;
	editxinput.setAttribute("value", aobject.xpos);

	canvasData.edit.appendChild(document.createTextNode("X: "));
	canvasData.edit.appendChild(editxinput);
	canvasData.edit.appendChild(document.createElement("br"));
	
	let edityinput = document.createElement("input");
	edityinput.type = "number";
	edityinput.id = "edityinput";
	edityinput.name = edityinput.id;
	edityinput.setAttribute("value", aobject.ypos);
	
	canvasData.edit.appendChild(document.createTextNode("Y: "));
	canvasData.edit.appendChild(edityinput);
	canvasData.edit.appendChild(document.createElement("br"));

	let editheightinput = document.createElement("input");
	editheightinput.type = "number";
	editheightinput.id = "editheightinput";
	editheightinput.name = editheightinput.id;
	editheightinput.setAttribute("value", aobject.height);
	
	canvasData.edit.appendChild(document.createTextNode("Height: "));
	canvasData.edit.appendChild(editheightinput);
	canvasData.edit.appendChild(document.createElement("br"));

	let editcodeinput = document.createElement("input");
	editcodeinput.type = "text";
	editcodeinput.id = "editcodeinput";
	editcodeinput.name = editcodeinput.id;
	editcodeinput.placeholder = "XXXX";
	editcodeinput.maxLength = 4;
	editcodeinput.setAttribute("value", aobject.stationCode);

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	canvasData.edit.appendChild(editcodeinput);
	canvasData.edit.appendChild(document.createElement("br"));
}

function getDataFromBasicInputs(aobject) {
	aobject.xpos = Number(document.getElementById("editxinput").value);
	aobject.ypos = Number(document.getElementById("edityinput").value);
	aobject.height = Number(document.getElementById("editheightinput").value);
	aobject.stationCode = String(document.getElementById("editcodeinput").value);
}

function addTrackEditInputs(aobject, id) {
	canvasData.edit.appendChild(document.createTextNode("Bezier: "));

	let editbezinput = document.createElement("input");
	editbezinput.id = "editbezinput";
	editbezinput.id = editbezinput.name;
	if(aobject.bezier) {
		editbezinput.setAttribute("checked", "");
	}

	canvasData.edit.appendChild(document.createElement("<hr>"));

	let em = document.createElement("<em>");
	em.appendChild(document.createTextNode("Control point values have no effect if Bezier curves are disabled."));
	canvasData.edit.appendChild(em);

	canvasData.edit.appendChild(document.createElement("<hr>"));

	canvasData.edit.appendChild(document.createTextNode("CP1;X: "));
	let cp1x = document.createElement("input");
	cp1x.type = "number";
	cp1x.id = "editcp1xinput";
	cp1x.name = cp1x.id;
	cp1x.setAttribute("value", aobject.controlPoint1.x);
	canvasData.edit.appendChild(cp1x);
	canvasData.edit.appendChild(document.createElement("<br>"));

	canvasData.edit.appendChild(document.createTextNode("CP1;Y: "));
	let cp1y = document.createElement("input");
	cp1y.type = "number";
	cp1y.id = "editcp1yinput";
	cp1y.name = cp1y.id;
	cp1y.setAttribute("value", aobject.controlPoint1.y);
	canvasData.edit.appendChild(cp1y);
	canvasData.edit.appendChild(document.createElement("<br>"));

	canvasData.edit.appendChild(document.createTextNode("CP2;X: "));
	let cp2x = document.createElement("input");
	cp2x.type = "number";
	cp2x.id = "editcp2xinput";
	cp2x.name = cp2x.id;
	cp2x.setAttribute("value", aobject.controlPoint2.x);
	canvasData.edit.appendChild(cp2x);
	canvasData.edit.appendChild(document.createElement("<br>"));

	canvasData.edit.appendChild(document.createTextNode("CP2;Y: "));
	let cp2y = document.createElement("input");
	cp2y.type = "number";
	cp2y.id = "editcp2yinput";
	cp2y.name = cp2y.id;
	cp2y.setAttribute("value", aobject.controlPoint2.y);
	canvasData.edit.appendChild(cp2y);
	canvasData.edit.appendChild(document.createElement("<br>"));

	let btnCP2CP1 = document.createElement("button");
	btnCP2CP1.onclick = "trackEditMoveCP2toCP1("+id+")";
	btnCP2CP1.appendChild(document.createTextNode("Move CP2 to CP1"));
	canvasData.edit.appendChild(document.createElement("<br>"));

	let btnHeight = document.createElement("button");
	btnHeight.onclick = "trackEditRecalcHeight("+id+")";
	btnHeight.appendChild(document.createTextNode("Recalculate height points"));
	canvasData.edit.appendChild(document.createElement("<br>"));

	let btnRecalc = document.createElement("button");
	btnHeight.onclick = "trackEditRecalcCP("+id+")";
	btnRecalc.appendChild(document.createTextNode("Reset control points"));
	canvasData.edit.appendChild(document.createElement("<br>"));

	canvasData.edit.appendChild(document.createElement("<hr>"));

	let h4 = document.createElement("h4");
	h4.appendChild(document.createTextNode("Edit heightpoints manually"));
	canvasData.edit.appendChild(h4);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		canvasData.edit.appendChild(document.createTextNode("HgPt"+i+": "));

		let hpinput = document.createElement("input");
		hpinput.type = "number";
		hpinput.id = "edithght"+i;
		hpinput.name = hpinput.id;
		hpinput.setAttribute("value", aobject.heightpoints[i]);

		canvasData.edit.appendChild(document.createElement("<br>"));
	}
}

function getDataFromTrackInputs(aobject) {
	aobject.bezier = document.getElementById("editbezinput").checked;
	aobject.controlPoint1.x = Number(document.getElementById("editcp1xinput").value);
	aobject.controlPoint1.y = Number(document.getElementById("editcp1yinput").value);
	aobject.controlPoint2.x = Number(document.getElementById("editcp2xinput").value);
	aobject.controlPoint2.y = Number(document.getElementById("editcp2yinput").value);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		aobject.heightpoints[i] = Number(document.getElementById("edithght"+i).value);
	}
}

let mode = {
	VIEW: 0,

	SCENARIO_NEW_LINE: 1,

	NODE_ADD: 10,
	SWITCH_ADD: 11,
	SWITCH_SIG_ADD: 12,
	SIGNAL_ADD: 13,
	RADIO_ADD: 14,
	TRACK_ADD: 15,
	BUILDING_ADD: 16,
	TREE_ADD: 17,
	LIGHT_ADD: 18,
	LANDMARK_ADD: 19,
	STATION_PILLAR_ADD: 20,
	WALL_ADD: 21,
	STATION_TRACK_ADD: 22,
	TEXTURE_PARCEL_ADD: 23,

	EDIT: 30,
	EDIT_TRACK: 31,
	EDIT_TEXTURE_PARCEL: 32,
};

let currentMode = mode.VIEW;

function viewMode() {
	currentMode = mode.VIEW;
	canvasData.mode.innerHTML = "View";
	canvasData.edit.innerHTML = "";
	canvasRedraw(); //get rid of temporary markings
}

function nodeAdd() {
	currentMode = mode.NODE_ADD;
	canvasData.mode.innerHTML = "Add node";
}
function onclickTNodeAddHandler(ax, ay) {
	nodeList.push(new TNode(ax, ay, "", ""));
	nodeList.at(-1).draw();
}

function switchAdd() {
	currentMode = mode.SWITCH_ADD;
	canvasData.mode.innerHTML = "Add switch";
}
function onclickSwitchAddHandler(ax, ay) {
	switchList.push(new Switch(ax, ay, "", "", 0));
	switchList.at(-1).draw();
}

function switchSignalAdd() {
	currentMode = mode.SWITCH_SIG_ADD;
	canvasData.mode.innerHTML = "Add switch signal";
}
function onclickSwitchSignalAddHandler(ax, ay) {
	switchSignalList.push(new SwitchStateSignal(ax, ay));
	switchSignalList.at(-1).draw();
}

function signalAdd() {
	currentMode = mode.SIGNAL_ADD;
	canvasData.mode.innerHTML = "Add signal";
}
function onclickSignalAddHandler(ax, ay) {
	signalList.push(new Signal(ax, ay));
	signalList.at(-1).draw();
}
function radioAdd() {
	currentMode = mode.RADIO_ADD;
	canvasData.mode.innerHTML = "Add radiobox";
}
function onclickRadioAddHandler(ax, ay) {
	radioList.push(new RadioBox(ax, ay));
	radioList.at(-1).draw();
}

function trackAdd() {
	currentMode = mode.TRACK_ADD;
	canvasData.mode.innerHTML = "Add track";
}

//also calls onclickTrackAddHandler
function stationTrackAdd() {
	currentMode = mode.STATION_TRACK_ADD;
	canvasData.mode.innerHTML = "Add station track";
}

let trackFirst = -1;
let trackFirstIsSwitch = false;

function redrawTrackFirst() {
	if(trackFirstIsSwitch) {
		switchList[trackFirst].draw();
	} 
	else {
		nodeList[trackFirst].draw();
	}
}

function onclickTrackAddHandler(ax, ay, aisStation) {
	if(trackFirst === -1) {
		trackFirst = getColliding(nodeList, ax, ay);
		//first node - returns -1 if fails
		if(trackFirst !== -1) {
			//if found first node
			nodeList[trackFirst].draw(SELECT_COLOR);
			trackFirstIsSwitch = false;
		}
		else {
			trackFirst = getColliding(switchList, ax, ay);
			if(trackFirst !== -1) {
				//if found first node - is switch
				switchList[trackFirst].draw(SELECT_COLOR);
				trackFirstIsSwitch = true;
			}
		}
	}
	else {
		let secondTNode = getColliding(nodeList, ax, ay);
		let secondSwitch = false;
		if(secondTNode === trackFirst && !trackFirstIsSwitch) {
			redrawTrackFirst();
			trackFirst = -1; //reset selection
			trackFirstIsSwitch = false;
			alert("Error: second node same as first.");
			return;
		}
		else if(secondTNode == -1) {
			//check if switch
			secondTNode = getColliding(switchList, ax, ay);
			if(secondTNode === trackFirst && trackFirstIsSwitch) {
				redrawTrackFirst();
				trackFirst = -1; //reset selection
				trackFirstIsSwitch = false;
				alert("Error: second switch same as first.");
				return;
			}
			else if(secondTNode === -1) {
				redrawTrackFirst();
				//reset selection
				trackFirst = -1; //reset selection
				trackFirstIsSwitch = false;
				return; //nothing clicked
			}
			else {
				secondSwitch = true;
			}
		}

		//track cannot lead between 2 switches
		if(trackFirstIsSwitch && secondSwitch) {
			alert("Error: track cannot lead between 2 switches.");
			redrawTrackFirst();
			trackFirst = -1;
			trackFirstIsSwitch = false;
			return;
		}

		//check if such track already exists (popup alert and ignore it)
		for(let i = 0; i < trackList.length; i++) {
			//if first and second node link to each other
			if(
				(
				(trackList[i].nodeIdFirst == trackFirst && trackList[i].firstIsSwitch == trackFirstIsSwitch) ||
				(trackList[i].nodeIdFirst == secondTNode && trackList[i].firstIsSwitch == secondSwitch)
			) &&
				(
				(trackList[i].nodeIdSecond == trackFirst && trackList[i].secondIsSwitch == trackFirstIsSwitch) ||
				(trackList[i].nodeIdSecond == secondTNode && trackList[i].secondIsSwitch == secondSwitch)
			) 
			) {
				alert("Error: track already exists.");
				redrawTrackFirst();
				trackFirst = -1;
				trackFirstIsSwitch = false;
				return;
			}
		}

		redrawTrackFirst();

		if(aisStation) {
			trackList.push(new StationTrack(trackFirst, secondTNode, trackFirstIsSwitch, secondSwitch))
		}
		else {
			trackList.push(new Track(trackFirst, secondTNode, trackFirstIsSwitch, secondSwitch))
		}
		trackFirst = -1;
		trackFirstIsSwitch = false;
		trackList.at(-1).draw();
	}
}

function buildingAdd() {
	currentMode = mode.BUILDING_ADD;
	canvasData.mode.innerHTML = "Add building";
}
function onclickBuildingAddHandler(ax, ay) {
	buildingSelectMenu(ax, ay);
}

function treeAdd() {
	currentMode = mode.TREE_ADD;
	canvasData.mode.innerHTML = "Add tree";
}
function onclickTreeAddHandler(ax, ay) {
	treeList.push(new Tree(ax, ay));
	treeList.at(-1).draw();
}

function lightAdd() {
	currentMode = mode.LIGHT_ADD;
	canvasData.mode.innerHTML = "Add light pole";
}
function onclickLightAddHandler(ax, ay) {
	lightList.push(new Light(ax, ay));
	lightList.at(-1).draw();
}

function wallAdd() {
	currentMode = mode.WALL_ADD;
	canvasData.mode.innerHTML = "Add wall";
}
function onclickWallAddHandler(ax, ay) {
	wallAddMenu(ax, ay);
}

function landmarkAdd() {
	currentMode = mode.LANDMARK_ADD;
	canvasData.mode.innerHTML = "Add landmark";
}
function onclickLandmarkAddHandler(ax, ay) {
	landmarkSelectMenu(ax, ay);
}

function stationPillarAdd() {
	currentMode = mode.STATION_PILLAR_ADD;
	canvasData.mode.innerHTML = "Add station pillar";
}

function onclickStationPillarAddHandler(ax, ay) {
	stationPillarList.push(new StationPillar(ax, ay));
	stationPillarList.at(-1).draw();
}

function edit() {
	currentMode = mode.EDIT;
	canvasData.mode.innerHTML = "Edit";
}
function onclickEditHandler(ax, ay) {
	let value = getColliding(wallList, ax, ay);
	if(value !== -1) {
		wallEditMenu(value);
		return;
	}

	value = getColliding(nodeList, ax, ay);
	if(value !== -1) {
		nodeEditMenu(value);
		return;
	}

	value = getColliding(switchList, ax, ay);
	if(value !== -1) {
		switchEditMenu(value);
		return;
	}

	value = getColliding(stationPillarList, ax, ay);
	if(value !== -1) {
		stationPillarEditMenu(value);
		return;
	}

	value = getColliding(signalList, ax, ay);
	if(value !== -1) {
		signalEditMenu(value);
		return;
	}

	value = getColliding(switchSignalList, ax, ay);
	if(value !== -1) {
		switchSignalEditMenu(value);
		return;
	}

	value = getColliding(lightList, ax, ay);
	if(value !== -1) {
		lightEditMenu(value);
		return;
	}

	value = getColliding(treeList, ax, ay);
	if(value !== -1) {
		treeEditMenu(value);
		return;
	}

	value = getColliding(radioList, ax, ay);
	if(value !== -1) {
		radioEditMenu(value);
		return;
	}

	value = getColliding(buildingList, ax, ay);
	if(value !== -1) {
		buildingEditMenu(value);
		return;
	}

	value = getColliding(landmarkList, ax, ay);
	if(value !== -1) {
		landmarkEditMenu(value);
		return;
	}

	console.log("Nothing clicked!");
}

function editTrack() {
	currentMode = mode.EDIT_TRACK;
	canvasData.mode.innerHTML = "Edit track";
}

//select 2 nodes, selects track between them
function onclickEditTrackHandler(ax, ay) {
	if(trackFirst === -1) {
		trackFirst = getColliding(nodeList, ax, ay);
		//first node - returns -1 if fails
		if(trackFirst !== -1) {
			//if found first node
			nodeList[trackFirst].draw(SELECT_COLOR);
		}
		else {
			trackFirst = getColliding(switchList, ax, ay);
			if(trackFirst !== -1) {
				//if found first node - is switch
				switchList[trackFirst].draw(SELECT_COLOR);
				trackFirstIsSwitch = true;
			}
		}
	}
	else {
		let secondTNode = getColliding(nodeList, ax, ay);
		let secondSwitch = false;
		if(secondTNode === trackFirst && !trackFirstIsSwitch) {
			redrawTrackFirst();
			trackFirst = -1; //reset selection
			trackFirstIsSwitch = false;
			alert("Error: second node same as first.");
			return;
		}
		if(secondTNode == -1) {
			//check if switch
			secondTNode = getColliding(switchList, ax, ay);
			if(secondTNode === trackFirst && trackFirstIsSwitch) {
				redrawTrackFirst();
				trackFirst = -1; //reset selection
				trackFirstIsSwitch = false;
				alert("Error: second switch same as first.");
				return;
			}
			else if(secondTNode === -1) {
				redrawTrackFirst();
				//reset selection
				trackFirst = -1; //reset selection
				trackFirstIsSwitch = false;
				return; //nothing clicked
			}
			else {
				secondSwitch = true;
			}
		}


		//check if such track already exists (popup alert and ignore it)
		for(let i = 0; i < trackList.length; i++) {
			//if first and second node link to each other
			if(
				(
				(trackList[i].nodeIdFirst == trackFirst && trackList[i].firstIsSwitch == trackFirstIsSwitch) ||
				(trackList[i].nodeIdFirst == secondTNode && trackList[i].firstIsSwitch == secondSwitch)
			) &&
				(
				(trackList[i].nodeIdSecond == trackFirst && trackList[i].secondIsSwitch == trackFirstIsSwitch) ||
				(trackList[i].nodeIdSecond == secondTNode && trackList[i].secondIsSwitch == secondSwitch)
			) 
			) {
				redrawTrackFirst();

				if(trackList instanceof StationTrack) {
					stationTrackEditMenu(i);
				}
				else {
					trackEditMenu(i);
				}
			}
		}

		trackFirst = -1;
		trackFirstIsSwitch = false;
		return;
	}
}

function textureParcelAdd() {
	currentMode = mode.TEXTURE_PARCEL_ADD;
	canvasData.mode.innerHTML = "Add texture parcel";
}

function onclickTextureParcelAddHandler(ax, ay) {
	texparcelAddMenu(ax, ay);
}

function textureParcelEdit() {
	currentMode = mode.EDIT_TEXTURE_PARCEL;
	canvasData.mode.innerHTML = "Edit texture parcel";
}

function onclickTextureParcelEditHandler(ax, ay) {
	for(let i = 0; i < texparcelList.length; i++) {
		if(texparcelList[i].collision(ax, ay)) {
			texparcelEditMenu(i);
			break;
		}
	}
}