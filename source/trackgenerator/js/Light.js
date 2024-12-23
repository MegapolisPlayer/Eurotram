const LIGHT_LENGTH = 100;

class Light {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;	

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#ffff00") {

	}
};

lightList = [];

function lightEditMenu() {
	canvasData.edit.innerHTML = "";
}

function lightUpdate() {

}

function lightRemove() {

}