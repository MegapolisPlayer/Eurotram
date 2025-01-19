class Wall {
	x1 = 0;
	y1 = 0;
	h1 = 0;

	x2 = 0;
	y2 = 0;
	h2 = 0;
	
	height = 0;

	stationCode = "";
	materialName = "";

	constructor(
		ax1 = 0, ay1 = 0, ah1 = 0, ax2 = 0, ay2 = 0, ah2 = 0, 
		ah = 0, astationCode = 0, amaterialName = 0
	) {
		this.x1 = ax1;
		this.y1 = ay1;
		this.h1 = ah1;

		this.x2 = ax2;
		this.y2 = ay2;
		this.h2 = ah2;

		this.height = ah;

		this.stationCode = astationCode;
		this.materialName = amaterialName;
	}

	draw(aStyle = "#e0c7c7") {
		if(!this.willRender()) { return; }

		canvasData.context.strokeStyle = aStyle;

		canvasData.context.beginPath();
		canvasData.context.moveTo(this.x1, this.y1);
		canvasData.context.lineTo(this.x2, this.y2);
		canvasData.context.stroke();
	}

	collision(aX, aY) {
		//click on either node or in the middle

		let avgX = (this.x1 + this.x2) / 2;
		let avgY = (this.y1 + this.y2) / 2;

		return ((aX >= this.x1-NODE_SIZE) &&
			(aX <= this.x1+NODE_SIZE) &&
			(aY >= this.y1-NODE_SIZE) &&
			(aY <= this.y1+NODE_SIZE)) ||
			((aX >= this.x2-NODE_SIZE) &&
			(aX <= this.x2+NODE_SIZE) &&
			(aY >= this.y2-NODE_SIZE) &&
			(aY <= this.y2+NODE_SIZE)) ||
			((aX >= avgX-(NODE_SIZE*2)) &&
			(aX <= avgX+(NODE_SIZE*2)) &&
			(aY >= avgY-(NODE_SIZE*2)) &&
			(aY <= avgY+(NODE_SIZE*2)))
	}

	willRender() {
		//either node in frustum	
		return canvasIsInFrustum(
			this.x1 - NODE_SIZE/2,
			this.y1 - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE) ||
		canvasIsInFrustum(
			this.x2 - NODE_SIZE/2,
			this.y2 - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE);
	}
};

let wallList = [];

function wallAddMenu(aX, aY) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Setup wall near x = "+aX+", y = "+aY));
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("X1: "));
	addInput("wx1input", aX-200, "number");

	canvasData.edit.appendChild(document.createTextNode("Y1: "));
	addInput("wy1input", aY-100, "number");

	canvasData.edit.appendChild(document.createTextNode("H1: "));
	addInput("wh1input", 0, "number");

	canvasData.edit.appendChild(document.createTextNode("X2: "));
	addInput("wx2input", aX+200, "number");

	canvasData.edit.appendChild(document.createTextNode("Y2: "));
	addInput("wy2input", aX+100, "number");
	
	canvasData.edit.appendChild(document.createTextNode("H2: "));
	addInput("wh2input", 0, "number");

	canvasData.edit.appendChild(document.createTextNode("Height: "));
	addInput("whinput", 3, "number");

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	addInputPlaceholder("wsinput", "", "text", "XXXX");

	canvasData.edit.appendChild(document.createTextNode("Material name: "));
	addInputPlaceholder("wminput", "", "text", "Name of material");

	let makeButton = document.createElement("button");
	makeButton.textContent = "Make";
	makeButton.addEventListener("click", () => {
		let x1 = Number(document.getElementById("wx1input").value);
		let y1 = Number(document.getElementById("wy1input").value);
		let h1 = Number(document.getElementById("wh1input").value);

		let x2 = Number(document.getElementById("wx2input").value);
		let y2 = Number(document.getElementById("wy2input").value);
		let h2 = Number(document.getElementById("wh2input").value);

		let h = Number(document.getElementById("whinput").value);

		let stationCode = document.getElementById("wsinput").value;
		let materialName = document.getElementById("wminput").value;

		wallList.push(new Wall(x1, y1, h1, x2, y2, h2, h, stationCode, materialName));
		wallList.at(-1).draw();

		canvasData.edit.replaceChildren();
	});
	canvasData.edit.appendChild(makeButton);
}

function wallEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing wall "+aID));
	canvasData.edit.appendChild(document.createElement("br"));
	addHiddenIdInput(aID);

	canvasData.edit.appendChild(document.createTextNode("X1: "));
	addInput("editx1input", wallList[aID].x1, "number");

	canvasData.edit.appendChild(document.createTextNode("Y1: "));
	addInput("edity1input", wallList[aID].y1, "number");

	canvasData.edit.appendChild(document.createTextNode("H1: "));
	addInput("edith1input", wallList[aID].h1, "number");

	canvasData.edit.appendChild(document.createTextNode("X2: "));
	addInput("editx2input", wallList[aID].x2, "number");

	canvasData.edit.appendChild(document.createTextNode("Y2: "));
	addInput("edity2input", wallList[aID].y2, "number");
	
	canvasData.edit.appendChild(document.createTextNode("H2: "));
	addInput("edith2input", wallList[aID].h2, "number");

	canvasData.edit.appendChild(document.createTextNode("Height: "));
	addInput("edithinput", wallList[aID].height, "number");

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	addInputPlaceholder("editsinput", wallList[aID].stationCode, "text", "XXXX");

	canvasData.edit.appendChild(document.createTextNode("Material name: "));
	addInputPlaceholder("editminput", wallList[aID].materialName, "text", "Name of material");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let wallId =  getIDFromInput();
		wallList[wallId].x1 = Number(document.getElementById("editx1input").value);
		wallList[wallId].x2 = Number(document.getElementById("editx2input").value);
		wallList[wallId].y1 = Number(document.getElementById("edity1input").value);
		wallList[wallId].y2 = Number(document.getElementById("edity2input").value);
		wallList[wallId].h1 = Number(document.getElementById("edith1input").value);
		wallList[wallId].h2 = Number(document.getElementById("edith2input").value);
		wallList[wallId].height = Number(document.getElementById("edithinput").value);
		wallList[wallId].stationCode = document.getElementById("editsinput").value;
		wallList[wallId].materialName = document.getElementById("editminput").value;
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(wallList);
	});
	canvasData.edit.appendChild(removeButton);
}