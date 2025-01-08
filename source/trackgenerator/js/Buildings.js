let buildingType = {
	NORMAL: 0,
	CORNER: 1,
	FAMILY: 2,
	PREFAB: 3
};

class Building {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;
	xsize = 0;
	ysize = 0;
	type = 0;
	stationCode = "";

	constructor(axpos = 0, aypos = 0, atype = 0, astationCode = "") {
		this.xpos = axpos;
		this.ypos = aypos;
		this.type = atype;
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

	draw(style = "#ff0000") {
		if(!this.willRender()) { return; }
		console.log("building draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-this.xsize/2,-this.ysize/2);

		canvasData.context.fillStyle = style;
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

		//reset back
		canvasData.context.translate(this.xsize/2,this.ysize/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(-this.xpos, -this.ypos);
	}

	collision(ax, ay) {
		return (ax >= this.xpos-this.xsize/2) &&
			(ax <= this.xpos+this.xsize/2) &&
			(ay >= this.ypos-this.ysize/2) &&
			(ay <= this.ypos+this.ysize/2);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xpos-this.xsize/2,
			this.ypos-this.ysize/2,
			this.xsize, this.ysize
		);	
	}
};

let buildingList = [];

let buildingTypeSelector;

function makeBuildingTypeSelector() {
	buildingTypeSelector = document.createElement("span");

	let label = document.createElement("label");
	label.for = "Select building type";

	let select = document.createElement("select");
	select.name = "buildtypeinput";
	select.id = select.name;

	for(let b in buildingType) {
		let option = document.createElement("option");
		option.setAttribute("value", buildingType[b]); //b is name of property, buildingType[b] is its value
		option.appendChild(document.createTextNode(b));
		select.appendChild(option);
	}

	buildingTypeSelector.appendChild(label)
	buildingTypeSelector.appendChild(select);
	buildingTypeSelector.appendChild(document.createElement("br"));
}

function buildingSelectMenu(ax, ay) {
	canvasData.edit.replaceChildren([]);

	canvasData.edit.appendChild(document.createTextNode("Select building type at pos x = "+ax+", y = "+ay));
	canvasData.edit.appendChild(document.createElement("br"));

	let buildxinput = document.createElement("input");
	buildxinput.type = "hidden";
	buildxinput.id = "buildxinput";
	buildxinput.setAttribute("value", ax);
	let buildyinput = document.createElement("input");
	buildyinput.type = "hidden";
	buildyinput.id = "buildyinput";
	buildyinput.setAttribute("value", ay);

	canvasData.edit.appendChild(buildxinput);
	canvasData.edit.appendChild(buildyinput);
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("Station code:"));

	let buildcodeinput = document.createElement("input");
	buildcodeinput.id = "buildcodeinput";
	buildcodeinput.name = buildcodeinput.id;
	buildcodeinput.placeholder = "XXXX";
	buildcodeinput.maxLength = 4;
	buildcodeinput.size = 4;

	canvasData.edit.appendChild(buildcodeinput);
	canvasData.edit.appendChild(document.createElement("br"));	

	canvasData.edit.appendChild(buildingTypeSelector);

	let makeButton = document.createElement("button");
	makeButton.appendChild(document.createTextNode("Make building"));
	makeButton.addEventListener("click", (e) => {
		buildingMake();
	});
	canvasData.edit.appendChild(makeButton);
}

function buildingMake() {
	let x = Number(document.getElementById("buildxinput").value);
	let y = Number(document.getElementById("buildyinput").value);
	let type = Number(document.getElementById("buildtypeinput").value);
	console.log(type, " and ", document.getElementById("buildtypeinput").value);
	let stationCode = document.getElementById("buildcodeinput").value;

	buildingList.push(new Building(x, y, type, stationCode));
	buildingList.at(-1).draw();

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function buildingEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing buidling "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	addBasicEditInputs(buildingList[aid]);
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+buildingList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += buildingTypeSelector;

	canvasData.edit.innerHTML += "<button type='' onclick='buildingUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingRemove()'>Remove building</button>";
}

function buildingUpdate() {
	console.log("Updating building...");

	let buildId = Number(document.getElementById("idinput").value);
	getDataFromBasicInputs(buildingList[buildId]);
	buildingList[buildId].rotation = Number(document.getElementById("editrotinput").value);
	buildingList[buildId].type = Number(document.getElementById("buildtypeinput").value);
	buildingList[buildId].updateFromType();

	canvasRedraw();
}

function buildingRemove() {
	console.log("Removing building...");

	let buildId =  Number(document.getElementById("idinput").value);
	buildingList.splice(buildId, 1);
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}