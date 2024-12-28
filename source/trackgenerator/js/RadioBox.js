class RadioBox {
	xpos = 0;
	ypos = 0;
	height = 0;
	stationCode = "";

	//we dont store value of switch - switch stores our value
	//in simulator - when tram in 1-2 meter radius - sends signal
	//1 radiobox may control multiple switches

	constructor(axpos, aypos, aheight) {
		this.xpos = axpos;
		this.ypos = aypos;
		this.height = aheight;
	}

	draw(style = "#800000") {
		if(!this.willRender()) { return; }
		console.log("radiobox draw");
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
}

let radioList = [];

function radioEditMenu(aid) {
	canvasData.edit.innerHTML = "";

	canvasData.edit.innerHTML += "Editing radiobox "+aid+"<br>";

	canvasData.edit.innerHTML += addBasicEditInputs(radioList[aid]);

	canvasData.edit.innerHTML += "<button type='' onclick='radioUpdate()'>Update</button>";
	canvasData.edit.innerHTML += "<button type='' onclick='radioRemove()'>Remove radio</button>";
}

function radioUpdate() {
	console.log("Updating radio...");

	let radioId =  Number(document.getElementById("idinput").value);

	getDataFromBasicInputs(radioList[radioId]);

	canvasRedraw();
}

function radioRemove() {
	console.log("Removing radio...");

	let radioId =  Number(document.getElementById("idinput").value);

	//remove references of switches
	nodeList.forEach((v) => {
		if(v instanceof Switch) {
			if(v.radioBoxId == radioId) {
				v.radioBoxId = -1;
			}
		}
	});

	nodeList.splice(radioId, 1);

	canvasData.edit.innerHTML = "";

	canvasRedraw();
}