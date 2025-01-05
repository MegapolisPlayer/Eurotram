//this approach is MUCH slower than using createElement and such
//here it is acceptable since we dont care about the performance of this part all that much
//canvas performance is the larger bottleneck here (and we do optimize it)
//speed data: https://stackoverflow.com/questions/7327056/appending-html-string-to-the-dom

//TODO for future release - rewriting of this to the createElement and appendChild syntax

function addBasicEditInputs(aobject) {
	return ""+
	"X:<input type='number' id='editxinput' name='editxinput' value="+aobject.xpos+"><br>"+
	"Y:<input type='number' id='edityinput' name='edityinput' value="+aobject.ypos+"><br>"+
	"Height:<input type='number' id='editheightinput' name='editheightinput' value="+aobject.height+"><br>"+
	"Station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+aobject.stationCode+"><br>";
}

function getDataFromBasicInputs(aobject) {
	aobject.xpos = Number(document.getElementById("editxinput").value);
	aobject.ypos = Number(document.getElementById("edityinput").value);
	aobject.height = Number(document.getElementById("editheightinput").value);
	aobject.stationCode = String(document.getElementById("editcodeinput").value);
}

function addTrackEditInputs(aobject, id) {
	let temp = "Bezier:<input type='checkbox' id='editbezinput' name='editbezinput' "+(aobject.bezier?"checked":"")+"><br>"+
	"<hr><em>Control point values have no effect if Bezier curves are disabled.</em><hr>"+
	"CP1,X:<input type='number' id='editcp1xinput' name='editcp1xinput' value="+aobject.controlPoint1.x+"><br>"+
	"CP1,Y:<input type='number' id='editcp1yinput' name='editcp1yinput' value="+aobject.controlPoint1.y+"><br>"+
	"CP2,X:<input type='number' id='editcp2xinput' name='editcp2xinput' value="+aobject.controlPoint2.x+"><br>"+
	"CP2,Y:<input type='number' id='editcp2yinput' name='editcp2yinput' value="+aobject.controlPoint2.y+"><br>"+
	"<button type='' onclick='trackEditMoveCP2toCP1("+id+")'>Move CP2 to CP1</button><br>"+
	"<button type='' onclick='trackEditRecalcHeight("+id+")'>Recalculate height points</button><br>"+
	"<button type='' onclick='trackEditRecalcCP("+id+")'>Reset control points</button><br>"+
	"<hr><h4>Edit heightpoints manually</h4>";

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		temp += "HgPt"+i+":<input type='number' id='edithght"+i+"' name='edithght"+i+"' value="+aobject.heightpoints[i]+"><br>";
	}

	return temp;
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

function scenarioNew() {
	currentMode = mode.SCENARIO_NEW;
	canvasData.mode.innerHTML = "New scenario";
}
function onclickNewScenarioHandler(ax, ay) {
	//open mode where we click stations and switches of line, then saves file
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