//---------------------------------------------------------------------------------------
// TRACK
//---------------------------------------------------------------------------------------

const TRACK_HEIGHTPOINTS_AMOUNT = 10;

//track between 2 nodes
class Track {
	nodeIdFirst = 0;
	nodeIdSecond = 0;

	bezier = false; //if bezier or straight line
	controlPoint1 = {
		x: 0, y: 0
	};
	controlPoint2 = {
		x: 0, y: 0
	};

	//10 evenly spaced out points with height information
	heightpoints = [];

	constructor(anodeIdFirst, anodeIdSecond) {
		this.nodeIdFirst = anodeIdFirst;
		this.nodeIdSecond = anodeIdSecond;
		this.bezier = false;

		this.recalculateCP();

		this.recalculateHeightpoints();	
	}

	draw(style = "#aaaaaa") {
		if(nodeList[this.nodeIdFirst].willRender() || nodeList[this.nodeIdSecond].willRender()) {
			console.log("track draw");
			canvasData.context.strokeStyle = style;
			canvasData.context.beginPath();
			canvasData.context.moveTo(
				nodeList[this.nodeIdFirst].xpos, 
				nodeList[this.nodeIdFirst].ypos);

			if(this.bezier) {
				canvasData.context.bezierCurveTo(
					//cp1
					this.controlPoint1.x,
					this.controlPoint1.y,
					//cp2
					this.controlPoint2.x,
					this.controlPoint2.y,
					//end point
					nodeList[this.nodeIdSecond].xpos, 
					nodeList[this.nodeIdSecond].ypos
				);
				canvasData.context.stroke();
			}
			else {
				canvasData.context.lineTo(
					nodeList[this.nodeIdSecond].xpos, 
					nodeList[this.nodeIdSecond].ypos);
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
		let stepHeightDelta = (nodeList[this.nodeIdSecond].height-nodeList[this.nodeIdFirst].height)/TRACK_HEIGHTPOINTS_AMOUNT;
		for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			//divide by delta for each p
			this.heightpoints.push(nodeList[this.nodeIdFirst].height + stepHeightDelta*i);
		}
		console.log("HghtPts "+this.heightpoints);
	}

	recalculateCP() {
		//set control points to middle
		this.controlPoint1.x = (nodeList[this.nodeIdFirst].xpos+nodeList[this.nodeIdSecond].xpos)/2;
		this.controlPoint1.y = (nodeList[this.nodeIdFirst].ypos+nodeList[this.nodeIdSecond].ypos)/2;

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
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing track "+aid+" between nodes "+trackList[aid].nodeIdFirst+" and "+trackList[aid].nodeIdSecond+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "Bezier:<input type='checkbox' id='editbezinput' name='editbezinput' "+(trackList[aid].bezier?"checked":"")+"><br>";
	
	canvasData.edit.innerHTML += "<hr><em>Control point values have no effect if Bezier curves are disabled.</em><hr>";

	canvasData.edit.innerHTML += "CP1X:<input type='number' id='editcp1xinput' name='editcp1xinput' value="+trackList[aid].controlPoint1.x+"><br>";
	canvasData.edit.innerHTML += "CP1Y:<input type='number' id='editcp1yinput' name='editcp1yinput' value="+trackList[aid].controlPoint1.y+"><br>";
	canvasData.edit.innerHTML += "CP2X:<input type='number' id='editcp2xinput' name='editcp2xinput' value="+trackList[aid].controlPoint2.x+"><br>";
	canvasData.edit.innerHTML += "CP2Y:<input type='number' id='editcp2yinput' name='editcp2yinput' value="+trackList[aid].controlPoint2.y+"><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='trackEditMoveCP2toCP1("+aid+")'>Move CP2 to CP1</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackEditRecalcHeight("+aid+")'>Recalculate height points</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackEditRecalcCP("+aid+")'>Reset control points</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackRemove()'>Remove track</button>";

	canvasData.edit.innerHTML += "<hr><h4>Edit heightpoints manually</h4>";
	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		canvasData.edit.innerHTML += "HgPt"+i+":<input type='number' id='edithght"+i+"' name='edithght"+i+"' value="+trackList[aid].heightpoints[i]+"><br>";
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
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}