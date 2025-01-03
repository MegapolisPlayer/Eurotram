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

let landmarkTypeSelector = "";

function makeLandmarkSelector() {
	//TODO for future release - rewrite to createElement syntax (and textContent)
	landmarkTypeSelector = 
	"<label for='landmarktypeinput'>Select landmark type:</label>"+
	"<select name='landmarktypeinput' id='landmarktypeinput'>";

	landmarkSizes.forEach((v) => {
		landmarkTypeSelector += "<option value="+v.code+">"+v.name+"</option>";
	});

	landmarkTypeSelector += "</select><br>";
}

class Landmark {
	xpos = 0;
	ypos = 0;
	xsize = 0;
	ysize = 0;
	height = 0;
	rotation = 0;
	//no station code - always rendered
	landmarkCode = 0;

	constructor(axpos = 0, aypos = 0, acode = 0) {
		this.xpos = axpos;
		this.ypos = aypos;
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

	draw(style = "#ff0000") {
		if(!this.willRender()) { return; }
		console.log("landmark draw");

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
			this.xpos - this.xsize/2,
			this.ypos - this.ysize/2, 
			this.xsize, this.ysize)
	}
};

landmarkList = [];

function landmarkSelectMenu(ax, ay) {
	canvasData.edit.innerHTML = "Select landmark at pos x = "+ax+", y = "+ay+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='landmarkxinput' value="+ax+"><br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='landmarkyinput' value="+ay+"><br>";

	canvasData.edit.innerHTML += landmarkTypeSelector;

	canvasData.edit.innerHTML += "<button onclick='landmarkMake()'>Add landmark</button>";
}

function landmarkMake() {
	let x = Number(document.getElementById("landmarkxinput").value);
	let y = Number(document.getElementById("landmarkyinput").value);
	let landmarkCode = document.getElementById("landmarktypeinput").value;

	landmarkList.push(new Landmark(x, y, landmarkCode));
	landmarkList.at(-1).draw();

	canvasData.edit.innerHTML = "";
}

function landmarkEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing landmark "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+landmarkList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+landmarkList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+landmarkList[aid].height+"><br>";
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+landmarkList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += landmarkTypeSelector;

	canvasData.edit.innerHTML += "<button type='' onclick='landmarkUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='landmarkRemove()'>Remove</button>";
}

function landmarkUpdate() {
	console.log("Updating landmark");

	let landmarkId = Number(document.getElementById("idinput").value);

	landmarkList[landmarkId].xpos = Number(document.getElementById("editxinput").value);
	landmarkList[landmarkId].ypos = Number(document.getElementById("edityinput").value);
	landmarkList[landmarkId].height = Number(document.getElementById("editheightinput").value);
	landmarkList[landmarkId].rotation = Number(document.getElementById("editrotinput").value);
	landmarkList[landmarkId].landmarkCode = document.getElementById("landmarktypeinput").value;
	landmarkList[landmarkId].updateFromLandmarkCode();
	
	canvasRedraw();
}

function landmarkRemove() {
	console.log("Removing landmark");

	let landmarkId = Number(document.getElementById("idinput").value);
	landmarkList.splice(landmarkId, 1);
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}