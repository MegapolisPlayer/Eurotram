let buildingType = {
	NORMAL: 0,
	CORNER: 1,
	FAMILY: 2,
	PREFAB: 3
};

//building types
//NORMAL
//
//CORNER
//
//FAMILY
//
//PREFAB
//

class Building {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;
	xsize = 0;
	ysize = 0;
	type = 0;

	constructor(axpos, aypos, atype) {
		this.xpos = axpos;
		this.ypos = aypos;
		this.type = atype;
		this.updateFromType(); //xsize, ysize
	}

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
		}

		this.xsize *= UNITS_PER_METER;
		this.ysize *= UNITS_PER_METER;
	}

	draw(style = "#ff0000") {
		if(!this.willRender()) { return; }
		console.log("building draw");

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));

		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(0, 0, this.xsize, this.ysize);
		canvasData.context.strokeStyle = "#000000";
		
		canvasData.context.strokeRect(0, 0, this.xsize, this.ysize);


		//reset back
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

function buildingSelectMenu(ax, ay) {
	canvasData.edit.innerHTML = "Select building type at pos x = "+ax+", y = "+ay+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='buildxinput' value="+ax+"><br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='buildyinput' value="+ay+"><br>";

	//TODO use select/option
	//TODO add same to edit menu

	//use buildingType "enum"
	canvasData.edit.innerHTML += "<button type='' onclick='buildingMake(0)'>Normal</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingMake(1)'>Corner</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingMake(2)'>Family</button><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingMake(3)'>Prefab</button><br>";
}

function buildingMake(atype) {
	let x = Number(document.getElementById("buildxinput").value);
	let y = Number(document.getElementById("buildyinput").value);

	buildingList.push(new Building(x, y, atype));
	buildingList.at(-1).draw();

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function buildingEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing buidling "+aid+"<br>";
	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+buildingList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+buildingList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+buildingList[aid].height+"><br>";


	canvasData.edit.innerHTML += "<button type='' onclick='buildingUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='buildingRemove()'>Remove buidling</button>";
}

function buildingUpdate() {
	console.log(" Updating building..");

	canvasRedraw();
}

function buildingRemove() {
	console.log("Removing building...");

	canvasRedraw();
}