class RadioBox {
	xpos = 0;
	ypos = 0;

	//we dont store value of switch - switch stores our value
	//in simulator - when tram in 1-2 meter radius - sends signal
	//1 radiobox may control multiple switches

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#ff0000") {
		if(!this.willRender()) { return; }
		console.log("radiobox draw");
		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(
			canvasData.shiftX+this.xpos - NODE_SIZE/2,
			canvasData.shiftY+this.ypos - NODE_SIZE/2,
		NODE_SIZE, NODE_SIZE);
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
}

let radioList = [];

function radioUpdate() {

}

function radioRemove() {

}