let mode = {
	VIEW: 0,

	SCENARIO_NEW: 1,

	NODE_ADD: 10,
	TRACK_ADD: 11,
	BUILDING_ADD: 12,
	TREE_ADD: 13,
	LIGHT_ADD: 14,

	EDIT: 20,
};

let currentMode = mode.VIEW;

function viewMode() {
	currentMode = mode.VIEW;
	document.getElementById("curmode").innerHTML = "View";
	canvasData.edit.innerHTML = "";
}

function scenarioNew() {
	currentMode = mode.SCENARIO_NEW;
	document.getElementById("curmode").innerHTML = "New scenario";
}

function nodeAdd() {
	currentMode = mode.NODE_ADD;
	document.getElementById("curmode").innerHTML = "Add node";
}
function onclickNodeAddHandler(ax, ay) {
	nodeList.push(new Node(ax, ay, "", "", 0));
	nodeList.at(-1).draw();
}

function trackAdd() {
	currentMode = mode.TRACK_ADD;
	document.getElementById("curmode").innerHTML = "Add track";
}

let tahFirst = -1;

function onclickTrackAddHandler(ax, ay) {
	if(tahFirst === -1) {
		tahFirst = getCollidingNode(ax, ay);
		//first node - returns -1 if fails
		if(tahFirst !== -1) {
			//if found first node
			nodeList[tahFirst].draw("#ff00ff");
		}
	}
	else {
		let secondNode = getCollidingNode(ax, ay);
		nodeList[tahFirst].draw();
		if(secondNode == -1) {
			tahFirst = -1; //reset selection
			return;
		}
		if(secondNode == tahFirst) {
			tahFirst = -1; //reset selection
			alert("Error: second node same as first.");
			return;
		}

		//check if such track already exists (popup alert and ignore it)
		for(let i = 0; i < trackList.length; i++) {
			//if first and second node link to each other
			if(
				(trackList[i].nodeIdFirst == tahFirst ||
				trackList[i].nodeIdFirst == secondNode) &&
				(trackList[i].nodeIdSecond == tahFirst ||
				trackList[i].nodeIdSecond == secondNode) 
			) {
				alert("Error: track already exists.");
				tahFirst = -1;
				return;
			}
		}

		trackList.push(new Track(tahFirst, secondNode, nodeList[tahFirst].height, nodeList[secondNode].height))
		tahFirst = -1;
		trackList.at(-1).draw();
	}
}

function buildingAdd() {
	currentMode = mode.BUILDING_ADD;
	document.getElementById("curmode").innerHTML = "Add building";
}

function treeAdd() {
	currentMode = mode.TREE_ADD;
	document.getElementById("curmode").innerHTML = "Add tree";
}

function lightAdd() {
	currentMode = mode.LIGHT_ADD;
	document.getElementById("curmode").innerHTML = "Add light pole";
}


function edit() {
	currentMode = mode.EDIT;
	document.getElementById("curmode").innerHTML = "Edit";
}
function onclickEditHandler(ax, ay) {
	let node = getCollidingNode(ax, ay);
	if(node !== -1) {
		nodeEditMenu(node);
	}
	else {
		canvasData.edit.innerHTML = "";
	}
}