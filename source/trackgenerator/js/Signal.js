//normal semaphore for trams
class Signal {
	xpos = 0;
	ypos = 0;
	height = 0;
	poleHeight = 2; //values <= 0 means that is hangs on catenary wire
	rotation = 0;
	stationCode = "";

	constructor(axpos = 0, aypos = 0) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	//teal square
	draw(style = "#0000aa") {
		if(!this.willRender()) { return; }
		console.log("signal draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-NODE_SIZE/2,-NODE_SIZE/2);

		canvasData.context.fillStyle = style;
		canvasData.context.strokeStyle = SELECT_COLOR;

		canvasData.context.fillRect(0, 0, NODE_SIZE, NODE_SIZE);

		canvasData.context.lineWidth = LINE_WIDTH/2;
		canvasData.context.beginPath();
		canvasData.context.moveTo(0, NODE_SIZE-LINE_WIDTH/4);
		canvasData.context.lineTo(NODE_SIZE, NODE_SIZE-LINE_WIDTH/4);
		canvasData.context.stroke();
		canvasData.context.lineWidth = LINE_WIDTH;

		canvasData.context.translate(NODE_SIZE/2,NODE_SIZE/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(
			-this.xpos,
			-this.ypos,
		);
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
			NODE_SIZE, NODE_SIZE
		);
	}
}

let signalList = [];

function signalEditMenu(aid) {
	canvasData.edit.replaceChldren();

	canvasData.edit.appendChild(document.createTextNode("Editing signal "+aid));

	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aid);

	addBasicEditInputs(signalList[aid]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));

	addInput("editrotinput", signalList[aid].rotation, "number");

	let updateButton = document.createElement("button");
	updateButton.addEventListener("click", signalUpdate);
	updateButton.textContent = "Update signal track";
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.addEventListener("click", signalRemove);
	removeButton.textContent = "Remove signal";
	canvasData.edit.appendChild(removeButton);
}

function signalUpdate() {
	console.log("Updating signal...");

	let signalId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(signalList[signalId]);
	signalList[signalId].rotation = Number(document.getElementById("editrotinput").value);

	canvasRedraw();
}
function signalRemove() {
	console.log("Removing signal...");

	let signalId =  Number(document.getElementById("idinput").value);
	signalList.splice(signalId, 1);
	canvasData.edit.replaceChldren();

	canvasRedraw();
}

//----------

//switch semaphore - shows
//can show state of multiple switches - every switch has letter
class SwitchStateSignal {
	xpos = 0;
	ypos = 0;
	height = 0;
	stationCode = "";
	rotation = 0;

	unitIds = []; //stores array of letters, id in switch references to this obj

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	//teal square with switch color border + text showing how many units
	draw(style = "#0000aa", style2 = "#aa6600") {
		if(!this.willRender()) { return; }
		console.log("switch signal draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-NODE_SIZE/2,-NODE_SIZE/2);

		canvasData.context.fillStyle = style;
		canvasData.context.strokeStyle = style2;

		canvasData.context.fillRect(0, 0, NODE_SIZE, NODE_SIZE);

		canvasData.context.lineWidth = LINE_WIDTH/2;

		canvasData.context.strokeRect(
			canvasData.context.lineWidth/2,
			canvasData.context.lineWidth/2,
		NODE_SIZE-canvasData.context.lineWidth, NODE_SIZE-canvasData.context.lineWidth);

		canvasData.context.strokeStyle = SELECT_COLOR;

		canvasData.context.beginPath();
		canvasData.context.moveTo(0, NODE_SIZE-LINE_WIDTH/4);
		canvasData.context.lineTo(NODE_SIZE, NODE_SIZE-LINE_WIDTH/4);
		canvasData.context.stroke();

		canvasData.context.lineWidth = LINE_WIDTH;

		canvasData.context.fillStyle = style2;
		canvasData.context.fillText(
			String(this.unitIds.length),
			NODE_SIZE/2, NODE_SIZE/2, NODE_SIZE
		);

		canvasData.context.translate(NODE_SIZE/2,NODE_SIZE/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(
			-this.xpos,
			-this.ypos,
		);
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
}

let switchSignalList = [];

function switchSignalEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing switch signal "+aid));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("idinput", aid);

	addBasicEditInputs(switchSignalList[aid]);

	addInput("editrotinput", switchSignalList[aid].rotation, "number");	

	let updateButton = document.createElement("button");
	updateButton.appendChild(document.createTextNode("Update"));
	updateButton.addEventListener("click", switchSignalUpdate);
	canvasData.edit.appendChild(updateButton);
	
	let removeButton = document.createElement("button");
	removeButton.appendChild(document.createTextNode("Remove"));
	removeButton.addEventListener("click", switchSignalRemove);
	canvasData.edit.appendChild(removeButton);
}

function switchSignalUpdate() {
	console.log("Updating switch signal..");

	let swsignalId =  Number(document.getElementById("idinput").value);
	getDataFromBasicInputs(switchSignalList[swsignalId]);
	switchSignalList[swsignalId].rotation = Number(document.getElementById("editrotinput").value);

	canvasRedraw();
}
function switchSignalRemove() {
	console.log("Removing switch signal..");

	let swsignalId =  Number(document.getElementById("idinput").value);
	switchSignalList.splice(swsignalId, 1);
	canvasData.edit.replaceChldren();

	canvasRedraw();
}