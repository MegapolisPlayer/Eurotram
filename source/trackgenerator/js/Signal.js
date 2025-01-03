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
			NODE_SIZE, NODE_SIZE)
	}
}

let signalList = [];

function signalEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing signal "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += addBasicEditInputs(signalList[aid]);
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+signalList[aid].rotation+"><br>";

	canvasData.edit.innerHTML +="<button type='' onclick='signalUpdate()'>Update signal track</button>";
	canvasData.edit.innerHTML +="<button type='' onclick='signalRemove()'>Remove signal</button>";
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
	canvasData.edit.innerHTML = "";

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
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing switch signal "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";
	
	canvasData.edit.innerHTML += addBasicEditInputs(switchSignalList[aid]);
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+switchSignalList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='switchSignalUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='switchSignalRemove()'>Remove</button>";
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
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}