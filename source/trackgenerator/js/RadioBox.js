class Radiobox extends StandardPoint{
	stationCode = "";

	//we dont store value of switch - switch stores our value
	//in simulator - when tram in 1-2 meter radius - sends signal
	//1 radiobox may control multiple switches

	constructor(axPos = 0, ayPos = 0) {
		super(axPos, ayPos);
	}

	draw(aStyle = "#800000") {
		abstractDrawPoint(aStyle, this);
	}
}

let radioList = [];

function radioEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing radiobox "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);

	addBasicEditInputs(radioList[aID]);

	let updateButton = document.createElement("button");
	updateButton.appendChild(document.createTextNode("Update"));
	updateButton.addEventListener("click", () => {
		let radioId = getIDFromInput();
		getDataFromBasicInputs(radioList[radioId]);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);
	
	let removeButton = document.createElement("button");
	removeButton.appendChild(document.createTextNode("Remove"));
	removeButton.addEventListener("click", () => {
		removeFromListById(radioList, () => {
			//remove references of switches
			nodeList.forEach((v) => {
				if(v instanceof Switch) {
					if(v.radioBoxId == radioId) {
						v.radioBoxId = -1;
					}
				}
			});
		});
	});
	canvasData.edit.appendChild(removeButton);
}
