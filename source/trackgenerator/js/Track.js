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

	constructor(aTNodeIdFirst, aTNodeIdSecond) {
		this.nodeIdFirst = aTNodeIdFirst;
		this.nodeIdSecond = aTNodeIdSecond;
		
		this.recalculateHeightpoints();	
	}

	draw(style = "#000000") {
		if(nodeList[this.nodeIdFirst].willRender() || nodeList[this.nodeIdSecond].willRender()) {
			console.log("track draw");
			canvasData.context.fillStyle = style;
			canvasData.context.beginPath();
			canvasData.context.moveTo(
				canvasData.shiftX+nodeList[this.nodeIdFirst].xpos, 
				canvasData.shiftY+nodeList[this.nodeIdFirst].ypos);
			canvasData.context.lineTo(
				canvasData.shiftX+nodeList[this.nodeIdSecond].xpos, 
				canvasData.shiftY+nodeList[this.nodeIdSecond].ypos);
			canvasData.context.stroke();
		}
		else return;
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
};

let trackList = [];

function trackEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing track "+aid+" between nodes "+trackList[aid].nodeIdFirst+" and "+trackList[aid].nodeIdSecond+"<br>";

	//TODO add checkbox bezier/linear
	//if bezier - add 2 controlpoints which are rendered only when menu enabled
	//render in purple (selection color)
	//move using edit menu - rerender ONLY said curve (render previous with white before setting new nodes)

	canvasData.edit.innerHTML += "<button type='' onclick='trackEditRecalcHeight("+aid+")'>Recalculate height points</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='trackRemove()'>Remove track</button>";

	canvasData.edit.innerHTML += "<hr><h4>Edit heightpoints manually</h4>";
	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {

	}

}

function trackEditRecalcHeight(aid) {
	trackList[aid].recalculateHeightpoints();
}

function trackUpdate() {
	
}

function trackUpdateLinear() {
	
}

function trackUpdateBezier() {
	
}

function trackRemove() {

}