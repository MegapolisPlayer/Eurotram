//nodes dont have to be in order - will be loaded into chunks

const NODE_SIZE = 20;

class TNode {
	xpos = 0;
	ypos = 0;
	stationCode = "";
	bordersWith = ""; //node can be in 2 stations (will be duplicated on save)
	height = 0;

	constructor(axpos, aypos, astationCode, abordersWith, height) {
		this.xpos = axpos;
		this.ypos = aypos;
		this.stationCode = astationCode;
		this.bordersWith = abordersWith;
		this.height = height;
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
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing node "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += addBasicEditInputs(nodeList[aid]);
	canvasData.edit.innerHTML += "2nd code:<input type='text' id='editcodetwoinput' name='editcodetwoinput' placeholder='XXXX' value="+nodeList[aid].bordersWith+"><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='nodeUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='nodeRemove()'>Remove node</button>";
}

function nodeUpdate() {
	console.log("Updating node...");

	let nodeId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(nodeList[nodeId]);
	nodeList[nodeId].bordersWith = String(document.getElementById("editcodetwoinput").value);

	canvasRedraw();
}

function nodeRemove() {
	console.log("Removing node/switch and connected track");

	let nodeId = Number(document.getElementById("idinput").value);

	console.log(trackList.length);

	trackList = trackList.filter((v, i) => {
		return !(v.nodeIdFirst === nodeId || v.nodeIdSecond === nodeId);
	});

	console.log(trackList.length);

	nodeList.splice(nodeId, 1);

	canvasData.edit.innerHTML = "";

	canvasRedraw();
}