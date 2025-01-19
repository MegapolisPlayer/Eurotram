class StationPillar extends RotatedStandardPoint {
	stationCode = "";

	//TODO support more types of pillars in the future release

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
		this.pointSizeX = NODE_SIZE;
		this.pointSizeY = NODE_SIZE/2;
	}

	draw(aStyle = "#aaaa00") {
		abstractDrawPoint(aStyle, this, () => {
			canvasData.context.strokeStyle = SELECT_COLOR;
		
			canvasData.context.lineWidth = LINE_WIDTH/2;
			canvasData.context.beginPath();
			canvasData.context.moveTo(0, NODE_SIZE/2-LINE_WIDTH/4);
			canvasData.context.lineTo(NODE_SIZE, NODE_SIZE/2-LINE_WIDTH/4);
			canvasData.context.stroke();
			canvasData.context.lineWidth = LINE_WIDTH;
		});
	}
}

let stationPillarList = [];

function stationPillarEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing station pillar "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(stationPillarList[aID]);
	canvasData.edit.appendChild(document.createTextNode("Rotation:"));
	addInput("editrotinput", stationPillarList[aID].rotation, "number");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let spId =  getIDFromInput();
		getDataFromBasicInputs(stationPillarList[spId]);
		stationPillarList[spId].rotation = Number(document.getElementById("editrotinput").value);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(stationPillarList);
	});
	canvasData.edit.appendChild(removeButton);
}

//uses some elements of track
//same system as node-switch
class StationTrack extends Track {
	stationCode = "";

	constructor(anodeIdFirst = 0, anodeIdSecond = 0, aswitchFirst = false, aswitchSecond = false) {
		super(anodeIdFirst, anodeIdSecond, aswitchFirst, aswitchSecond);
		this.stationCode = "";
	}

	draw(aStyle = "#ff0000") {
		super.draw(aStyle); //aStyle override
	}
}

//IMPORTANT station tracks are saved in track list

function stationTrackEditMenu(aID) {
	canvasData.edit.replaceChildren();
	
	canvasData.edit.appendChild(document.createTextNode("Editing station track "+aID+" between"));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aID].firstIsSwitch ? " switch " : " node " + trackList[aID].nodeIdFirst
	));

	canvasData.edit.appendChild(document.createTextNode(
		trackList[aID].secondIsSwitch ? " switch " : " node " + trackList[aID].nodeIdSecond
	));

	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("idinput", aID);

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	addInputPlaceholder("editcodeinput", trackList[aID].stationCode, "text", "XXXX");

	addTrackEditInputs(trackList[aID], aID);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let trackId = getIDFromInput();
		trackList[trackId].stationCode = String(document.getElementById("editcodeinput").value);
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
	xPos = 0;
	yPos = 0;
}

let stationPlatformList = [];

function stationPlatformEditMenu(aID) {
	
}
*/