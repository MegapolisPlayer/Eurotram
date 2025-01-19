const LIGHT_LENGTH = 60;

class Light extends RotatedStandardPoint {	
	stationCode = "";

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
	}

	draw(aStyle = "#808080") {
		abstractDrawPoint(aStyle, this, () => {
			canvasData.context.strokeStyle = aStyle;

			canvasData.context.beginPath();
			canvasData.context.moveTo(NODE_SIZE/2, 0);
			canvasData.context.lineTo(NODE_SIZE/2, LIGHT_LENGTH);
			canvasData.context.stroke();
	
			canvasData.context.fillRect(
				0, LIGHT_LENGTH-NODE_SIZE/2,
				NODE_SIZE, NODE_SIZE);
		}, true);
	}

	collision(aX, aY) {
		let endX = this.xPos + (LIGHT_LENGTH * Math.sin(toRadians(this.rotation)));
		let endY = this.yPos + (LIGHT_LENGTH * Math.cos(toRadians(this.rotation)));

		//hitbox larger than box itself so it is easier to click on it
		//consider both "nodes" (start, end)
		return (
		((aX >= endX-NODE_SIZE) &&
		(aX <= endX+NODE_SIZE) &&
		(aY >= endY-NODE_SIZE) &&
		(aY <= endY+NODE_SIZE)) ||
		((aX >= this.xPos-NODE_SIZE) &&
		(aX <= this.xPos+NODE_SIZE) &&
		(aY >= this.yPos-NODE_SIZE) &&
		(aY <= this.yPos+NODE_SIZE))
		);
	}

	willRender() {
		//check rectangle around lamp

		let minX = this.xPos;
		let minY = this.yPos;

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

function lightEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing light "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(lightList[aID]);

	canvasData.edit.appendChild(document.createTextNode("Rotation: "));

	addInput("editrotinput", lightList[aID].rotation, "text");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let lightId = getIDFromInput();
		getDataFromBasicInputs(lightList[lightId]);
		lightList[lightId].rotation = Number(document.getElementById("editrotinput").value);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(lightList);
	});
	canvasData.edit.appendChild(removeButton);
}