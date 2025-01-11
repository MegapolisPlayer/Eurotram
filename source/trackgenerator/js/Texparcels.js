class Texparcel {
	//4 points

	x1 = 0;
	y1 = 0;
	h1 = 0;
	x2 = 0;
	y2 = 0;
	h2 = 0;
	x3 = 0;
	y3 = 0;
	h3 = 0;
	x4 = 0;
	y4 = 0;
	h4 = 0;

	rotation = 0;

	//can be in 2 stations since can be on track between the two
	stationCode1 = "";
	stationCode2 = "";

	materialName = "";
	
	constructor(axpos = 0, aypos = 0, axsize = 0, aysize = 0, astationCode1 = "", astationCode2 = "", amaterialName = "") {
		this.x1 = axpos-axsize/2;
		this.y1 = aypos-aysize/2;

		this.x2 = axpos+axsize/2;
		this.y2 = aypos-aysize/2;

		this.x3 = axpos+axsize/2;
		this.y3 = aypos+aysize/2;

		this.x4 = axpos-axsize/2;
		this.y4 = aypos+aysize/2;

		this.stationCode1 = astationCode1;
		this.stationCode2 = astationCode2;
		this.materialName = amaterialName;
	}

	draw() {
		if(!this.willRender()) { return; }
		console.log("texparcel draw");

		canvasData.context.globalAlpha = 0.25;
		
		canvasData.context.fillStyle = "#aa0000";
		canvasData.context.strokeStyle = "#000000";

		canvasData.context.beginPath();
		canvasData.context.moveTo(this.x1, this.y1);
		canvasData.context.lineTo(this.x2, this.y2);
		canvasData.context.lineTo(this.x3, this.y3);
		canvasData.context.lineTo(this.x4, this.y4);
		canvasData.context.lineTo(this.x1, this.y1);
		canvasData.context.stroke();

		canvasData.context.beginPath();
		canvasData.context.moveTo(this.x1, this.y1);
		canvasData.context.lineTo(this.x2, this.y2);
		canvasData.context.lineTo(this.x3, this.y3);
		canvasData.context.lineTo(this.x4, this.y4);
		canvasData.context.lineTo(this.x1, this.y1);
		canvasData.context.fill();

		let avgX = (this.x1 + this.x2 + this.x3 + this.x4) / 4;
		let avgY = (this.y1 + this.y2 + this.y3 + this.y4) / 4;

		//draw select "gizmo"
		canvasData.context.fillStyle = SELECT_COLOR;
		canvasData.context.beginPath();
		canvasData.context.arc(
			avgX, avgY, NODE_SIZE, 0, 2*Math.PI
		);
		canvasData.context.fill();

		canvasData.context.globalAlpha = 1;
	}

	collision(ax, ay) {
		//check collision with small point in middle		

		let avgX = (this.x1 + this.x2 + this.x3 + this.x4) / 4;
		let avgY = (this.y1 + this.y2 + this.y3 + this.y4) / 4;

		return (ax >= avgX-NODE_SIZE) &&
			(ax <= avgX+NODE_SIZE) &&
			(ay >= avgY-NODE_SIZE) &&
			(ay <= avgY+NODE_SIZE);
	}

	willRender() {
		//check each point

		return canvasIsInFrustum(this.x1, this.y1, 0, 0) ||
		canvasIsInFrustum(this.x2, this.y2, 0, 0) || 
		canvasIsInFrustum(this.x3, this.y3, 0, 0) || 
		canvasIsInFrustum(this.x4, this.y4, 0, 0);
	}
};

let texparcelList = [];

function renderTextureParcels() {
	if(canvasData.hideTPElem.checked) { return; } //if hidden
	texparcelList.forEach((v) => {
		v.draw();
	});
}

function texparcelAddMenu(ax, ay) {
	canvasData.edit.appendChild(document.createTextNode("Setup texture parcel x = "+ax+", y = "+ay));
	canvasData.edit.appendChild(document.createElement("br"));

	addInput("tpxinput", ax, "hidden");
	addInput("tpyinput", ay, "hidden");

	canvasData.edit.appendChild(document.createTextNode("X Size: "));
	addInput("tpwinput", 200, "number");
	canvasData.edit.appendChild(document.createTextNode("Y Size: "));
	addInput("tphinput", 100, "number");

	canvasData.edit.appendChild(document.createTextNode("Station code 1: "));
	canvasData.edit.appendChild(document.createElement("br"));
	addInputPlaceholder("tps1input", "", "text", "XXXX");

	canvasData.edit.appendChild(document.createTextNode("Station code 2: "));
	canvasData.edit.appendChild(document.createElement("br"));
	addInputPlaceholder("tps2input", "", "text", "XXXX");

	canvasData.edit.appendChild(document.createTextNode("Material: "));
	canvasData.edit.appendChild(document.createElement("br"));
	addInputPlaceholder("tpmatinput", "", "text", "Name of material");

	let makeButton = document.createElement("button");
	makeButton.textContent = "Add texparcel";
	makeButton.addEventListener("click", texparcelMake);
	canvasData.edit.appendChild(makeButton);
}

function texparcelMake() {
	let x = Number(document.getElementById("tpxinput").value);
	let y = Number(document.getElementById("tpyinput").value);
	let sx = Number(document.getElementById("tpwinput").value);
	let sy = Number(document.getElementById("tphinput").value);
	let stationCode1 = document.getElementById("tps1input").value;
	let stationCode2 = document.getElementById("tps2input").value;
	let materialName = document.getElementById("tpmatinput").value;

	texparcelList.push(new Texparcel(x, y, sx, sy, stationCode1, stationCode2, materialName));
	texparcelList.at(-1).draw();

	canvasData.edit.replaceChildren(); //clear AFTER getting values
}

function texparcelEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing texture parcel "+aid));
	canvasData.edit.appendChild(document.createElement("br"));
	addHiddenIdInput(aid);

	canvasData.edit.appendChild(document.createTextNode("X1: "));
	addInput("editx1input", texparcelList[aid].x1, "number");
	canvasData.edit.appendChild(document.createTextNode("Y1: "));
	addInput("edity1input", texparcelList[aid].y1, "number");
	canvasData.edit.appendChild(document.createTextNode("H1: "));
	addInput("edith1input", texparcelList[aid].h1, "number");
	canvasData.edit.appendChild(document.createTextNode("X2: "));
	addInput("editx2input", texparcelList[aid].x2, "number");
	canvasData.edit.appendChild(document.createTextNode("Y2: "));
	addInput("edity2input", texparcelList[aid].y2, "number");
	canvasData.edit.appendChild(document.createTextNode("H2: "));
	addInput("edith2input", texparcelList[aid].h2, "number");
	canvasData.edit.appendChild(document.createTextNode("X3: "));
	addInput("editx3input", texparcelList[aid].x3, "number");
	canvasData.edit.appendChild(document.createTextNode("Y3: "));
	addInput("edity3input", texparcelList[aid].y3, "number");
	canvasData.edit.appendChild(document.createTextNode("H3: "));
	addInput("edith3input", texparcelList[aid].h3, "number");
	canvasData.edit.appendChild(document.createTextNode("X4: "));
	addInput("editx4input", texparcelList[aid].x4, "number");
	canvasData.edit.appendChild(document.createTextNode("Y4: "));
	addInput("edity4input", texparcelList[aid].y4, "number");
	canvasData.edit.appendChild(document.createTextNode("H4: "));
	addInput("edith4input", texparcelList[aid].h4, "number");

	canvasData.edit.appendChild(document.createTextNode("Station code 1: "));
	addInputPlaceholder("editsc1input", texparcelList[aid].stationCode1, "text", "XXXX");
	canvasData.edit.appendChild(document.createTextNode("Station code 2: "));
	addInputPlaceholder("editsc2input", texparcelList[aid].stationCode2, "text", "XXXX");
	canvasData.edit.appendChild(document.createTextNode("Material name: "));
	addInputPlaceholder("editmatinput", texparcelList[aid].materialNames, "text", "Name of material");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", texparcelUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", texparcelRemove);
	canvasData.edit.appendChild(removeButton);
}
function texparcelUpdate() {
	console.log("Updating texparcel...");

	let tpId =  Number(document.getElementById("idinput").value);

	texparcelList[tpId].x1 = Number(document.getElementById("editx1input").value);
	texparcelList[tpId].x2 = Number(document.getElementById("editx2input").value);
	texparcelList[tpId].x3 = Number(document.getElementById("editx3input").value);
	texparcelList[tpId].x4 = Number(document.getElementById("editx4input").value);
	texparcelList[tpId].y1 = Number(document.getElementById("edity1input").value);
	texparcelList[tpId].y2 = Number(document.getElementById("edity2input").value);
	texparcelList[tpId].y3 = Number(document.getElementById("edity3input").value);
	texparcelList[tpId].y4 = Number(document.getElementById("edity4input").value);
	texparcelList[tpId].h1 = Number(document.getElementById("edith1input").value);
	texparcelList[tpId].h2 = Number(document.getElementById("edith2input").value);
	texparcelList[tpId].h3 = Number(document.getElementById("edith3input").value);
	texparcelList[tpId].h4 = Number(document.getElementById("edith4input").value);

	texparcelList[tpId].stationCode1 = document.getElementById("editsc1input").value;
	texparcelList[tpId].stationCode2 = document.getElementById("editsc2input").value;
	texparcelList[tpId].materialName = document.getElementById("editmatinput").value;

	canvasRedraw();
}
function texparcelRemove() {
	console.log("Removing texparcel...");

	let tpId =  Number(document.getElementById("idinput").value);
	texparcelList.splice(tpId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}