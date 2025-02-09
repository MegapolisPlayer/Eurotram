//normal semaphore for trams
class Signal extends RotatedStandardPoint {
	poleHeight = 2; //values <= 0 means that is hangs on catenary wire
	stationCode = "";

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
	}

	draw(aStyle = "#0000aa") {
		abstractDrawPoint(aStyle, this, () => {
			canvasData.context.lineWidth = LINE_WIDTH/2;
			canvasData.context.beginPath();
			canvasData.context.moveTo(0, NODE_SIZE-LINE_WIDTH/4);
			canvasData.context.lineTo(NODE_SIZE, NODE_SIZE-LINE_WIDTH/4);
			canvasData.context.stroke();
			canvasData.context.lineWidth = LINE_WIDTH;
		});
	}
}

let signalList = [];

function signalEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing signal "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(signalList[aID]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));

	addInput("editrotinput", signalList[aID].rotation, "number");

	let updateButton = document.createElement("button");
	updateButton.addEventListener("click", () => {
		let signalId =  getIDFromInput();
		getDataFromBasicInputs(signalList[signalId]);
		signalList[signalId].rotation = Number(document.getElementById("editrotinput").value);
		canvasRedraw();
	});
	updateButton.textContent = "Update";
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.addEventListener("click", () => {
		removeFromListById(signalList);
	});
	removeButton.textContent = "Remove";
	canvasData.edit.appendChild(removeButton);
}

//----------

class Presignal extends Signal {
	signalId = 0; //which signal do we reference

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
	}

	draw(aStyle = "#00ffff") {
		super.draw(aStyle); //aStyle override
	}
};

//IMPORTANT presignals are saved in signal list

function presignalEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing presignal "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(signalList[aID]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));
	addInput("editrotinput", signalList[aID].rotation, "number");

	canvasData.edit.appendChild(document.createTextNode("Signal id: "));
	addInput("editsiginput", signalList[aID].signalId, "number");

	let updateButton = document.createElement("button");
	updateButton.addEventListener("click", () => {
		let signalId = getIDFromInput();
		getDataFromBasicInputs(signalList[signalId]);
		signalList[signalId].rotation = Number(document.getElementById("editrotinput").value);
		signalList[signalId].signalId = Number(document.getElementById("editsiginput").value);
		canvasRedraw();
	});
	updateButton.textContent = "Update";
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.addEventListener("click", () => {
		removeFromListById(signalList);
	});
	removeButton.textContent = "Remove";
	canvasData.edit.appendChild(removeButton);
}

//remove handled by signal

//----------

//switch semaphore - shows
//can show state of multiple switches - every switch has letter
class SwitchStateSignal extends RotatedStandardPoint {
	stationCode = "";

	unitIds = []; //stores array of letters, id in switch references to this obj

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
	}

	//teal square with switch color border + text showing how many units
	draw(aStyle = "#0000aa", style2 = "#aa6600") {
		abstractDrawPoint(aStyle, this, () => {
			canvasData.context.strokeStyle = style2;

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
		});
	}
}

let switchSignalList = [];

function switchSignalEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing switch signal "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("idinput", aID);

	addBasicEditInputs(switchSignalList[aID]);

	addInput("editrotinput", switchSignalList[aID].rotation, "number");	

	let updateButton = document.createElement("button");
	updateButton.appendChild(document.createTextNode("Update"));
	updateButton.addEventListener("click", () => {
		let swsignalId =  getIDFromInput();
		getDataFromBasicInputs(switchSignalList[swsignalId]);
		switchSignalList[swsignalId].rotation = Number(document.getElementById("editrotinput").value);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);
	
	let removeButton = document.createElement("button");
	removeButton.appendChild(document.createTextNode("Remove"));
	removeButton.addEventListener("click", () => {
		removeFromListById(switchSignalList);
	});
	canvasData.edit.appendChild(removeButton);
}