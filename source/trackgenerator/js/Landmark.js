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

class Landmark {
	xpos = 0;
	ypos = 0;
	height = 0;
	//no station code - always rendered

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#00ff00") {
		
	}

	collision(ax, ay) {
		return (ax >= this.xpos-NODE_SIZE) &&
			(ax <= this.xpos+NODE_SIZE) &&
			(ay >= this.ypos-NODE_SIZE) &&
			(ay <= this.ypos+NODE_SIZE);
	}

	willRender() {
		return canvasIsInFrustum(
			canvasData.shiftX + this.xpos - NODE_SIZE/2,
			canvasData.shiftY + this.ypos - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE)
	}
};

landmarkList = [];

function landmarkEditMenu() {
	canvasData.edit.innerHTML = "";
}

function landmarkUpdate() {
	console.log("Updating landmark");
}

function landmarkRemove() {
	console.log("Removing landmark");


}