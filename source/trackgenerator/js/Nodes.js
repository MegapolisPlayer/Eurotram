//nodes dont have to be in order - will be loaded into chunks

const NODE_SIZE = 40;

class TNode extends StandardPoint {	 
	stationCode = "";
	bordersWith = ""; //node can be in 2 stations (will be duplicated on save)

	constructor(axPos = 0, ayPos = 0, astationCode = "", abordersWith = "") {
		super(axPos, ayPos);
		this.stationCode = astationCode;
		this.bordersWith = abordersWith;
	}

	draw(aStyle = "#000000") {
		abstractDrawPoint(aStyle, this);
	}
};

//switches also in node list
let nodeList = [];

function nodeEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing node "+aID));
	addHiddenIdInput(aID);

	canvasData.edit.appendChild(document.createElement("br"));

	addBasicEditInputs(nodeList[aID]);

	canvasData.edit.appendChild(document.createTextNode("2nd code: "));

	addInputPlaceholder("editcodetwoinput", nodeList[aID].bordersWith, "text", "XXXX");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		let nodeId = getIDFromInput();
		getDataFromBasicInputs(nodeList[nodeId]);
		nodeList[nodeId].bordersWith = String(document.getElementById("editcodetwoinput").value);
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", () => {
		removeFromListById(nodeList, (aID, aList) => {
			trackList = trackList.filter((v, i) => {
				return !((v.nodeIdFirst === aID && !v.firstIsSwitch) ||
				(v.nodeIdSecond === aID && !v.secondIsSwitch));
			});
		
			//shift track
			trackList.forEach((v, i, a) => {
				if(v.nodeIdFirst > aID && !v.firstIsSwitch) {
					a[i].nodeIdFirst--;
				}
				if(v.nodeIdSecond > aID && !v.secondIsSwitch) {
					a[i].nodeIdSecond--;
				}
			});
		});
	});
	canvasData.edit.appendChild(removeButton);
}