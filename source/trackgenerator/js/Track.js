const TRACK_HEIGHTPOINTS_AMOUNT = 10;

//track between 2 nodes
class Track {
	nodeIdFirst = 0;
	nodeIdSecond = 0;

	firstIsSwitch = false;
	secondIsSwitch = false;

	bezier = false; //if bezier or straight line
	controlPoint1 = {
		x: 0, y: 0
	};
	controlPoint2 = {
		x: 0, y: 0
	};

	//10 evenly spaced out points with height information
	heightpoints = [];

	constructor(anodeIdFirst = 0, anodeIdSecond = 0, aswitchFirst = false, aswitchSecond = false) {
		this.nodeIdFirst = anodeIdFirst;
		this.nodeIdSecond = anodeIdSecond;
		this.firstIsSwitch = aswitchFirst;
		this.secondIsSwitch = aswitchSecond;

		this.recalculateCP();
		this.recalculateHeightpoints();	
	}

	draw(style = "#aaaaaa") {
		let point1 = this.firstIsSwitch ? switchList[this.nodeIdFirst] : nodeList[this.nodeIdFirst];
		let point2 = this.secondIsSwitch ? switchList[this.nodeIdSecond] : nodeList[this.nodeIdSecond];

		if(point1.willRender() || point2.willRender()) {
			console.log("track draw");
			canvasData.context.strokeStyle = style;
			canvasData.context.beginPath();
			canvasData.context.moveTo(point1.xpos, point1.ypos);

			if(this.bezier) {
				canvasData.context.bezierCurveTo(
					//cp1
					this.controlPoint1.x,
					this.controlPoint1.y,
					//cp2
					this.controlPoint2.x,
					this.controlPoint2.y,
					//end point
					point2.xpos,
					point2.ypos
				);
				canvasData.context.stroke();
			}
			else {
				canvasData.context.lineTo(
					point2.xpos,
					point2.ypos
				);
				canvasData.context.stroke();
			}
		}
		else return;
	}

	drawControlPts(style = "#bbbbbb") {
		if(nodeList[this.nodeIdFirst].willRender() || nodeList[this.nodeIdSecond].willRender()) {
			console.log("track ctl draw");
			canvasData.context.fillStyle = style;

			canvasData.context.fillRect(
				this.controlPoint1.x - NODE_SIZE/2,
				this.controlPoint1.y - NODE_SIZE/2,
			NODE_SIZE, NODE_SIZE);
			canvasData.context.fillRect(
				this.controlPoint2.x - NODE_SIZE/2,
				this.controlPoint2.y - NODE_SIZE/2,
			NODE_SIZE, NODE_SIZE);
		}
	}

	recalculateHeightpoints() {
		this.heightpoints.length = 0;

		let point1 = this.firstIsSwitch ? switchList[this.nodeIdFirst] : nodeList[this.nodeIdFirst];
		let point2 = this.secondIsSwitch ? switchList[this.nodeIdSecond] : nodeList[this.nodeIdSecond];

		console.log(point1, point2);

		let stepHeightDelta = (point2.height-point1.height)/TRACK_HEIGHTPOINTS_AMOUNT;
		for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			//divide by delta for each p
			this.heightpoints.push(point1.height + stepHeightDelta*i);
		}
		console.log("HghtPts "+this.heightpoints);
	}

	recalculateCP() {
		//set control points to middle

		//get points (may be in switch list or node list)
		let point1 = this.firstIsSwitch ? switchList[this.nodeIdFirst] : nodeList[this.nodeIdFirst];
		let point2 = this.secondIsSwitch ? switchList[this.nodeIdSecond] : nodeList[this.nodeIdSecond];

		this.controlPoint1.x = (point1.xpos+point2.xpos)/2;
		this.controlPoint1.y = (point1.ypos+point2.ypos)/2;

		this.controlPoint2.x = this.controlPoint1.x;
		this.controlPoint2.y = this.controlPoint1.y;
	}
};

let trackList = [];

function trackRecalcAllHeightpoints() {
	trackList.forEach((v) => {
		v.recalculateHeightpoints();
	});
}

function trackEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing track "+aid+" between"));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aid].firstIsSwitch ? " switch " : " node " + trackList[aid].nodeIdFirst
	));
	canvasData.edit.appendChild(document.createTextNode(
		trackList[aid].secondIsSwitch ? " switch " : " node " + trackList[aid].nodeIdSecond
	));

	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aid);

	canvasData.edit.appendChild(document.createTextNode("Bezier: "));
	addInputCheckbox("editbezinput", trackList[aid].bezier);
	
	canvasData.edit.appendChild(document.createElement("hr"));
	let em = document.createElement("em");
	em.textContent = "Control point values have no effect if Bezier curves are disabled.";
	canvasData.edit.appendChild(em);

	canvasData.edit.appendChild(document.createTextNode("CP1X: "));
	addInput("editcp1xinput", trackList[aid].controlPoint1.x, "number");
	canvasData.edit.appendChild(document.createTextNode("CP1Y: "));
	addInput("editcp1yinput", trackList[aid].controlPoint1.y, "number");
	canvasData.edit.appendChild(document.createTextNode("CP2X: "));
	addInput("editcp2xinput", trackList[aid].controlPoint2.x , "number");
	canvasData.edit.appendChild(document.createTextNode("CP2Y: "));
	addInput("editcp2yinput", trackList[aid].controlPoint2.y, "number");
	
	let moveButton = document.createElement("button");
	moveButton.textContent = "Move CP2 to CP1";
	moveButton.onclick = "trackEditMoveCP2toCP1("+aid+")";
	canvasData.edit.appendChild(moveButton);

	let recalcButton = document.createElement("button");
	recalcButton.textContent = "Recalculate height points";
	recalcButton.onclick = "trackEditRecalcHeight("+aid+")";
	canvasData.edit.appendChild(recalcButton);

	let resetCPButton = document.createElement("button");
	resetCPButton.textContent = "Reset control points";
	resetCPButton.onclick = "trackEditRecalcCP("+aid+")";
	canvasData.edit.appendChild(resetCPButton);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", trackUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", trackRemove);
	canvasData.edit.appendChild(removeButton);

	canvasData.edit.appendChild(document.createElement("hr"));

	let h4 = document.createElement("h4");
	h4.textContent = "Edit heightpoints manually";
	canvasData.edit.appendChild(h4);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		canvasData.edit.appendChild(document.createTextNode("HgPt"+i+": "));
		addInput("edithght"+i, trackList[aid].heightpoints[i], "number");
	}
}

function trackEditMoveCP2toCP1(aid) {
	//move control pt 2 to ct 1 - when we only want to have single control point

	trackList[aid].controlPoint1.x = Number(document.getElementById("editcp1xinput").value);
	trackList[aid].controlPoint1.y = Number(document.getElementById("editcp1yinput").value);

	trackList[aid].controlPoint2.x = trackList[aid].controlPoint1.x;
	trackList[aid].controlPoint2.y = trackList[aid].controlPoint1.y;

	canvasRedraw();

	trackList[aid].drawControlPts();

	document.getElementById("editcp2xinput").value = trackList[aid].controlPoint2.x;
	document.getElementById("editcp2yinput").value = trackList[aid].controlPoint2.y;
}

function trackEditRecalcHeight(aid) {
	trackList[aid].recalculateHeightpoints();
}

function trackEditRecalcCP(aid) {
	trackList[aid].recalculateCP();
	canvasRedraw();
	trackList[aid].drawControlPts();
}

function trackUpdate() {
	console.log("Updating track...");

	let trackId = Number(document.getElementById("idinput").value);

	getDataFromTrackInputs(trackList[trackId]);

	canvasRedraw();

	if(trackList[trackId].bezier) {
		trackList[trackId].drawControlPts(); //draw only for edited track
	}
}

function trackRemove() {
	console.log("Removing track...");

	let trackId = Number(document.getElementById("idinput").value);
	trackList.splice(trackId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}