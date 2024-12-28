//normal semaphore for trams
class Signal {
	xpos = 0;
	ypos = 0;
	height = 0;
	poleHeight = 2; //values <= 0 means that is hangs on catenary wire
	stationCode = "";

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	//teal square
	draw(style = "#0000aa") {
		if(!this.willRender()) { return; }
		console.log("signal draw");
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
}

let signalList = [];

function signalEditMenu(aid) {

}

function signalUpdate() {
	console.log("Updating signal..");
}
function signalRemove() {
	console.log("Removing signal..");
}

//----------

class SwitchStateSignalUnit {
	letter = ""; //letter shown near signal
}

let signalUnits = []; //switches write here

//switch semaphore - shows
//can show state of multiple switches - every switch has letter
class SwitchStateSignal {
	xpos = 0;
	ypos = 0;
	height = 0;
	stationCode = "";

	unitIds = [];

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	//teal square with switch color border + text showing how many units
	draw(style = "#0000aa", style2 = "#aa6600") {
		if(!this.willRender()) { return; }
		console.log("switch signal draw");

		canvasData.context.fillStyle = style;
		canvasData.context.strokeStyle = style2;

		canvasData.context.lineWidth = LINE_WIDTH/2;

		canvasData.context.fillRect(
			this.xpos - NODE_SIZE/2,
			this.ypos - NODE_SIZE/2,
		NODE_SIZE, NODE_SIZE);

		canvasData.context.strokeRect(
			this.xpos - NODE_SIZE/2 + canvasData.context.lineWidth/2,
			this.ypos - NODE_SIZE/2 + canvasData.context.lineWidth/2,
		NODE_SIZE-canvasData.context.lineWidth, NODE_SIZE-canvasData.context.lineWidth);

		canvasData.context.lineWidth = LINE_WIDTH;

		canvasData.context.fillStyle = style2;
		canvasData.context.fillText(
			String(this.unitIds.length),
			this.xpos, this.ypos, NODE_SIZE
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

}

function switchSignalUpdate() {
	console.log("Updating switch signal..");
}
function switchSignalRemove() {
	console.log("Removing switch signal..");
}