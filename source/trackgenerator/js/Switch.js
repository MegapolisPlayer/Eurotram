class Switch {
	xpos = 0; //same as node, interchangable
	ypos = 0;
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

	draw(style = "#00ff00") {
		if(!this.willRender()) { return; }
		console.log("switch draw");
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

	willRender() {
		return canvasIsInFrustum(
			canvasData.shiftX + this.xpos - NODE_SIZE/2,
			canvasData.shiftY + this.ypos - NODE_SIZE/2, 
			NODE_SIZE, NODE_SIZE)
	}
};

//switches also in node list

function switchEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing switch (node no. "+aid+")<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	canvasData.edit.innerHTML += "X:<input type='number' id='editxinput' name='editxinput' value="+nodeList[aid].xpos+"><br>";
	canvasData.edit.innerHTML += "Y:<input type='number' id='edityinput' name='edityinput' value="+nodeList[aid].ypos+"><br>";
	canvasData.edit.innerHTML += "Height:<input type='number' id='editheightinput' name='editheightinput' value="+nodeList[aid].height+"><br>";

	canvasData.edit.innerHTML += "Switch station code:<input type='text' id='editcodeinput' name='editcodeinput' placeholder='XXXX' value="+nodeList[aid].stationCode+"><br>";
	canvasData.edit.innerHTML += "Before node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editbefinput' name='editbefinput' value="+nodeList[aid].beforeId+"><br>";
	canvasData.edit.innerHTML += "Front node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editfroinput' name='editfroinput' value="+nodeList[aid].frontId+"><br>";
	canvasData.edit.innerHTML += "Left node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editlefinput' name='editlefinput' value="+nodeList[aid].leftId+"><br>";
	canvasData.edit.innerHTML += "Right node id: <input type='number' min='-1' max='"+(nodeList.length-1)+"' id='editriginput' name='editriginput' value="+nodeList[aid].rightId+"><br>";
	canvasData.edit.innerHTML += "Radio box id: <input type='number' min='-1' max='"+(radioList.length-1)+"' id='editradinput' name='editradinput' value="+nodeList[aid].radioBoxId+"><br>";

	canvasData.edit.innerHTML += "<hr><em>A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.</em><hr>";

	canvasData.edit.innerHTML += "<button type='' onclick='switchUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='switchRemove()'>Remove switch</button>";
}

function switchUpdate() {

}

function switchRemove() {

}