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
	signalId = -1;
	signalLetter = '-';

	constructor(axpos = 0, aypos = 0, astationCode = "") {
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

let switchList = [];

function switchEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing switch no. "+aid+"<br>";
	canvasData.edit.innerHTML += "<input type='hidden' id='idinput' value="+aid+"><br>";

	addBasicEditInputs(switchList[aid]);
	
	canvasData.edit.innerHTML += "Before node id: <input type='number' min='-1' max='"+(switchList.length-1)+"' id='editbefinput' name='editbefinput' value="+switchList[aid].beforeId+"><br>";
	canvasData.edit.innerHTML += "Front node id: <input type='number' min='-1' max='"+(switchList.length-1)+"' id='editfroinput' name='editfroinput' value="+switchList[aid].frontId+"><br>";
	canvasData.edit.innerHTML += "Left node id: <input type='number' min='-1' max='"+(switchList.length-1)+"' id='editlefinput' name='editlefinput' value="+switchList[aid].leftId+"><br>";
	canvasData.edit.innerHTML += "Right node id: <input type='number' min='-1' max='"+(switchList.length-1)+"' id='editriginput' name='editriginput' value="+switchList[aid].rightId+"><br>";
	canvasData.edit.innerHTML += "Radio box id: <input type='number' min='-1' max='"+(radioList.length-1)+"' id='editradinput' name='editradinput' value="+switchList[aid].radioBoxId+"><br>";
	canvasData.edit.innerHTML += "Switch signal id: <input type='number' min='-1' max='"+(switchSignalList.length-1)+"' id='editsiginput' name='editsiginput' value="+switchList[aid].signalId+"><br>";
	canvasData.edit.innerHTML += "Switch signal letter: <input type='text' maxlength='1' id='editsiglinput' name='editsiglinput' value="+switchList[aid].signalLetter+"><br>";

	canvasData.edit.innerHTML += "<hr><em>A junction is a split - the track from the 'before' node splits into the 'front', 'left' and 'right' tracks. Enter value of -1 if switch does not turn to that direction.</em><hr>";

	canvasData.edit.innerHTML += "<hr><em>If the switch signal letter is equal to '-' or the switch signal id is -1, the switch does not have a signal attached to it.</em><hr>";

	canvasData.edit.innerHTML += "<button type='' onclick='switchUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='switchRemove()'>Remove switch</button>";
}

function switchUpdate() {
	console.log("Updating switch...");

	let switchId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(switchList[switchId]);

	//TODO check if switch doesnt self-reference

	switchList[switchId].beforeId = Number(document.getElementById("editbefinput").value);
	switchList[switchId].frontId = Number(document.getElementById("editfroinput").value);
	switchList[switchId].leftId = Number(document.getElementById("editlefinput").value);
	switchList[switchId].rightId = Number(document.getElementById("editriginput").value);
	switchList[switchId].radioBoxId = Number(document.getElementById("editradinput").value);
	switchList[switchId].signalId = Number(document.getElementById("editsiginput").value);
	switchList[switchId].signalLetter = document.getElementById("editsiglinput").value;

	canvasRedraw();
}

function switchRemove() {
	console.log("Removing switch and connected track");

	let switchId = Number(document.getElementById("idinput").value);

	trackList = trackList.filter((v, i) => {
		return !((v.nodeIdFirst === switchId && v.firstIsSwitch) || (v.nodeIdSecond === switchId && v.secondIsSwitch));
	});

	switchList.splice(switchId, 1);

	canvasData.edit.innerHTML = "";

	canvasRedraw();
}