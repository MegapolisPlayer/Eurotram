//TODO rewrite texparcels

//TODO rewrite serialization of this, add material selection
//every texparcel has material - texcoords dynamically in sim
//assign material to each parcel

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
	canvasData.edit.innerHTML = "Setup texture parcel x = "+ax+", y = "+ay+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='tpxinput' value="+ax+"><br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='tpyinput' value="+ay+"><br>";

	canvasData.edit.innerHTML += "X Size:<input type='number' id='tpwinput' name='tpwinput' value='200'><br>";
	canvasData.edit.innerHTML += "Y Size:<input type='number' id='tphinput' name='tphinput' value='100'><br>";

	canvasData.edit.innerHTML += "Station code 1:<input type='text' id='tps1input' name='tps1input' placeholder='XXXX'><br>";
	canvasData.edit.innerHTML += "Station code 2:<input type='text' id='tps2input' name='tps2input' placeholder='XXXX'><br>";

	canvasData.edit.innerHTML += "Material:<input type='text' id='tpmatinput' name='tpmatinput' placeholder='Name'><br>";

	canvasData.edit.innerHTML += "<button onclick='texparcelMake()'>Add texparcel</button>";
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

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function texparcelEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing texture parcel "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X1:<input type='number' id='editx1input' name='editx1input' value="+texparcelList[aid].x1+"><br>";
	canvasData.edit.innerHTML += "Y1:<input type='number' id='edity1input' name='edity1input' value="+texparcelList[aid].y1+"><br>";
	canvasData.edit.innerHTML += "H1:<input type='number' id='edith1input' name='edith1input' value="+texparcelList[aid].h1+"><br>";

	canvasData.edit.innerHTML += "X2:<input type='number' id='editx2input' name='editx2input' value="+texparcelList[aid].x2+"><br>";
	canvasData.edit.innerHTML += "Y2:<input type='number' id='edity2input' name='edity2input' value="+texparcelList[aid].y2+"><br>";
	canvasData.edit.innerHTML += "H2:<input type='number' id='edith2input' name='edith2input' value="+texparcelList[aid].h2+"><br>";

	canvasData.edit.innerHTML += "X3:<input type='number' id='editx3input' name='editx3input' value="+texparcelList[aid].x3+"><br>";
	canvasData.edit.innerHTML += "Y3:<input type='number' id='edity3input' name='edity3input' value="+texparcelList[aid].y3+"><br>";
	canvasData.edit.innerHTML += "H3:<input type='number' id='edith3input' name='edith3input' value="+texparcelList[aid].h3+"><br>";

	canvasData.edit.innerHTML += "X4:<input type='number' id='editx4input' name='editx4input' value="+texparcelList[aid].x4+"><br>";
	canvasData.edit.innerHTML += "Y4:<input type='number' id='edity4input' name='edity4input' value="+texparcelList[aid].y4+"><br>";
	canvasData.edit.innerHTML += "H4:<input type='number' id='edith4input' name='edith4input' value="+texparcelList[aid].h4+"><br>";

	canvasData.edit.innerHTML += "Station code 1:<input type='text' id='editsc1input' name='editsc1input' placeholder='XXXX' value="+texparcelList[aid].stationCode1+"><br>";
	canvasData.edit.innerHTML += "Station code 2:<input type='text' id='editsc2input' name='editsc2input' placeholder='XXXX' value="+texparcelList[aid].stationCode2+"><br>";
	canvasData.edit.innerHTML += "Material:<input type='text' id='editmatinput' name='editmatinput' placeholder='Name' value="+texparcelList[aid].materialName+"><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='texparcelUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='texparcelRemove()'>Remove</button>";
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
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}