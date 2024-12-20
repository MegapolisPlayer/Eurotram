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

	constructor(aNodeIdFirst, aNodeIdSecond, aStartHeight, aEndHeight) {
		this.nodeIdFirst = aNodeIdFirst;
		this.nodeIdSecond = aNodeIdSecond;
		
		let stepHeightDelta = ((aEndHeight-aStartHeight)/TRACK_HEIGHTPOINTS_AMOUNT);
		for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			//divide by delta for each p
			this.heightpoints.push(aStartHeight + stepHeightDelta*i);
		}
		console.log("HghtPts "+this.heightpoints);
	}

	draw() {
		canvasData.context.fillStyle = "#000000";
		canvasData.context.beginPath();
		canvasData.context.moveTo(
			canvasData.shiftX+nodeList[this.nodeIdFirst].xpos, 
			canvasData.shiftY+nodeList[this.nodeIdFirst].ypos);
		canvasData.context.lineTo(
			canvasData.shiftX+nodeList[this.nodeIdSecond].xpos, 
			canvasData.shiftY+nodeList[this.nodeIdSecond].ypos);
		canvasData.context.stroke();
	}
};

let trackList = [];

function trackEditMenu(aid) {

}

function trackUpdate() {
	
}