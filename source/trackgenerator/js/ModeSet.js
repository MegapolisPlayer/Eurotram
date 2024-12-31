//this approach is MUCH slower than using createElement and such
//here it is acceptable since we dont care about the performance of this part all that much
//canvas performance is the larger bottleneck here (and we do optimize it)
//speed data: https://stackoverflow.com/questions/7327056/appending-html-string-to-the-dom

//TODO for future release - rewriting of this to the createElement and appendChild syntax

function addBasicEditInputs(object) {
	return ""+
	"X:<input type='number' id='editxinput' name='editxinput' value="+object.xpos+"><br>"+
	"Y:<input type='number' id='edityinput' name='edityinput' value="+object.ypos+"><br>"+
	"Height:<input type='number' id='editheightinput' name='editheightinput' value="+object.height+"><br>"+
	"Station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+object.stationCode+"><br>";
}

function getDataFromBasicInputs(object) {
	object.xpos = Number(document.getElementById("editxinput").value);
	object.ypos = Number(document.getElementById("edityinput").value);
	object.height = Number(document.getElementById("editheightinput").value);
	object.stationCode = String(document.getElementById("editcodeinput").value);
}

function addTrackEditInputs(object, id) {
	let temp = "Bezier:<input type='checkbox' id='editbezinput' name='editbezinput' "+(object.bezier?"checked":"")+"><br>"+
	"<hr><em>Control point values have no effect if Bezier curves are disabled.</em><hr>"+
	"CP1,X:<input type='number' id='editcp1xinput' name='editcp1xinput' value="+object.controlPoint1.x+"><br>"+
	"CP1,Y:<input type='number' id='editcp1yinput' name='editcp1yinput' value="+object.controlPoint1.y+"><br>"+
	"CP2,X:<input type='number' id='editcp2xinput' name='editcp2xinput' value="+object.controlPoint2.x+"><br>"+
	"CP2,Y:<input type='number' id='editcp2yinput' name='editcp2yinput' value="+object.controlPoint2.y+"><br>"+
	"<button type='' onclick='trackEditMoveCP2toCP1("+id+")'>Move CP2 to CP1</button><br>"+
	"<button type='' onclick='trackEditRecalcHeight("+id+")'>Recalculate height points</button><br>"+
	"<button type='' onclick='trackEditRecalcCP("+id+")'>Reset control points</button><br>"+
	"<hr><h4>Edit heightpoints manually</h4>";

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		temp += "HgPt"+i+":<input type='number' id='edithght"+i+"' name='edithght"+i+"' value="+object.heightpoints[i]+"><br>";
	}

	return temp;
}

function getDataFromTrackInputs(object) {
	object.bezier = document.getElementById("editbezinput").checked;
	object.controlPoint1.x = Number(document.getElementById("editcp1xinput").value);
	object.controlPoint1.y = Number(document.getElementById("editcp1yinput").value);
	object.controlPoint2.x = Number(document.getElementById("editcp2xinput").value);
	object.controlPoint2.y = Number(document.getElementById("editcp2yinput").value);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		object.heightpoints[i] = Number(document.getElementById("edithght"+i).value);
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
	STATION_TRACK_ADD: 21,
	TEXTURE_PARCEL_ADD: 22,

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

function onclickTrackAddHandler(ax, ay, aisStation) {
	if(trackFirst === -1) {
		trackFirst = getColliding(nodeList, ax, ay);
		//first node - returns -1 if fails
		if(trackFirst !== -1) {
			//if found first node
			nodeList[trackFirst].draw(SELECT_COLOR);
		}
	}
	else {
		let secondTNode = getColliding(nodeList, ax, ay);
		nodeList[trackFirst].draw();
		if(secondTNode == -1) {
			trackFirst = -1; //reset selection
			return;
		}
		if(secondTNode == trackFirst) {
			trackFirst = -1; //reset selection
			alert("Error: second node same as first.");
			return;
		}

		//check if such track already exists (popup alert and ignore it)
		for(let i = 0; i < trackList.length; i++) {
			//if first and second node link to each other
			if(
				(trackList[i].nodeIdFirst == trackFirst ||
				trackList[i].nodeIdFirst == secondTNode) &&
				(trackList[i].nodeIdSecond == trackFirst ||
				trackList[i].nodeIdSecond == secondTNode) 
			) {
				alert("Error: track already exists.");
				trackFirst = -1;
				return;
			}
		}

		if(aisStation) {
			trackList.push(new StationTrack(trackFirst, secondTNode))
		}
		else {
			trackList.push(new Track(trackFirst, secondTNode))
		}
		trackFirst = -1;
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
function onclicktreeAddHandler(ax, ay) {
	treeList.push(new Tree(ax, ay));
	treeList.at(-1).draw();
}

function lightAdd() {
	currentMode = mode.LIGHT_ADD;
	canvasData.mode.innerHTML = "Add light pole";
}
function onclicklightAddHandler(ax, ay) {
	lightList.push(new Light(ax, ay));
	lightList.at(-1).draw();
}

function landmarkAdd() {
	currentMode = mode.LANDMARK_ADD;
	canvasData.mode.innerHTML = "Add landmark";
}
function onclicklandmarkAddHandler(ax, ay) {
	
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
	let node = getColliding(nodeList, ax, ay);
	if(node !== -1) {
		if(nodeList[node] instanceof TNode) {
			nodeEditMenu(node);
		}
		else if(nodeList[node] instanceof Switch) {
			switchEditMenu(node);
		}
		else {
			canvasData.edit.innerHTML = "";
		}

		return;
	}

	let pillar = getColliding(stationPillarList, ax, ay);
	if(pillar !== -1) {
		stationPillarEditMenu(pillar);
		return;
	}

	let signal = getColliding(signalList, ax, ay);
	if(signal !== -1) {
		signalEditMenu(signal);
		return;
	}

	let swsignal = getColliding(switchSignalList, ax, ay);
	if(swsignal !== -1) {
		switchSignalEditMenu(swsignal);
		return;
	}

	let light = getColliding(lightList, ax, ay);
	if(light !== -1) {
		lightEditMenu(light);
		return;
	}

	let tree = getColliding(treeList, ax, ay);
	if(tree !== -1) {
		treeEditMenu(tree);
		return;
	}

	let radio = getColliding(radioList, ax, ay);
	if(radio !== -1) {
		radioEditMenu(radio);
		return;
	}

	let building = getColliding(buildingList, ax, ay);
	if(building !== -1) {
		buildingEditMenu(building);
		return;
	}

	let landmark = getColliding(landmarkList, ax, ay);
	if(landmark !== -1) {
		landmarkEditMenu(landmark);
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
	}
	else {
		let secondTNode = getColliding(nodeList, ax, ay);
		nodeList[trackFirst].draw();
		if(secondTNode == -1) {
			trackFirst = -1; //reset selection
			return;
		}
		if(secondTNode == trackFirst) {
			trackFirst = -1; //reset selection
			alert("Error: second node same as first.");
			return;
		}

		for(let i = 0; i < trackList.length; i++) {
			if((
				trackList[i].nodeIdFirst == trackFirst ||
				trackList[i].nodeIdFirst == secondTNode) &&
				(trackList[i].nodeIdSecond == trackFirst ||
				trackList[i].nodeIdSecond == secondTNode)
			) {
				if(trackList[i] instanceof StationTrack) {
					stationTrackEditMenu(i);
				}
				else {
					trackEditMenu(i);
				}
				trackFirst = -1;
				return;
			}
		}
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