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

	draw(aStyle = "#aaaaaa") {
		let point1 = this.firstIsSwitch ? switchList[this.nodeIdFirst] : nodeList[this.nodeIdFirst];
		let point2 = this.secondIsSwitch ? switchList[this.nodeIdSecond] : nodeList[this.nodeIdSecond];

		if(point1.willRender() || point2.willRender()) {
			canvasData.context.strokeStyle = aStyle;
			canvasData.context.beginPath();
			canvasData.context.moveTo(point1.xPos, point1.yPos);

			if(this.bezier) {
				canvasData.context.bezierCurveTo(
					//cp1
					this.controlPoint1.x,
					this.controlPoint1.y,
					//cp2
					this.controlPoint2.x,
					this.controlPoint2.y,
					//end point
					point2.xPos,
					point2.yPos
				);
				canvasData.context.stroke();
			}
			else {
				canvasData.context.lineTo(
					point2.xPos,
					point2.yPos
				);
				canvasData.context.stroke();
			}
		}
		else return;
	}

	drawControlPts(aStyle = "#bbbbbb") {
		if(nodeList[this.nodeIdFirst].willRender() || nodeList[this.nodeIdSecond].willRender()) {
			canvasData.context.fillStyle = aStyle;

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

		//console.log(point1, point2);

		let stepHeightDelta = (point2.height-point1.height)/TRACK_HEIGHTPOINTS_AMOUNT;
		for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			//divide by delta for each p
			this.heightpoints.push(point1.height + stepHeightDelta*i);
		}
		//console.log("HghtPts "+this.heightpoints);
	}

	recalculateCP() {
		//set control points to middle

		//get points (may be in switch list or node list)
		let point1 = this.firstIsSwitch ? switchList[this.nodeIdFirst] : nodeList[this.nodeIdFirst];
		let point2 = this.secondIsSwitch ? switchList[this.nodeIdSecond] : nodeList[this.nodeIdSecond];

		this.controlPoint1.x = (point1.xPos+point2.xPos)/2;
		this.controlPoint1.y = (point1.yPos+point2.yPos)/2;

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

function trackEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing track "+aID+" between"));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aID].firstIsSwitch ? " switch " : " node " + trackList[aID].nodeIdFirst
	));
	canvasData.edit.appendChild(document.createTextNode(
		trackList[aID].secondIsSwitch ? " switch " : " node " + trackList[aID].nodeIdSecond
	));

	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addTrackEditInputs(trackList[aID], aID);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let trackId = getIDFromInput();
		getDataFromTrackInputs(trackList[trackId]);
		canvasRedraw();
		if(trackList[trackId].bezier) {
			trackList[trackId].drawControlPts(); //draw only for edited track
		}
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(trackList);
	});
	canvasData.edit.appendChild(removeButton);
}

function trackEditMoveCP2toCP1(aID) {
	//move control pt 2 to ct 1 - when we only want to have single control point

	trackList[aID].controlPoint1.x = Number(document.getElementById("editcp1xinput").value);
	trackList[aID].controlPoint1.y = Number(document.getElementById("editcp1yinput").value);

	trackList[aID].controlPoint2.x = trackList[aID].controlPoint1.x;
	trackList[aID].controlPoint2.y = trackList[aID].controlPoint1.y;

	canvasRedraw();

	trackList[aID].drawControlPts();

	document.getElementById("editcp2xinput").value = trackList[aID].controlPoint2.x;
	document.getElementById("editcp2yinput").value = trackList[aID].controlPoint2.y;
}

function trackEditRecalcHeight(aID) {
	trackList[aID].recalculateHeightpoints();
}

function trackEditRecalcCP(aID) {
	trackList[aID].recalculateCP();
	canvasRedraw();
	trackList[aID].drawControlPts();
}