class Switch extends StandardPoint {
	stationCode = "";
	//switch cannot border 2 stations

	beforeId = -1; //before split
	frontId = -1; //after split
	leftId = -1; 
	rightId = -1;

	radioBoxId = -1;
	signalId = -1;
	signalLetter = '-';

	constructor(axPos = 0, ayPos = 0, astationCode = "") {
		super(axPos, ayPos);
		this.stationCode = astationCode;	
	}

	draw(aStyle = "#aa6600") {
		abstractDrawPoint(aStyle, this);
	}
};

let switchList = [];

function switchEditMenu(aID) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing switch no. "+aID));
	canvasData.edit.appendChild(document.createElement("br"));

	addHiddenIdInput(aID);
	canvasData.edit.appendChild(document.createElement("br"));

	addBasicEditInputs(switchList[aID]);
	
	canvasData.edit.appendChild(document.createTextNode("Before node id: "));
	addInput(aID, switchList[aID].beforeId, "text");

	canvasData.edit.appendChild(document.createTextNode("Front node id: "));
	addInput(aID, switchList[aID].frontId, "text");

	canvasData.edit.appendChild(document.createTextNode("Left node id: "));
	addInput(aID, switchList[aID].leftId, "text");

	canvasData.edit.appendChild(document.createTextNode("Right node id: "));
	addInput(aID, switchList[aID].rightId, "text");

	canvasData.edit.appendChild(document.createTextNode("Radio box id: "));
	addInput(aID, switchList[aID].radioBoxId, "text");

	canvasData.edit.appendChild(document.createTextNode("Switch signal id: "));
	addInput(aID, switchList[aID].signalId, "text");

	canvasData.edit.appendChild(document.createTextNode("Switch signal letter: "));
	addInput(aID, switchList[aID].signalLetter, "text");

	canvasData.edit.appendChild(document.createElement("hr"));

	let em1 = document.createElement("em");
	em1.textContent = "A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.";
	canvasData.edit.appendChild(em1);
	canvasData.edit.appendChild(document.createElement("hr"));

	let em2 = document.createElement("em");
	em2.textContent = "If the switch signal letter is equal to '-' or the switch signal id is -1, the switch does not have a signal attached to it.";
	canvasData.edit.appendChild(em2);
	canvasData.edit.appendChild(document.createElement("hr"));

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", () => {
		//TODO check if switch doesnt self-reference
		let switchId = getIDFromInput();
		getDataFromBasicInputs(switchList[switchId]);
		switchList[switchId].beforeId = Number(document.getElementById("editbefinput").value);
		switchList[switchId].frontId = Number(document.getElementById("editfroinput").value);
		switchList[switchId].leftId = Number(document.getElementById("editlefinput").value);
		switchList[switchId].rightId = Number(document.getElementById("editriginput").value);
		switchList[switchId].radioBoxId = Number(document.getElementById("editradinput").value);
		switchList[switchId].signalId = Number(document.getElementById("editsiginput").value);
		switchList[switchId].signalLetter = document.getElementById("editsiglinput").value;
		canvasRedraw();
	});
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove switch";
	removeButton.addEventListener("click", () => {
		removeFromListById(switchList, (aID, aList) => {
			trackList = trackList.filter((v, i) => {
				return !((v.nodeIdFirst === aID && v.firstIsSwitch) || (v.nodeIdSecond === aID && v.secondIsSwitch));
			});
		
			//shift track
			trackList.forEach((v, i, a) => {
				if(v.nodeIdFirst > aID && v.firstIsSwitch) {
					a[i].nodeIdFirst--;
				}
				if(v.nodeIdSecond > aID && v.secondIsSwitch) {
					a[i].nodeIdSecond--;
				}
			});
		});
	});
	canvasData.edit.appendChild(removeButton);
}