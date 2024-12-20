//nodes dont have to be in order - will be loaded into chunks

//---------------------------------------------------------------------------------------
// NODE
//---------------------------------------------------------------------------------------

const NODE_SIZE = 20;

class Node {
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
		this.height = 0;
	}

	draw(style = "#000000") {
		canvasData.context.fillStyle = style;
		canvasData.context.fillRect(
			canvasData.shiftX+this.xpos - NODE_SIZE/2,
			canvasData.shiftY+this.ypos - NODE_SIZE/2,
		NODE_SIZE, NODE_SIZE);
	}

	collision(ax, ay) {
		return (ax >= this.xpos-NODE_SIZE) &&
			(ax <= this.xpos+NODE_SIZE) &&
			(ay >= this.ypos-NODE_SIZE) &&
			(ay <= this.ypos+NODE_SIZE);
	}
};

let nodeList = [];

function getCollidingNode(ax, ay) {
	for(let i = 0; i < nodeList.length; i++) {
		if(nodeList[i].collision(ax, ay)) {
			return i;
		}
	}
	return -1;
}

function nodeEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing node "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+nodeList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+nodeList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+nodeList[aid].height+"><br>";

	canvasData.edit.innerHTML += "Node station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+nodeList[aid].stationCode+"><br>";
	canvasData.edit.innerHTML += "Node 2nd code:<input type='text' id='editcodetwoinput' name='editcodetwoinput' placeholder='XXXX' value="+nodeList[aid].bordersWith+"><br>";
	canvasData.edit.innerHTML += "<button type='' onclick='nodeUpdate()'>Update</button>";
}

function nodeUpdate() {
	console.log("Updating node...");

	let nodeId =  Number(document.getElementById("idinput").value);
	nodeList[nodeId].xpos = Number(document.getElementById("editxinput").value);
	nodeList[nodeId].ypos = Number(document.getElementById("edityinput").value);
	nodeList[nodeId].height = Number(document.getElementById("editheightinput").value);
	nodeList[nodeId].stationCode = String(document.getElementById("editcodeinput").value);
	nodeList[nodeId].bordersWith = String(document.getElementById("editcodetwoinput").value);

	canvasRedraw();
}