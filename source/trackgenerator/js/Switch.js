class Switch {
	xpos = 0; //same as node, interchangable
	ypos = 0;
	height = 0;
	stationCode = "";
	//switch cannot border 2 stations

	beforeId = -1; //before split
	frontId = -1; //after split
	leftId = -1; 
	rightId = -1;

	radioBoxId = -1;
	signalId = -1;
	signalLetter = '-';

	constructor(axpos = 0, aypos = 0, astationCode = "") {
		this.xpos = axpos;
		this.ypos = aypos;
		this.stationCode = astationCode;	
	}

	draw(style = "#aa6600") {
		if(!this.willRender()) { return; }
		console.log("switch draw");
		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(
			this.xpos - NODE_SIZE/2,
			this.ypos - NODE_SIZE/2,
		NODE_SIZE, NODE_SIZE);
	}

	collision(ax, ay) {
		return (ax >= this.xpos-NODE_SIZE) &&
			(ax <= this.xpos+NODE_SIZE) &&
			(ay >= this.ypos-NODE_SIZE) &&
			(ay <= this.ypos+NODE_SIZE);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xpos - NODE_SIZE/2,
			this.ypos - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE)
	}
};

let switchList = [];

function switchEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing switch no. "+aid));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aid);
	canvasData.edit.appendChild(document.createElement("br"));

	addBasicEditInputs(switchList[aid]);
	
	canvasData.edit.appendChild(document.createTextNode("Before node id: "));
	addInput(aid, switchList[aid].beforeId, "text");

	canvasData.edit.appendChild(document.createTextNode("Front node id: "));
	addInput(aid, switchList[aid].frontId, "text");

	canvasData.edit.appendChild(document.createTextNode("Left node id: "));
	addInput(aid, switchList[aid].leftId, "text");

	canvasData.edit.appendChild(document.createTextNode("Right node id: "));
	addInput(aid, switchList[aid].rightId, "text");

	canvasData.edit.appendChild(document.createTextNode("Radio box id: "));
	addInput(aid, switchList[aid].radioBoxId, "text");

	canvasData.edit.appendChild(document.createTextNode("Switch signal id: "));
	addInput(aid, switchList[aid].signalId, "text");

	canvasData.edit.appendChild(document.createTextNode("Switch signal letter: "));
	addInput(aid, switchList[aid].signalLetter, "text");

	canvasData.edit.appendChild(document.createElement("hr"));

	let em1 = document.createElement("em");
	em1.textContent = "A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.";
	canvasData.edit.appendChild(em1);
	canvasData.edit.appendChild(document.createElement("hr"));

	let em2 = document.createElement("em");
	em2.textContent = "If the switch signal letter is equal to '-' or the switch signal id is -1, the switch does not have a signal attached to it.";
	canvasData.edit.appendChild(em2);
	canvasData.edit.appendChild(document.createElement("hr"));

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", switchUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove switch";
	removeButton.addEventListener("click", switchRemove);
	canvasData.edit.appendChild(removeButton);
}

function switchUpdate() {
	console.log("Updating switch...");

	let switchId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(switchList[switchId]);

	//TODO check if switch doesnt self-reference

	switchList[switchId].beforeId = Number(document.getElementById("editbefinput").value);
	switchList[switchId].frontId = Number(document.getElementById("editfroinput").value);
	switchList[switchId].leftId = Number(document.getElementById("editlefinput").value);
	switchList[switchId].rightId = Number(document.getElementById("editriginput").value);
	switchList[switchId].radioBoxId = Number(document.getElementById("editradinput").value);
	switchList[switchId].signalId = Number(document.getElementById("editsiginput").value);
	switchList[switchId].signalLetter = document.getElementById("editsiglinput").value;

	canvasRedraw();
}

function switchRemove() {
	console.log("Removing switch and connected track");

	let switchId = Number(document.getElementById("idinput").value);

	trackList = trackList.filter((v, i) => {
		return !((v.nodeIdFirst === switchId && v.firstIsSwitch) || (v.nodeIdSecond === switchId && v.secondIsSwitch));
	});

	//shift track
	trackList.forEach((v, i, a) => {
		if(v.nodeIdFirst > switchId && v.firstIsSwitch) {
			a[i].nodeIdFirst--;
		}
		if(v.nodeIdSecond > switchId && v.secondIsSwitch) {
			a[i].nodeIdSecond--;
		}
	});

	switchList.splice(switchId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}