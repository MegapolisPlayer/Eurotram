//set x,y size, orientation, code for simulator

//LANDMARK CODES
//OLHB - Olsany cemetery
//ATRF - Atrium Flora
//JZPC - church at JzP square
//NTM1 - National Museum (old building)
//NTM2 - National museum (new building)
//NMMC - church at Namesti Miru
//VRSC - residence at Vrsovice square
//CCHC - church at Cechovo namesti
//ZZTV - Zizkov TV tower

function makeLandmarkEntry(acode, aname, axsize, aysize) {
	this.code = acode;
	this.name = aname;
	this.xsize = axsize;
	this.ysize = aysize;
}

const landmarkSizes = [
	new makeLandmarkEntry("OLHB", "Olsany cemetery", 150, 15),
	new makeLandmarkEntry("ATRF", "Atrium Flora", 0, 0),
	new makeLandmarkEntry("JZPC", "JzP Church", 30, 65),
	new makeLandmarkEntry("NTM1", "National Museum (old)", 0, 0),
	new makeLandmarkEntry("NTM2", "National Museum (new)", 0, 0),
	new makeLandmarkEntry("NMMC", "Namesti Miru church", 0, 0),
	new makeLandmarkEntry("VRSC", "Vrsovice square residence", 0, 0),
	new makeLandmarkEntry("CCHC", "Cechovo namesti church", 0, 0),
	new makeLandmarkEntry("ZZTV", "Zizkov TV Tower", 0, 0),
];

let landmarkTypeSelector;

function makeLandmarkSelector() {
	landmarkTypeSelector = new DocumentFragment();

	let label = document.createElement("label");
	label.setAttribute("for", "landmarktypeinput");
	label.textContent = "Select landmark type: ";

	let select = document.createElement("select");
	select.id = "landmarktypeinput";
	select.name = "landmarktypeinput";

	landmarkSizes.forEach((v) => {
		let option = document.createElement("option");
		option.setAttribute("value", v.code);
		option.textContent = v.name;
		select.appendChild(option);
	});

	landmarkTypeSelector.appendChild(label);
	landmarkTypeSelector.appendChild(document.createElement("br"));
	landmarkTypeSelector.appendChild(select);
	landmarkTypeSelector.appendChild(document.createElement("br"));
}

class Landmark {
	xPos = 0;
	yPos = 0;
	xsize = 0;
	ysize = 0;
	height = 0;
	rotation = 0;
	//no station code - always rendered
	landmarkCode = 0;

	constructor(axPos = 0, ayPos = 0, acode = 0) {
		this.xPos = axPos;
		this.yPos = ayPos;
		this.landmarkCode = acode;
		this.updateFromLandmarkCode();
	}

	updateFromLandmarkCode() {
		for(let i = 0; i < landmarkSizes.length; i++) {
			if(landmarkSizes[i].code === this.landmarkCode) {
				this.xsize = landmarkSizes[i].xsize*UNITS_PER_METER;
				this.ysize = landmarkSizes[i].ysize*UNITS_PER_METER;
				break;
			}
		}
	}

	draw(aStyle = "#ff0000") {
		if(!this.willRender()) { return; }

		canvasData.context.translate(this.xPos, this.yPos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-this.xsize/2,-this.ysize/2);

		canvasData.context.fillStyle = aStyle;
		canvasData.context.fillRect(0, 0, this.xsize, this.ysize);

		canvasData.context.strokeStyle = SELECT_COLOR;
		canvasData.context.beginPath();
		canvasData.context.moveTo(0, this.ysize-LINE_WIDTH/2);
		canvasData.context.lineTo(this.xsize, this.ysize-LINE_WIDTH/2);
		canvasData.context.stroke();

		//reset back
		canvasData.context.translate(this.xsize/2,this.ysize/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(-this.xPos, -this.yPos);
	}

	collision(aX, aY) {
		return (aX >= this.xPos-this.xsize/2) &&
			(aX <= this.xPos+this.xsize/2) &&
			(aY >= this.yPos-this.ysize/2) &&
			(aY <= this.yPos+this.ysize/2);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xPos - this.xsize/2,
			this.yPos - this.ysize/2, 
			this.xsize, this.ysize)
	}
};

let landmarkList = [];

function landmarkSelectMenu(aX, aY) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Select landmark at pos x = "+aX+", y = "+aY));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenInput("landmarkxinput", aX);
	addHiddenInput("landmarkyinput", aY);

	let dfcopy = landmarkTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);

	let make = document.createElement("button");
	make.textContent = "Add landmark";
	make.addEventListener("click", () => {
		let x = Number(document.getElementById("landmarkxinput").value);
		let y = Number(document.getElementById("landmarkyinput").value);
		let landmarkCode = document.getElementById("landmarktypeinput").value;
		landmarkList.push(new Landmark(x, y, landmarkCode));
		landmarkList.at(-1).draw();
		canvasData.edit.replaceChildren();
	});
	canvasData.edit.appendChild(make);
}

function landmarkEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing landmark "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	canvasData.edit.appendChild(document.createTextNode("X: "));
	addInput("editxinput", landmarkList[aID].xPos, "number");

	canvasData.edit.appendChild(document.createTextNode("Y: "));
	addInput("edityinput", landmarkList[aID].yPos, "number");

	canvasData.edit.appendChild(document.createTextNode("Height: "));
	addInput("editheightinput", landmarkList[aID].height, "number");

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));
	addInput("editrotinput", landmarkList[aID].rotation, "text");

	let dfcopy = landmarkTypeSelector.cloneNode(true);
	canvasData.edit.append(dfcopy);
	
	canvasData.edit.appendChild(document.createElement("br"));

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let landmarkId = getIDFromInput();
		landmarkList[landmarkId].xPos = Number(document.getElementById("editxinput").value);
		landmarkList[landmarkId].yPos = Number(document.getElementById("edityinput").value);
		landmarkList[landmarkId].height = Number(document.getElementById("editheightinput").value);
		landmarkList[landmarkId].rotation = Number(document.getElementById("editrotinput").value);
		landmarkList[landmarkId].landmarkCode = document.getElementById("landmarktypeinput").value;
		landmarkList[landmarkId].updateFromLandmarkCode();
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(landmarkList);
	});
	canvasData.edit.appendChild(removeButton);
}