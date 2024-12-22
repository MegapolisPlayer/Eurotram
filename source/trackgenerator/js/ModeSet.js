let mode = {
	VIEW: 0,

	SCENARIO_NEW: 1,

	NODE_ADD: 10,
	SWITCH_ADD: 11,
	RADIO_ADD: 12,
	TRACK_ADD: 13,
	BUILDING_ADD: 14,
	TREE_ADD: 15,
	LIGHT_ADD: 16,

	EDIT: 20,
	EDIT_TRACK: 21,
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
	nodeList.push(new TNode(ax, ay, "", "", 0));
	nodeList.at(-1).draw();
}

function switchAdd() {
	currentMode = mode.SWITCH_ADD;
	canvasData.mode.innerHTML = "Add switch";
}
function onclickSwitchAddHandler(ax, ay) {
	//switches also in node list
	nodeList.push(new Switch(ax, ay, "", "", 0));
	nodeList.at(-1).draw();
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

let tahFirst = -1;

function onclickTrackAddHandler(ax, ay) {
	if(tahFirst === -1) {
		tahFirst = getCollidingTNode(ax, ay);
		//first node - returns -1 if fails
		if(tahFirst !== -1) {
			//if found first node
			nodeList[tahFirst].draw("#ff00ff");
		}
	}
	else {
		let secondTNode = getCollidingTNode(ax, ay);
		nodeList[tahFirst].draw();
		if(secondTNode == -1) {
			tahFirst = -1; //reset selection
			return;
		}
		if(secondTNode == tahFirst) {
			tahFirst = -1; //reset selection
			alert("Error: second node same as first.");
			return;
		}

		//check if such track already exists (popup alert and ignore it)
		for(let i = 0; i < trackList.length; i++) {
			//if first and second node link to each other
			if(
				(trackList[i].nodeIdFirst == tahFirst ||
				trackList[i].nodeIdFirst == secondTNode) &&
				(trackList[i].nodeIdSecond == tahFirst ||
				trackList[i].nodeIdSecond == secondTNode) 
			) {
				alert("Error: track already exists.");
				tahFirst = -1;
				return;
			}
		}

		trackList.push(new Track(tahFirst, secondTNode, nodeList[tahFirst].height, nodeList[secondTNode].height))
		tahFirst = -1;
		trackList.at(-1).draw();
	}
}

function buildingAdd() {
	currentMode = mode.BUILDING_ADD;
	canvasData.mode.innerHTML = "Add building";
}
function onclickBuildingAddHandler(ax, ay) {
	//add building - type to choose from
}

function treeAdd() {
	currentMode = mode.TREE_ADD;
	canvasData.mode.innerHTML = "Add tree";
}
function onclicktreeAddHandler(ax, ay) {
	//add tree - just circle (handled in sim itself - random)
}

function lightAdd() {
	currentMode = mode.LIGHT_ADD;
	canvasData.mode.innerHTML = "Add light pole";
}
function onclicklightAddHandler(ax, ay) {
	//just add streetlamp
}

function edit() {
	currentMode = mode.EDIT;
	canvasData.mode.innerHTML = "Edit";
}
function onclickEditHandler(ax, ay) {
	let node = getCollidingTNode(ax, ay);
	if(node === -1) {
		console.log("Node not clicked");
		return;
	}

	if(nodeList[node] instanceof TNode) {
		nodeEditMenu(node);
	}
	else if(nodeList[node] instanceof Switch) {
		switchEditMenu(node);
	}
	else {
		canvasData.edit.innerHTML = "";
	}
}

function editTrack() {
	currentMode = mode.EDIT_TRACK;
	canvasData.mode.innerHTML = "Edit track";
}

let ethFirst = -1;

//select 2 nodes, selects track between them
function onclickEditTrackHandler(ax, ay) {
	if(ethFirst === -1) {
		ethFirst = getCollidingTNode(ax, ay);
		//first node - returns -1 if fails
		if(ethFirst !== -1) {
			//if found first node
			nodeList[ethFirst].draw("#ff00ff");
		}
	}
	else {
		let secondTNode = getCollidingTNode(ax, ay);
		nodeList[ethFirst].draw();
		if(secondTNode == -1) {
			ethFirst = -1; //reset selection
			return;
		}
		if(secondTNode == ethFirst) {
			ethFirst = -1; //reset selection
			alert("Error: second node same as first.");
			return;
		}

		for(let i = 0; i < trackList.length; i++) {
			if((
				trackList[i].nodeIdFirst == ethFirst ||
				trackList[i].nodeIdFirst == secondTNode) &&
				(trackList[i].nodeIdSecond == ethFirst ||
				trackList[i].nodeIdSecond == secondTNode)
			) {
				trackEditMenu(i);
				ethFirst = -1;
				return;
			}
		}
	}
}