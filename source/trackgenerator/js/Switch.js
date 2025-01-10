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

	let idinput = document.createElement("input");
	idinput.type = "hidden";
	idinput.id = "input";
	idinput.name = idinput.id;
	idinput.setAttribute("value", aid);
	canvasData.edit.appendChild(idinput);
	canvasData.edit.appendChild(document.createElement("br"));

	addBasicEditInputs(switchList[aid]);
	
	canvasData.edit.appendChild(document.createTextNode("Before node id: "));
	let editbefinput = document.createElement("input");
	editbefinput.id = "editbefinput";
	editbefinput.name = editbefinput.id;
	editbefinput.setAttribute("value", switchList[aid].beforeId);
	editbefinput.min = -1;
	editbefinput.max = (switchList.length-1);
	canvasData.edit.appendChild(editbefinput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Front node id: "));
	let editfroinput = document.createElement("input");
	editfroinput.id = "editfroinput";
	editfroinput.name = editfroinput.id;
	editfroinput.setAttribute("value", switchList[aid].frontId);
	editfroinput.min = -1;
	editfroinput.max = (switchList.length-1);
	canvasData.edit.appendChild(editfroinput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Left node id: "));
	let editlefinput = document.createElement("input");
	editlefinput.id = "editlefinput";
	editlefinput.name = editlefinput.id;
	editlefinput.setAttribute("value", switchList[aid].leftId);
	editlefinput.min = -1;
	editlefinput.max = (switchList.length-1);
	canvasData.edit.appendChild(editlefinput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Right node id: "));
	let editriginput = document.createElement("input");
	editriginput.id = "editriginput";
	editriginput.name = editriginput.id;
	editriginput.setAttribute("value", switchList[aid].rightId);
	editriginput.min = -1;
	editriginput.max = (switchList.length-1);
	canvasData.edit.appendChild(editriginput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Radio box id: "));
	let editradinput = document.createElement("input");
	editradinput.id = "editradinput";
	editradinput.name = editradinput.id;
	editradinput.setAttribute("value", switchList[aid].radioBoxId);
	editradinput.min = -1;
	editradinput.max = (switchList.length-1);
	canvasData.edit.appendChild(editradinput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Switch signal id: "));
	let editsiginput = document.createElement("input");
	editsiginput.id = "editsiginput";
	editsiginput.name = editsiginput.id;
	editsiginput.setAttribute("value", switchList[aid].signalId);
	editsiginput.min = -1;
	editsiginput.max = (switchList.length-1);
	canvasData.edit.appendChild(editsiginput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Switch signal letter: "));
	let editsiglinput = document.createElement("input");
	editsiglinput.id = "editsiglinput";
	editsiglinput.name = editsiglinput.id;
	editsiglinput.setAttribute("value", switchList[aid].signalLetter);
	editsiglinput.min = -1;
	editsiglinput.max = (switchList.length-1);
	canvasData.edit.appendChild(editsiglinput);

	canvasData.edit.appendChild(document.createElement("hr"));

	let em1 = document.createElement("em");
	em1.textContent = "A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.";
	canvasData.edit.appendChild(em1);

	canvasData.edit.appendChild(document.createElement("hr"));

	let em2 = document.createElement("em");
	em2.textContent = "If the switch signal letter is equal to '-' or the switch signal id is -1, the switch does not have a signal attached to it.";
	canvasData.edit.appendChild(em2);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		switchUpdate();
	})
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove switch";
	removeButton.addEventListener("click", () => {
		switchRemove();
	})
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

	switchList.splice(switchId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}