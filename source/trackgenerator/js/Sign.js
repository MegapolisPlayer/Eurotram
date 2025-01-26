class Sign extends RotatedStandardPoint{
	stationCode = 0;
	type = 0;

	constructor(axPos = 0, ayPos = 0, aRotation = 0, aType = 0, astationCode = "") {
		super(axPos, ayPos, aRotation);
		this.type = aType;
		this.stationCode = astationCode;
		this.pointSizeX = NODE_SIZE;
		this.pointSizeY = NODE_SIZE/2;
	}

	draw(aStyle = "#008000") {
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
};

let signList = [];

let signTypeSelector;

let signTypes = {
	NONE: 0,
	MANUAL_SWITCH: 1,
	STOP: 2,
	MAX_SPEED_UNEXPECTED: 4,
	MAX_SPEED_UNEXPECTED_END: 5,
	SLOW: 6,
	MAX_SPEED: 7,
	MAX_SPEED_END: 8,
	MAX_SPEED_POINT: 9,
	SLOW_SWITCH: 10,
	TIGHT_CURVE: 11,
	ZONE_30: 12,
	ZONE_40: 13,
	ZONE_30_END: 14,
	ZONE_40_END: 15,
	SEGMENT_DIVIDER: 16,
	SEGMENT_DIVIDER_MAGNET: 17,
	PANTOGRAPH_DOWN: 18,
	PANTOGRAPH_UP: 19
};

function makeSignTypeSelector() {
	signTypeSelector = new DocumentFragment();

	let label = document.createElement("label");
	label.textContent = "Select sign";

	let select = document.createElement("select");
	select.name = "signtypeinput";
	select.id = "signtypeinput";

	for(let s in signTypes) {
		let option = document.createElement("option");
		option.textContent = s;
		option.setAttribute("value", signTypes[s]);
		select.appendChild(option);
	}

	signTypeSelector.appendChild(label)
	signTypeSelector.appendChild(select);
	signTypeSelector.appendChild(document.createElement("br"));
}

function signSelectMenu(aX, aY) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Select sign type at pos x = "+aX+", y = "+aY));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("signxinput", aX);
	addHiddenInput("signyinput", aY);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));
	addInput("signrotinput", 0, "text");

	canvasData.edit.appendChild(document.createTextNode("Station code:"));

	addInputPlaceholder("signcodeinput", "", "text", "XXXX");

	let dfcopy = signTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);

	let makeButton = document.createElement("button");
	makeButton.textContent = "Make sign";
	makeButton.addEventListener("click", () => {
		let x = Number(document.getElementById("signxinput").value);
		let y = Number(document.getElementById("signyinput").value);
		let r = Number(document.getElementById("signrotinput").value);
		let type = Number(document.getElementById("signtypeinput").value);
		let stationCode = document.getElementById("signcodeinput").value;
		signList.push(new Sign(x, y, r, type, stationCode));
		signList.at(-1).draw();
		canvasData.edit.replaceChildren();
	});
	canvasData.edit.appendChild(makeButton);
}

function signEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing sign "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(signList[aID]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));
	addInput("editrotinput", signList[aID].rotation, "text");

	canvasData.edit.appendChild(document.createTextNode("Current: "));
	let updateBlock1 = document.createElement("span");
	updateBlock1.id = "currentselected";
	updateBlock1.textContent = getPropertyOfValue(signTypes, signList[aID].type);
	canvasData.edit.appendChild(updateBlock1);
	canvasData.edit.appendChild(document.createElement("br"));

	let dfcopy = signTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);

	let updateButton = document.createElement("button");
	updateButton.appendChild(document.createTextNode("Update"));
	updateButton.addEventListener("click", () => {
		let signId = getIDFromInput();
		getDataFromBasicInputs(signList[signId]);
		signList[signId].rotation = Number(document.getElementById("editrotinput").value);
		signList[signId].type = Number(document.getElementById("signtypeinput").value);
		document.getElementById("currentselected").textContent = getPropertyOfValue(signTypes, signList[signId].type);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.appendChild(document.createTextNode("Remove"));
	removeButton.addEventListener("click", () => {
		removeFromListById(signList);
	});
	canvasData.edit.appendChild(removeButton);
}
