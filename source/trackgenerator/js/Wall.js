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

	draw(style = "#e0c7c7") {
		if(!this.willRender()) { return; }

		canvasData.context.strokeStyle = style;

		canvasData.context.beginPath();
		canvasData.context.moveTo(this.x1, this.y1);
		canvasData.context.lineTo(this.x2, this.y2);
		canvasData.context.stroke();
	}

	collision(ax, ay) {
		//click on either node or in the middle

		let avgX = (this.x1 + this.x2) / 2;
		let avgY = (this.y1 + this.y2) / 2;

		return ((ax >= this.x1-NODE_SIZE) &&
			(ax <= this.x1+NODE_SIZE) &&
			(ay >= this.y1-NODE_SIZE) &&
			(ay <= this.y1+NODE_SIZE)) ||
			((ax >= this.x2-NODE_SIZE) &&
			(ax <= this.x2+NODE_SIZE) &&
			(ay >= this.y2-NODE_SIZE) &&
			(ay <= this.y2+NODE_SIZE)) ||
			((ax >= avgX-(NODE_SIZE*2)) &&
			(ax <= avgX+(NODE_SIZE*2)) &&
			(ay >= avgY-(NODE_SIZE*2)) &&
			(ay <= avgY+(NODE_SIZE*2)))
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

function wallAddMenu(ax, ay) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Setup wall near x = "+ax+", y = "+ay));
	canvasData.edit.appendChild(document.createElement("br"));

	canvasData.edit.appendChild(document.createTextNode("X1: "));
	addInput("wx1input", ax-200, "number");

	canvasData.edit.appendChild(document.createTextNode("Y1: "));
	addInput("wy1input", ay-100, "number");

	canvasData.edit.appendChild(document.createTextNode("H1: "));
	addInput("wh1input", 0, "number");

	canvasData.edit.appendChild(document.createTextNode("X2: "));
	addInput("wx2input", ax+200, "number");

	canvasData.edit.appendChild(document.createTextNode("Y2: "));
	addInput("wy2input", ax+100, "number");
	
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
	makeButton.addEventListener("click", wallMake);
	canvasData.edit.appendChild(makeButton);
}

function wallMake() {
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
}

function wallEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing wall "+aid));
	canvasData.edit.appendChild(document.createElement("br"));
	addHiddenIdInput(aid);

	canvasData.edit.appendChild(document.createTextNode("X1: "));
	addInput("editx1input", wallList[aid].x1, "number");

	canvasData.edit.appendChild(document.createTextNode("Y1: "));
	addInput("edity1input", wallList[aid].y1, "number");

	canvasData.edit.appendChild(document.createTextNode("H1: "));
	addInput("edith1input", wallList[aid].h1, "number");

	canvasData.edit.appendChild(document.createTextNode("X2: "));
	addInput("editx2input", wallList[aid].x2, "number");

	canvasData.edit.appendChild(document.createTextNode("Y2: "));
	addInput("edity2input", wallList[aid].y2, "number");
	
	canvasData.edit.appendChild(document.createTextNode("H2: "));
	addInput("edith2input", wallList[aid].h2, "number");

	canvasData.edit.appendChild(document.createTextNode("Height: "));
	addInput("edithinput", wallList[aid].height, "number");

	canvasData.edit.appendChild(document.createTextNode("Station code: "));
	addInputPlaceholder("editsinput", wallList[aid].stationCode, "text", "XXXX");

	canvasData.edit.appendChild(document.createTextNode("Material name: "));
	addInputPlaceholder("editminput", wallList[aid].materialName, "text", "Name of material");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", wallUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", wallRemove);
	canvasData.edit.appendChild(removeButton);
}

function wallUpdate() {
	console.log("Updating wall...");

	let wallId =  Number(document.getElementById("idinput").value);

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
}

function wallRemove() {
	console.log("Removing wall...");

	let wallId =  Number(document.getElementById("idinput").value);
	wallList.splice(wallId, 1);
	canvasData.edit.replaceChildren();


	canvasRedraw();
}