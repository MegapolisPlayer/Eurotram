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
	SIGN_ADD: 22,
	STATION_TRACK_ADD: 23,
	TEXTURE_PARCEL_ADD: 24,
	PRESIGNAL_ADD: 25,

	EDIT: 30,
	EDIT_TRACK: 31,
	EDIT_TEXTURE_PARCEL: 32,
};

let currentMode = mode.VIEW;

function viewMode() {
	currentMode = mode.VIEW;
	canvasData.mode.textContent = "View";
	canvasData.edit.replaceChildren();
	canvasRedraw(); //get rid of temporary markings
}

function nodeAdd() {
	currentMode = mode.NODE_ADD;
	canvasData.mode.textContent = "Add node";
}
function onclickTNodeAddHandler(aX, aY) {
	nodeList.push(new TNode(aX, aY, "", ""));
	nodeList.at(-1).draw();
}

function switchAdd() {
	currentMode = mode.SWITCH_ADD;
	canvasData.mode.textContent = "Add switch";
}
function onclickSwitchAddHandler(aX, aY) {
	switchList.push(new Switch(aX, aY, "", "", 0));
	switchList.at(-1).draw();
}

function switchSignalAdd() {
	currentMode = mode.SWITCH_SIG_ADD;
	canvasData.mode.textContent = "Add switch signal";
}
function onclickSwitchSignalAddHandler(aX, aY) {
	switchSignalList.push(new SwitchStateSignal(aX, aY));
	switchSignalList.at(-1).draw();
}

function signalAdd() {
	currentMode = mode.SIGNAL_ADD;
	canvasData.mode.textContent = "Add signal";
}
function onclickSignalAddHandler(aX, aY) {
	signalList.push(new Signal(aX, aY));
	signalList.at(-1).draw();
}
function presignalAdd() {
	currentMode = mode.PRESIGNAL_ADD;
	canvasData.mode.textContent = "Add presignal";
}
function onclickPreignalAddHandler(aX, aY) {
	signalList.push(new Presignal(aX, aY));
	signalList.at(-1).draw();
}

function radioAdd() {
	currentMode = mode.RADIO_ADD;
	canvasData.mode.textContent = "Add radiobox";
}
function onclickRadioAddHandler(aX, aY) {
	radioList.push(new RadioBox(aX, aY));
	radioList.at(-1).draw();
}

function trackAdd() {
	currentMode = mode.TRACK_ADD;
	canvasData.mode.textContent = "Add track";
}

//also calls onclickTrackAddHandler
function stationTrackAdd() {
	currentMode = mode.STATION_TRACK_ADD;
	canvasData.mode.textContent = "Add station track";
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

function onclickTrackAddHandler(aX, aY, aisStation) {
	if(trackFirst === -1) {
		trackFirst = getColliding(nodeList, aX, aY);
		//first node - returns -1 if fails
		if(trackFirst !== -1) {
			//if found first node
			nodeList[trackFirst].draw(SELECT_COLOR);
			trackFirstIsSwitch = false;
		}
		else {
			trackFirst = getColliding(switchList, aX, aY);
			if(trackFirst !== -1) {
				//if found first node - is switch
				switchList[trackFirst].draw(SELECT_COLOR);
				trackFirstIsSwitch = true;
			}
		}
	}
	else {
		let secondTNode = getColliding(nodeList, aX, aY);
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
			secondTNode = getColliding(switchList, aX, aY);
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
	canvasData.mode.textContent = "Add building";
}
function onclickBuildingAddHandler(aX, aY) {
	buildingSelectMenu(aX, aY);
}

function treeAdd() {
	currentMode = mode.TREE_ADD;
	canvasData.mode.textContent = "Add tree";
}
function onclickTreeAddHandler(aX, aY) {
	treeList.push(new Tree(aX, aY));
	treeList.at(-1).draw();
}

function lightAdd() {
	currentMode = mode.LIGHT_ADD;
	canvasData.mode.textContent = "Add light pole";
}
function onclickLightAddHandler(aX, aY) {
	lightList.push(new Light(aX, aY));
	lightList.at(-1).draw();
}

function wallAdd() {
	currentMode = mode.WALL_ADD;
	canvasData.mode.textContent = "Add wall";
}
function onclickWallAddHandler(aX, aY) {
	wallAddMenu(aX, aY);
}

function signAdd() {
	currentMode = mode.SIGN_ADD;
	canvasData.mode.textContent = "Add sign";
}
function onclickSignAddHandler(aX, aY) {
	signSelectMenu(aX, aY);
}

function landmarkAdd() {
	currentMode = mode.LANDMARK_ADD;
	canvasData.mode.textContent = "Add landmark";
}
function onclickLandmarkAddHandler(aX, aY) {
	landmarkSelectMenu(aX, aY);
}

function stationPillarAdd() {
	currentMode = mode.STATION_PILLAR_ADD;
	canvasData.mode.textContent = "Add station pillar";
}

function onclickStationPillarAddHandler(aX, aY) {
	stationPillarList.push(new StationPillar(aX, aY));
	stationPillarList.at(-1).draw();
}

function edit() {
	currentMode = mode.EDIT;
	canvasData.mode.textContent = "Edit";
}
function onclickEditHandler(aX, aY) {
	let value = getColliding(wallList, aX, aY);
	if(value !== -1) {
		wallEditMenu(value);
		return;
	}

	value = getColliding(nodeList, aX, aY);
	if(value !== -1) {
		nodeEditMenu(value);
		return;
	}

	value = getColliding(switchList, aX, aY);
	if(value !== -1) {
		switchEditMenu(value);
		return;
	}

	value = getColliding(stationPillarList, aX, aY);
	if(value !== -1) {
		stationPillarEditMenu(value);
		return;
	}

	value = getColliding(signalList, aX, aY);
	if(value !== -1) {
		if(signalList[value] instanceof Presignal) {
			presignalEditMenu(value);
		}
		else {
			signalEditMenu(value);
		}
		return;
	}

	value = getColliding(switchSignalList, aX, aY);
	if(value !== -1) {
		switchSignalEditMenu(value);
		return;
	}

	value = getColliding(lightList, aX, aY);
	if(value !== -1) {
		lightEditMenu(value);
		return;
	}

	value = getColliding(treeList, aX, aY);
	if(value !== -1) {
		treeEditMenu(value);
		return;
	}

	value = getColliding(radioList, aX, aY);
	if(value !== -1) {
		radioEditMenu(value);
		return;
	}

	value = getColliding(signList, aX, aY);
	if(value !== -1) {
		signEditMenu(value);
		return;
	}

	value = getColliding(buildingList, aX, aY);
	if(value !== -1) {
		buildingEditMenu(value);
		return;
	}

	value = getColliding(landmarkList, aX, aY);
	if(value !== -1) {
		landmarkEditMenu(value);
		return;
	}

	console.log("Nothing clicked!");
}

function editTrack() {
	currentMode = mode.EDIT_TRACK;
	canvasData.mode.textContent = "Edit track";
}

//select 2 nodes, selects track between them
function onclickEditTrackHandler(aX, aY) {
	if(trackFirst === -1) {
		trackFirst = getColliding(nodeList, aX, aY);
		//first node - returns -1 if fails
		if(trackFirst !== -1) {
			//if found first node
			nodeList[trackFirst].draw(SELECT_COLOR);
		}
		else {
			trackFirst = getColliding(switchList, aX, aY);
			if(trackFirst !== -1) {
				//if found first node - is switch
				switchList[trackFirst].draw(SELECT_COLOR);
				trackFirstIsSwitch = true;
			}
		}
	}
	else {
		let secondTNode = getColliding(nodeList, aX, aY);
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
			secondTNode = getColliding(switchList, aX, aY);
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

				if(trackList[i] instanceof StationTrack) {
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
	canvasData.mode.textContent = "Add texture parcel";
}

function onclickTextureParcelAddHandler(aX, aY) {
	texparcelAddMenu(aX, aY);
}

function textureParcelEdit() {
	currentMode = mode.EDIT_TEXTURE_PARCEL;
	canvasData.mode.textContent = "Edit texture parcel";
}

function onclickTextureParcelEditHandler(aX, aY) {
	for(let i = 0; i < texparcelList.length; i++) {
		if(texparcelList[i].collision(aX, aY)) {
			texparcelEditMenu(i);
			break;
		}
	}
}