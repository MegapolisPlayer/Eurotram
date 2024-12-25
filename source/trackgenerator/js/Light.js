const LIGHT_LENGTH = 40;

function toRadians(degrees) {
	return degrees * Math.PI / 180.0;
}

class Light {
	xpos = 0;
	ypos = 0;
	height = 0;
	rotation = 0;	

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
			canvasData.shiftX + this.xpos,
			canvasData.shiftY + this.ypos
		);

		let endX = canvasData.shiftX + this.xpos + (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let endY = canvasData.shiftY + this.ypos + (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		canvasData.context.lineTo(endX, endY);

		canvasData.context.stroke();

		canvasData.context.fillRect(
			endX - NODE_SIZE/4,
			endY - NODE_SIZE/4,
		NODE_SIZE/2, NODE_SIZE/2);
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

		let minX = canvasData.shiftX + this.xpos;
		let minY = canvasData.shiftY + this.ypos;

		let sizeX = (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let sizeY = (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		//move top left corner
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

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+lightList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+lightList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+lightList[aid].height+"><br>";
	canvasData.edit.innerHTML += "Rotation:<input type='number' id='editrotinput' name='editrotinput' value="+lightList[aid].rotation+"><br>";

	canvasData.edit.innerHTML += "<button type='' onclick='lightUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='lightRemove()'>Remove light</button>";
}

function lightUpdate() {
	console.log("Updating light...");

	let lightId = Number(document.getElementById("idinput").value);

	lightList[lightId].xpos = Number(document.getElementById("editxinput").value);
	lightList[lightId].ypos = Number(document.getElementById("edityinput").value);
	lightList[lightId].height = Number(document.getElementById("editheightinput").value);
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