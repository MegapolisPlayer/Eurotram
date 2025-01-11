const TREE_RADIUS = 40;

class Tree {
	xpos = 0;
	ypos = 0;
	height = 0;
	stationCode = "";

	constructor(axpos = 0, aypos = 0) {
		this.xpos = axpos;
		this.ypos = aypos;
	}

	draw(style = "#00ff00") {
		if(!this.willRender()) { return; }
		console.log("tree draw");
		canvasData.context.fillStyle = style;
		canvasData.context.beginPath();
		canvasData.context.arc(
			this.xpos,
			this.ypos,
			TREE_RADIUS, 0, 2*Math.PI
		);
		canvasData.context.fill();
	}

	collision(ax, ay) {
		return (ax >= this.xpos-TREE_RADIUS) &&
			(ax <= this.xpos+TREE_RADIUS) &&
			(ay >= this.ypos-TREE_RADIUS) &&
			(ay <= this.ypos+TREE_RADIUS);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xpos - TREE_RADIUS,
			this.ypos - TREE_RADIUS, 
			TREE_RADIUS*2, TREE_RADIUS*2)
	}
};

let treeList = [];

function treeEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing tree no. "+aid));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aid);

	addBasicEditInputs(treeList[aid]);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", treeUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", treeRemove);
	canvasData.edit.appendChild(removeButton);
}

function treeUpdate() {
	console.log("Updating tree...");

	let treeId = Number(document.getElementById("idinput").value);
	getDataFromBasicInputs(treeList[treeId]);

	canvasRedraw();
}

function treeRemove() {
	console.log("Updating tree...");

	let treeId = Number(document.getElementById("idinput").value);
	treeList.splice(treeId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}