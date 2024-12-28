const LIGHT_LENGTH = 40;

function toRadians(degrees) {
	return degrees * Math.PI / 180.0;
}

class Light {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;	
	stationCode = "";

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#808080") {
		if(!this.willRender()) { return; }
		console.log("draw light");

		canvasData.context.fillStyle = style;
		canvasData.context.strokeStyle = style;

		canvasData.context.beginPath();

		canvasData.context.moveTo(
			this.xpos,
			this.ypos
		);

		let endX = this.xpos + (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let endY = this.ypos + (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		canvasData.context.lineTo(endX, endY);

		canvasData.context.stroke();

		canvasData.context.fillRect(
			endX - NODE_SIZE/2,
			endY - NODE_SIZE/2,
		NODE_SIZE, NODE_SIZE);
	}

	collision(ax, ay) {
		let endX = this.xpos + (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let endY = this.ypos + (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		//hitbox larger than box itself so it is easier to click on it
		//consider both "nodes" (start, end)
		return (
		((ax >= endX-NODE_SIZE) &&
		(ax <= endX+NODE_SIZE) &&
		(ay >= endY-NODE_SIZE) &&
		(ay <= endY+NODE_SIZE)) ||
		((ax >= this.xpos-NODE_SIZE) &&
		(ax <= this.xpos+NODE_SIZE) &&
		(ay >= this.ypos-NODE_SIZE) &&
		(ay <= this.ypos+NODE_SIZE))
		);
	}

	willRender() {
		//check rectangle around lamp

		let minX = this.xpos;
		let minY = this.ypos;

		let sizeX = (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let sizeY = (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		//move to top left corner
		if(sizeX < 0) {
			minX += sizeX;
		}
		if(sizeY < 0) {
			minY += sizeY;
		}

		return canvasIsInFrustum(
			minX, minY,
			Math.abs(sizeX),
			Math.abs(sizeY)
		);
	}
};

lightList = [];

function lightEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing light "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += addBasicEditInputs(lightList[aid]);
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+lightList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='lightUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='lightRemove()'>Remove light</button>";
}

function lightUpdate() {
	console.log("Updating light...");

	let lightId = Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(lightList[lightId]);
	lightList[lightId].rotation = Number(document.getElementById("editrotinput").value);

	canvasRedraw();
}

function lightRemove() {
	console.log("Remove light...");

	let lightId = Number(document.getElementById("idinput").value);
	lightList.splice(lightId, 1);
	canvasData.edit.innerHTML = "";

	canvasRedraw();
}