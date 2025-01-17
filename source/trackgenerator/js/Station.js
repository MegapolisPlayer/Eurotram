class StationPillar {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;
	stationCode = "";

	//TODO support more types of pillars in the future release

	constructor(axpos = 0, aypos = 0) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#aaaa00") {
		if(!this.willRender()) { return; }
		console.log("station pillar draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-NODE_SIZE/2,-NODE_SIZE/4);
		
		canvasData.context.fillStyle = style;
		canvasData.context.strokeStyle = SELECT_COLOR;

		canvasData.context.fillRect(0, 0, NODE_SIZE, NODE_SIZE/2);

		canvasData.context.lineWidth = LINE_WIDTH/2;
		canvasData.context.beginPath();
		canvasData.context.moveTo(0, NODE_SIZE/2-LINE_WIDTH/4);
		canvasData.context.lineTo(NODE_SIZE, NODE_SIZE/2-LINE_WIDTH/4);
		canvasData.context.stroke();
		canvasData.context.lineWidth = LINE_WIDTH;

		canvasData.context.translate(NODE_SIZE/2,NODE_SIZE/4);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(-this.xpos, -this.ypos);
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

let stationPillarList = [];

function stationPillarEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing station pillar "+aid));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aid);

	addBasicEditInputs(stationPillarList[aid]);
	canvasData.edit.appendChild(document.createTextNode("Rotation:"));
	addInput("editrotinput", stationPillarList[aid].rotation, "number");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", stationPillarUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", stationPillarRemove);
	canvasData.edit.appendChild(removeButton);
}

function stationPillarUpdate() {
	console.log("Updating station pillar...");

	let spId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(stationPillarList[spId]);
	stationPillarList[spId].rotation = Number(document.getElementById("editrotinput").value);

	canvasRedraw();
}
function stationPillarRemove() {
	console.log("Removing station pillar...");

	let spId =  Number(document.getElementById("idinput").value);
	stationPillarList.splice(spId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}

//uses some elements of track
//same system as node-switch
class StationTrack extends Track {
	stationCode = "";

	constructor(anodeIdFirst = 0, anodeIdSecond = 0, aswitchFirst = false, aswitchSecond = false) {
		super(anodeIdFirst, anodeIdSecond, aswitchFirst, aswitchSecond);
		this.stationCode = "";
	}

	draw(style = "#ff0000") {
		super.draw(style); //style override
	}
}

//IMPORTANT station tracks are saved in track list

function stationTrackEditMenu(aid) {
	canvasData.edit.replaceChildren();
	
	canvasData.edit.appendChild(document.createTextNode("Editing station track "+aid+" between"));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aid].firstIsSwitch ? " switch " : " node " + trackList[aid].nodeIdFirst
	));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aid].secondIsSwitch ? " switch " : " node " + trackList[aid].nodeIdSecond
	));

	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("idinput", aid);

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	addInputPlaceholder("editcodeinput", trackList[aid].stationCode, "text", "XXXX");

	addTrackEditInputs(trackList[aid], aid);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", stationTrackUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", trackRemove);
	canvasData.edit.appendChild(removeButton);
}

function stationTrackUpdate() {
	console.log("Updating station track...");

	let trackId = Number(document.getElementById("idinput").value);
	trackList[trackId].stationCode = String(document.getElementById("editcodeinput").value);

	getDataFromTrackInputs(trackList[trackId]);

	canvasRedraw();

	if(trackList[trackId].bezier) {
		trackList[trackId].drawControlPts(); //draw only for edited track
	}
}

//removing is done by track function

/*

// Station platforms will be added in a future release

//short: 30m (1 tram / 2xT3)
//long: 70m (2 trams / 4xT3)

let stationPlatformType = {
	NONE: 0, //no platform, e.g. Sumavska
	
	//slim, just small asphalt island - no fences, nothing

	OLD_SHORT: 1, //e.g. Jiriho z Podebrad (to center)
	OLD_LONG: 2, //e.g. Dlouha Trida

	//tiled, may have fences

	MIDDLE_SHORT: 3, //e.g. Ruska
	MIDDLE_SHORT_FENCE: 4, //e.g. Vyton (from center)
	MIDDLE_LONG: 5, //e.g. Sumavska, Cechovo nam.
	MIDDLE_LONG_FENCE: 6, //e.g. Flora

	//asphalt with red line

	NEW_SHORT: 7, //e.g. Vinohradska trznice
	NEW_SHORT_FENCE: 8, //e.g. Libus (exit only)
	NEW_LONG: 9, //e.g. Krejcarek
	NEW_LONG_FENCE: 10, //e.g. U Zvonu
};

class StationPlatform {
	xpos = 0;
	ypos = 0;
}

let stationPlatformList = [];

function stationPlatformEditMenu(aid) {
	
}
*/