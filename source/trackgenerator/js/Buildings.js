let buildingType = {
	NORMAL: 0,
	CORNER: 1,
	FAMILY: 2,
	PREFAB: 3
};

class Building extends RotatedStandardBigPoint {
	type = 0;
	stationCode = "";

	constructor(axPos = 0, ayPos = 0, aType = 0, astationCode = "") {
		super(axPos, ayPos);
		this.type = aType;
		this.stationCode = astationCode;
		this.updateFromType(); //xsize, ysize
	}

	//sizes of building

	updateFromType() {
		switch(this.type) {
			case(buildingType.NORMAL):
			this.xsize = 20;
			this.ysize = 19;
			break;
			case(buildingType.CORNER):
			this.xsize = 24;
			this.ysize = 24;
			break;
			case(buildingType.FAMILY):
			this.xsize = 20;
			this.ysize = 12;
			break;
			case(buildingType.PREFAB):
			this.xsize = 25;
			this.ysize = 15;
			break;
			default:
				console.error("Type invalid!");
				this.xsize = 0;
				this.ysize = 0;
				break;
		}

		this.xsize *= UNITS_PER_METER;
		this.ysize *= UNITS_PER_METER;
	}

	draw(aStyle = "#800000") {
		abstractDrawBigPoint(aStyle, this, () => {
			canvasData.context.fillRect(0, 0, this.xsize, this.ysize);

			canvasData.context.strokeStyle = SELECT_COLOR;
			canvasData.context.beginPath();
			canvasData.context.moveTo(0, this.ysize-LINE_WIDTH/2);
			canvasData.context.lineTo(this.xsize, this.ysize-LINE_WIDTH/2);
			canvasData.context.stroke();
	
			if(this.type === buildingType.CORNER) {
				//mark second front edge
				canvasData.context.beginPath();
				canvasData.context.moveTo(this.xsize-LINE_WIDTH/2, 0);
				canvasData.context.lineTo(this.xsize-LINE_WIDTH/2, this.ysize);
				canvasData.context.stroke();
			}
		});
	}

	collision(aX, aY) {
		return (aX >= this.xPos-this.xsize/2) &&
			(aX <= this.xPos+this.xsize/2) &&
			(aY >= this.yPos-this.ysize/2) &&
			(aY <= this.yPos+this.ysize/2);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xPos-this.xsize/2,
			this.yPos-this.ysize/2,
			this.xsize, this.ysize
		);	
	}
};

let buildingList = [];

let buildingTypeSelector;

function makeBuildingTypeSelector() {
	buildingTypeSelector = new DocumentFragment();

	let label = document.createElement("label");
	label.textContent = "Select building type";

	let select = document.createElement("select");
	select.name = "buildtypeinput";
	select.id = "buildtypeinput";

	for(let b in buildingType) {
		let option = document.createElement("option");
		option.textContent = b;
		option.setAttribute("value", buildingType[b]);
		select.appendChild(option);
	}

	buildingTypeSelector.appendChild(label)
	buildingTypeSelector.appendChild(select);
	buildingTypeSelector.appendChild(document.createElement("br"));
}

function buildingSelectMenu(aX, aY) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Select building type at pos x = "+aX+", y = "+aY));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("buildxinput", aX);
	addHiddenInput("buildyinput", aY);

	canvasData.edit.appendChild(document.createTextNode("Station code:"));

	addInputPlaceholder("buildcodeinput", "", "text", "XXXX");

	let dfcopy = buildingTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);

	let makeButton = document.createElement("button");
	makeButton.textContent = "Make building";
	makeButton.addEventListener("click", buildingMake);
	canvasData.edit.appendChild(makeButton);
}

function buildingMake() {
	let x = Number(document.getElementById("buildxinput").value);
	let y = Number(document.getElementById("buildyinput").value);
	let type = Number(document.getElementById("buildtypeinput").value);
	let stationCode = document.getElementById("buildcodeinput").value;

	buildingList.push(new Building(x, y, type, stationCode));
	buildingList.at(-1).draw();

	canvasData.edit.replaceChildren();//clear AFTER getting values
}

function buildingEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing building "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(buildingList[aID]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));
	addInput("editrotinput", buildingList[aID].rotation, "text");

	let dfcopy = buildingTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);

	let updateButton = document.createElement("button");
	updateButton.appendChild(document.createTextNode("Update"));
	updateButton.addEventListener("click", () => {
		let buildId = getIDFromInput();
		getDataFromBasicInputs(buildingList[buildId]);
		buildingList[buildId].rotation = Number(document.getElementById("editrotinput").value);
		buildingList[buildId].type = Number(document.getElementById("buildtypeinput").value);
		buildingList[buildId].updateFromType();
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);
	
	let removeButton = document.createElement("button");
	removeButton.appendChild(document.createTextNode("Remove"));
	removeButton.addEventListener("click", () => {
		removeFromListById(buildingList);
	});
	canvasData.edit.appendChild(removeButton);
}