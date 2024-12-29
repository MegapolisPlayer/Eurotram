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

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw() {
		//if(!this.willRender()) { return; }

		canvasData.context.translate(this.xpos, this.ypos);
		canvasData.context.rotate(toRadians(this.rotation));
		canvasData.context.translate(-this.xsize/2,-this.ysize/2);

		//TODO draw transparent rectangle w/ transparent border

		canvasData.context.translate(this.xsize/2,this.ysize/2);
		canvasData.context.rotate(-toRadians(this.rotation));
		canvasData.context.translate(-this.xpos, -this.ypos);
	}

	collision() {

	}

	willRender() {
		//TODO
	}
}

let texparcelList = [];

function renderTextureParcels() {
	if(canvasData.hideTPElem) { return; } //if hidden
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
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='tprinput' name='tprinput' value='200'><br>";

	canvasData.edit.innerHTML += "Station code 1:<input type='text' id='tps1input' name='tps1input' placeholder='XXXX'><br>";
	canvasData.edit.innerHTML += "Station code 2:<input type='text' id='tps2input' name='tps2input' placeholder='XXXX'><br>";

	

	canvasData.edit.innerHTML += "<button onclick='texparcelMake()'>Add texparcel</button>";
}
function texparcelMake() {

	texparcelList.push(new Texparcel());

	canvasData.edit.innerHTML = ""; //clear AFTER getting values
}

function texparcelEditMenu(aid) {

}
function texparcelUpdate() {
	console.log("Updating texparcel...");
}
function texparcelRemove() {
	console.log("Removing texparcel...");
}

/*
TODO remove when texparcels code done
function renderGrid() {
	let xAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.width/GRID_SIZE)+1;
	let yAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.height/GRID_SIZE)+1;
	let xStart = Math.floor(-canvasData.shiftX / GRID_SIZE) * GRID_SIZE;
	let yStart = Math.floor(-canvasData.shiftY / GRID_SIZE) * GRID_SIZE;

	canvasData.context.fillStyle = "#ff0000";
	canvasData.context.globalAlpha = 0.25;

	for(let i = 0; i < yAmountToDraw; i++) {
		for(let j = 0; j < xAmountToDraw; j++) {
			if((j % 2) == (i % 2 == 0)) {
				canvasData.context.fillRect(
					xStart+(j*GRID_SIZE),
					yStart+(i*GRID_SIZE),
					GRID_SIZE, GRID_SIZE
				);
			}
		}
	}

	canvasData.context.globalAlpha = 1;
}
*/