class Switch {
	xpos = 0; //same as node, interchangable
	ypos = 0;
	height = 0;
	stationCode = "";
	//switch cannot border 2 stations

	beforeId = -1; //before split
	frontId = -1; //after split
	leftId = -1; 
	rightId = -1;

	radioBoxId = -1;

	constructor(axpos, aypos, astationCode) {
		this.xpos = axpos;
		this.ypos = aypos;
		this.stationCode = astationCode;	
	}

	draw(style = "#aa6600") {
		if(!this.willRender()) { return; }
		console.log("switch draw");
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

function switchEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing switch (node no. "+aid+")<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += addBasicEditInputs(nodeList[aid]);
	
	canvasData.edit.innerHTML += "Before node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editbefinput' name='editbefinput' value="+nodeList[aid].beforeId+"><br>";
	canvasData.edit.innerHTML += "Front node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editfroinput' name='editfroinput' value="+nodeList[aid].frontId+"><br>";
	canvasData.edit.innerHTML += "Left node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editlefinput' name='editlefinput' value="+nodeList[aid].leftId+"><br>";
	canvasData.edit.innerHTML += "Right node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editriginput' name='editriginput' value="+nodeList[aid].rightId+"><br>";
	canvasData.edit.innerHTML += "Radio box id: <input type='number' min='-1' max='"+(radioList.length-1)+"' id='editradinput' name='editradinput' value="+nodeList[aid].radioBoxId+"><br>";

	canvasData.edit.innerHTML += "<hr><em>A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.</em><hr>";

	canvasData.edit.innerHTML += "<button type='' onclick='switchUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='nodeRemove()'>Remove switch</button>";
}

function switchUpdate() {
	console.log("Updating switch...");

	let switchId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(nodeList[switchId]);

	nodeList[switchId].beforeId = Number(document.getElementById("editbefinput").value);
	nodeList[switchId].frontId = Number(document.getElementById("editfroinput").value);
	nodeList[switchId].leftId = Number(document.getElementById("editlefinput").value);
	nodeList[switchId].rightId = Number(document.getElementById("editriginput").value);
	nodeList[switchId].radioBoxId = Number(document.getElementById("editradinput").value);

	canvasRedraw();
}

//removal same as node