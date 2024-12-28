class StationPillar {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;
	stationCode = "";

	//TODO support more types of pillars in the future

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#ffff00") {
		if(!this.willRender()) { return; }
		console.log("station pillar draw");

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

let stationPillarList = [];

function stationPillarEditMenu(aid) {
	
}

//uses some elements of track
//same system as node-switch
class StationTrack extends Track {
	stationCode = "";

	constructor(anodeIdFirst, anodeIdSecond) {
		super(anodeIdFirst, anodeIdSecond);
	}

	draw(style = "#ff0000") {
		super.draw(style); //style override
	}
}

//saved in track list

function stationTrackEditMenu(aid) {
	canvasData.edit.innerHTML = "";
	
	canvasData.edit.innerHTML += "Editing station track "+aid+" between nodes "+trackList[aid].nodeIdFirst+" and "+trackList[aid].nodeIdSecond+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "Station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+trackList[aid].stationCode+"><br>";

	canvasData.edit.innerHTML += addTrackEditInputs(trackList[aid], aid);

	canvasData.edit.innerHTML +="<button type='' onclick='stationTrackUpdate()'>Update station track</button>";
	canvasData.edit.innerHTML +="<button type='' onclick='trackRemove()'>Remove track</button>";
}

function stationTrackUpdate() {
	console.log("Updating station track...");

	let trackId = Number(document.getElementById("idinput").value);

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