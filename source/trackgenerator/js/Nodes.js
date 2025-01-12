//nodes dont have to be in order - will be loaded into chunks

const NODE_SIZE = 40;

class TNode {
	xpos = 0;
	ypos = 0;
	stationCode = "";
	bordersWith = ""; //node can be in 2 stations (will be duplicated on save)
	height = 0;

	constructor(axpos = 0, aypos = 0, astationCode = "", abordersWith = "") {
		this.xpos = axpos;
		this.ypos = aypos;
		this.stationCode = astationCode;
		this.bordersWith = abordersWith;
	}

	draw(style = "#000000") {
		if(!this.willRender()) { return; }
		console.log("node draw");
		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(
			this.xpos - NODE_SIZE/2,
			this.ypos - NODE_SIZE/2,
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
			this.xpos - NODE_SIZE/2,
			this.ypos - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE)
	}
};

//switches also in node list
let nodeList = [];

function nodeEditMenu(aid) {
	canvasData.edit.replaceChildren();

	canvasData.edit.appendChild(document.createTextNode("Editing node "+aid));
	addHiddenIdInput(aid);

	canvasData.edit.appendChild(document.createElement("br"));

	addBasicEditInputs(nodeList[aid]);

	canvasData.edit.appendChild(document.createTextNode("2nd code: "));

	addInputPlaceholder("editcodetwoinput", nodeList[aid].bordersWith, "text", "XXXX");

	let updateButton = document.createElement("button");
	updateButton.textContent = "Update";
	updateButton.addEventListener("click", nodeUpdate);
	canvasData.edit.appendChild(updateButton);

	let removeButton = document.createElement("button");
	removeButton.textContent = "Remove";
	removeButton.addEventListener("click", nodeRemove);
	canvasData.edit.appendChild(removeButton);
}

function nodeUpdate() {
	console.log("Updating node...");

	let nodeId = Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(nodeList[nodeId]);
	nodeList[nodeId].bordersWith = String(document.getElementById("editcodetwoinput").value);

	canvasRedraw();
}

function nodeRemove() {
	console.log("Removing node and connected track");

	let nodeId = Number(document.getElementById("idinput").value);

	trackList = trackList.filter((v, i) => {
		return !((v.nodeIdFirst === nodeId && !v.firstIsSwitch) || (v.nodeIdSecond === nodeId && !v.secondIsSwitch));
	});

	//shift track
	trackList.forEach((v, i, a) => {
		if(v.nodeIdFirst > nodeId && !v.firstIsSwitch) {
			a[i].nodeIdFirst--;
		}
		if(v.nodeIdSecond > nodeId && !v.secondIsSwitch) {
			a[i].nodeIdSecond--;
		}
	});

	nodeList.splice(nodeId, 1);
	canvasData.edit.replaceChildren();

	canvasRedraw();
}