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
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML = "Setup wall near x = "+ax+", y = "+ay+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='wxinput' value="+ax+"><br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='wyinput' value="+ay+"><br>";

	canvasData.edit.innerHTML += "X1:<input type='number' id='wx1input' name='wx1input' value="+(ax-200)+"><br>";
	canvasData.edit.innerHTML += "Y1:<input type='number' id='wy1input' name='wy1input' value="+ay+"><br>";
	canvasData.edit.innerHTML += "H1:<input type='number' id='wh1input' name='wh1input' value='0'><br>";

	canvasData.edit.innerHTML += "X2:<input type='number' id='wx2input' name='wx2input' value="+(ax+200)+"><br>";
	canvasData.edit.innerHTML += "Y2:<input type='number' id='wy2input' name='wy2input' value="+ay+"><br>";
	canvasData.edit.innerHTML += "H2:<input type='number' id='wh2input' name='wh2input' value='0'><br>";

	canvasData.edit.innerHTML += "Height:<input type='number' id='whinput' name='whinput' value='3'><br>";

	canvasData.edit.innerHTML += "Station code:<input type='text' id='wsinput' name='wsinput' placeholder='XXXX'><br>";
	canvasData.edit.innerHTML += "Material name:<input type='text' id='wminput' name='wminput' placeholder='Name'><br>";

	canvasData.edit.innerHTML += "<button onclick='wallMake()'>Add wall</button>";
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

	canvasData.edit.innerHTML = "";
}

function wallEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing wall "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X1:<input type='number' id='editx1input' name='editx1input' value="+wallList[aid].x1+"><br>";
	canvasData.edit.innerHTML += "Y1:<input type='number' id='edity1input' name='edity1input' value="+wallList[aid].y1+"><br>";
	canvasData.edit.innerHTML += "H1:<input type='number' id='edith1input' name='edith1input' value="+wallList[aid].h1+"><br>";

	canvasData.edit.innerHTML += "X2:<input type='number' id='editx2input' name='editx2input' value="+wallList[aid].x2+"><br>";
	canvasData.edit.innerHTML += "Y2:<input type='number' id='edity2input' name='edity2input' value="+wallList[aid].y2+"><br>";
	canvasData.edit.innerHTML += "H2:<input type='number' id='edith2input' name='edith2input' value="+wallList[aid].h2+"><br>";

	canvasData.edit.innerHTML += "Height:<input type='number' id='edithinput' name='edithinput' value="+wallList[aid].height+"><br>";

	canvasData.edit.innerHTML += "Station code:<input type='text' id='editsinput' name='editsinput' placeholder='XXXX'><br>";
	canvasData.edit.innerHTML += "Material name:<input type='text' id='editminput' name='editminput' placeholder='Name'><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='wallUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='wallRemove()'>Remove</button>";
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
	wallList[wallId].materialName = document.getElementById("editminput").value;
	
	canvasRedraw();
}

function wallRemove() {
	console.log("Removing wall...");

	let wallId =  Number(document.getElementById("idinput").value);
	wallList.splice(wallId, 1);
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}