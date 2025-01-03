//when saving - set different depth to each to avoid z-fighting
//add button - "download texture preps" - saves into zip file with station name

class Texparcel {
	xpos = 0; //original top left corner
	ypos = 0;
	xsize = 0;
	ysize = 0;
	rotation = 0;

	heightPointTL = 0; //top-left
	heightPointTR = 0; //top-right
	heightPointBL = 0; //bottom-left
	heightPointBR = 0; //bottom-right

	//can be in 2 stations since can be on track between the two
	stationCode1 = "";
	stationCode2 = "";

	depthOffset = 0.0; //to prevent z-fighting
	
	constructor(axpos = 0, aypos = 0, axsize = 0, aysize = 0, arotation = 0, astationCode1 = "", astationCode2 = "") {
		this.xpos = axpos;
		this.ypos = aypos;
		this.xsize = axsize;
		this.ysize = aysize;
		this.rotation = arotation;
		this.stationCode1 = astationCode1;
		this.stationCode2 = astationCode2;
		this.depthOffset = 0.0;
	}

	draw() {
		if(!this.willRender()) { return; }
		console.log("texparcel draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-this.xsize/2,-this.ysize/2);

		canvasData.context.globalAlpha = 0.25;
		
		canvasData.context.fillStyle = "#aa0000";
		canvasData.context.strokeStyle = "#000000";

		canvasData.context.fillRect(0, 0, this.xsize, this.ysize);
		canvasData.context.strokeRect(0, 0, this.xsize, this.ysize);

		//draw select "gizmo"
		canvasData.context.fillStyle = SELECT_COLOR;
		canvasData.context.beginPath();
		canvasData.context.arc(
			this.xsize/2,
			this.ysize/2,
			NODE_SIZE, 0, 2*Math.PI
		);
		canvasData.context.fill();

		canvasData.context.globalAlpha = 1;

		canvasData.context.translate(this.xsize/2,this.ysize/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(-this.xpos, -this.ypos);
	}

	collision(ax, ay) {
		//check collision with small point in middle		

		let midPosX = this.xpos;
		let midPosY = this.ypos;

		return (ax >= midPosX-NODE_SIZE) &&
			(ax <= midPosX+NODE_SIZE) &&
			(ay >= midPosY-NODE_SIZE) &&
			(ay <= midPosY+NODE_SIZE);
	}

	willRender() {
		//we dont check precise borders, but top left and bottom right corner

		return canvasIsInFrustum(
			this.xpos + this.xsize/2,
			this.ypos + this.ysize/2,
			NODE_SIZE*2, NODE_SIZE*2
		) || canvasIsInFrustum(
			this.xpos - this.xsize/2,
			this.ypos - this.ysize/2,
			NODE_SIZE*2, NODE_SIZE*2
		);
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
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='tprinput' name='tprinput' value='0'><br>";

	canvasData.edit.innerHTML += "Station code 1:<input type='text' id='tps1input' name='tps1input' placeholder='XXXX'><br>";
	canvasData.edit.innerHTML += "Station code 2:<input type='text' id='tps2input' name='tps2input' placeholder='XXXX'><br>";

	canvasData.edit.innerHTML += "<button onclick='texparcelMake()'>Add texparcel</button>";
}
function texparcelMake() {
	let x = Number(document.getElementById("tpxinput").value);
	let y = Number(document.getElementById("tpyinput").value);
	let sx = Number(document.getElementById("tpwinput").value);
	let sy = Number(document.getElementById("tphinput").value);
	let rotation = Number(document.getElementById("tprinput").value);
	let stationCode1 = Number(document.getElementById("tps1input").value);
	let stationCode2 = Number(document.getElementById("tps2input").value);

	texparcelList.push(new Texparcel(x, y, sx, sy, rotation, stationCode1, stationCode2));
	texparcelList.at(-1).draw();

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function texparcelEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing texture parcel "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+texparcelList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+texparcelList[aid].ypos+"><br>";
	
	canvasData.edit.innerHTML += "X size:<input type='number' id='editwinput' name='editwinput' value="+texparcelList[aid].xsize+"><br>";
	canvasData.edit.innerHTML += "Y size:<input type='number' id='edithinput' name='edithinput' value="+texparcelList[aid].ysize+"><br>";

	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+texparcelList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += "Height TL:<input type='number' id='edittlinput' value="+texparcelList[aid].heightPointTL+"><br>";
	canvasData.edit.innerHTML += "Height TR:<input type='number' id='edittrinput' value="+texparcelList[aid].heightPointTR+"><br>";
	canvasData.edit.innerHTML += "Height BL:<input type='number' id='editblinput' value="+texparcelList[aid].heightPointBL+"><br>";
	canvasData.edit.innerHTML += "Height BR:<input type='number' id='editbrinput' value="+texparcelList[aid].heightPointBR+"><br>";

	canvasData.edit.innerHTML += "Station code 1:<input type='text' id='editsc1input' name='editsc1input' placeholder='XXXX' value="+texparcelList[aid].stationCode1+"><br>";
	canvasData.edit.innerHTML += "Station code 2:<input type='text' id='editsc2input' name='editsc2input' placeholder='XXXX' value="+texparcelList[aid].stationCode2+"><br>";

	canvasData.edit.innerHTML += "Depth offset: <input type='number' id='editdinput' name='editdinput' min=-1 max=1 step=0.005 value='"+texparcelList[aid].depthOffset+"'><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='texparcelUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='texparcelRemove()'>Remove</button>";
}
function texparcelUpdate() {
	console.log("Updating texparcel...");

	let tpId =  Number(document.getElementById("idinput").value);

	texparcelList[tpId].xpos = Number(document.getElementById("editxinput").value);
	texparcelList[tpId].ypos = Number(document.getElementById("edityinput").value);
	texparcelList[tpId].xsize = Number(document.getElementById("editwinput").value);
	texparcelList[tpId].ysize = Number(document.getElementById("edithinput").value);
	texparcelList[tpId].rotation = Number(document.getElementById("editrotinput").value);
	
	texparcelList[tpId].heightPointTL = Number(document.getElementById("edittlinput").value);
	texparcelList[tpId].heightPointTR = Number(document.getElementById("edittrinput").value);
	texparcelList[tpId].heightPointBL = Number(document.getElementById("editblinput").value);
	texparcelList[tpId].heightPointBR = Number(document.getElementById("editbrinput").value);

	texparcelList[tpId].stationCode1 = document.getElementById("editsc1input").value;
	texparcelList[tpId].stationCode2 = document.getElementById("editsc2input").value;

	texparcelList[tpId].depthOffset = document.getElementById("editdinput").value;

	canvasRedraw();
}
function texparcelRemove() {
	console.log("Removing texparcel...");

	let tpId =  Number(document.getElementById("idinput").value);
	texparcelList.splice(tpId, 1);
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}