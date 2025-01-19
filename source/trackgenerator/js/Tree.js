const TREE_RADIUS = 40;

class Tree extends StandardPoint {
	stationCode = "";

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
		this.pointSizeX = TREE_RADIUS*2;
		this.pointSizeY = TREE_RADIUS*2;
	}

	draw(aStyle = "#00ff00") {
		abstractDrawPoint(aStyle, this, () => {
			canvasData.context.beginPath();
			canvasData.context.arc(
				TREE_RADIUS,
				TREE_RADIUS,
				TREE_RADIUS, 0, 2*Math.PI
			);
			canvasData.context.fill();
		}, true);
	}
};

let treeList = [];

function treeEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing tree no. "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(treeList[aID]);

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let treeId = getIDFromInput();
		getDataFromBasicInputs(treeList[treeId]);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(treeList);
	});
	canvasData.edit.appendChild(removeButton);
}