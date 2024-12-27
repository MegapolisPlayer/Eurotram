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

	constructor(axpos, aypos, atype, astationCode) {
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
				break;
		}

		this.xsize *= UNITS_PER_METER;
		this.ysize *= UNITS_PER_METER;
	}

	draw(style = "#ff0000") {
		if(!this.willRender()) { return; }
		console.log("building draw");

		canvasData.context.translate(
			this.xpos-this.xsize/2,
			this.ypos-this.ysize/2,
		);
		canvasData.context.rotate(toRadians(this.rotation));

		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(0, 0, this.xsize, this.ysize);

		canvasData.context.strokeStyle = "#ff00ff";
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
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(
			-this.xpos+this.xsize/2,
			-this.ypos+this.ysize/2,
		);

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

//cannot add to DOM as strings - completes element (adds second tag) and breaks stuff
const buildingTypeSelector = 
	"<label for='buildtypeinput'>Select building type:</label>"+
	"<select name='buildtypeinput' id='buildtypeinput'>"+
	"<option value="+buildingType.NORMAL+">Normal</option>"+
	"<option value="+buildingType.CORNER+">Corner</option>"+
	"<option value="+buildingType.FAMILY+">Family</option>"+
	"<option value="+buildingType.PREFAB+">Prefab</option>"+
	"</select><br>"

function buildingSelectMenu(ax, ay) {
	canvasData.edit.innerHTML = "Select building type at pos x = "+ax+", y = "+ay+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='buildxinput' value="+ax+"><br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='buildyinput' value="+ay+"><br>";

	canvasData.edit.innerHTML += "Station code:<input type='text' id='buildcodeinput' name='editcodeinput' placeholder='XXXX'><br>";

	canvasData.edit.innerHTML += buildingTypeSelector;

	canvasData.edit.innerHTML += "<button onclick='buildingMake()'>Make building</button>";
}

function buildingMake() {
	let x = Number(document.getElementById("buildxinput").value);
	let y = Number(document.getElementById("buildyinput").value);
	let type = Number(document.getElementById("buildtypeinput").value);
	let stationCode = Number(document.getElementById("buildcodeinput").value);

	buildingList.push(new Building(x, y, type, stationCode));
	buildingList.at(-1).draw();

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function buildingEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing buidling "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+buildingList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+buildingList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+buildingList[aid].height+"><br>";
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+buildingList[aid].rotation+"><br>";
	canvasData.edit.innerHTML += "Station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+buildingList[aid].stationCode+"><br>";

	canvasData.edit.innerHTML += buildingTypeSelector;

	canvasData.edit.innerHTML += "<button type='' onclick='buildingUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingRemove()'>Remove buidling</button>";
}

function buildingUpdate() {
	console.log("Updating building..");

	let buildId = Number(document.getElementById("idinput").value);
	buildingList[buildId].xpos = Number(document.getElementById("editxinput").value);
	buildingList[buildId].ypos = Number(document.getElementById("edityinput").value);
	buildingList[buildId].height = Number(document.getElementById("editheightinput").value);
	buildingList[buildId].rotation = Number(document.getElementById("editrotinput").value);
	buildingList[buildId].type = Number(document.getElementById("buildtypeinput").value);
	buildingList[buildId].stationCode = Number(document.getElementById("editcodeinput").value);
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