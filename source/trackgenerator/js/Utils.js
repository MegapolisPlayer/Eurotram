//General utilities

function toRadians(aDegrees) {
	return aDegrees * Math.PI / 180.0;
}

function getPoint(aEvent) {
	return canvasData.context.getTransform().invertSelf().transformPoint(new DOMPoint(
		Math.trunc(aEvent.pageX - aEvent.target.offsetLeft),
		Math.trunc(aEvent.pageY - aEvent.target.offsetTop)
	));
}

function getPropertyOfValue(aObject, aValue) {
	for(let p in aObject) {
		if(aObject[p] === aValue) {
			return p;
		}
	}	
	console.error("Property with value not found!");
	return Number.MIN_VALUE;
}

//Management utilities

function getColliding(aList, aX, aY) {
	for(let i = 0; i < aList.length; i++) {
		if(aList[i].collision(aX, aY)) {
			return i;
		}
	}
	return -1;
}

function getIDFromInput() {
	return Number(document.getElementById("idinput").value);
}

//requirement: idinput input tag with id
//function params: this = object, second param id of object
function removeFromListById(aList, aFunction = null) {
	let id = getIDFromInput();

	if(aFunction) {
		aFunction.call(aList[id], id, aList);
	}

	aList.splice(id, 1);
	canvasData.edit.replaceChildren();
	canvasRedraw();
}

//Draw abstractions

class StandardPoint {
	xPos = 0;
	yPos = 0;
	height = 0;

	pointSizeX = NODE_SIZE;
	pointSizeY = NODE_SIZE;

	constructor(axPos = 0, ayPos = 0) {
		this.xPos = axPos;
		this.yPos = ayPos;
	}

	draw() {
		console.error("StandardPoint draw() function must be overriden!");
	}

	collision(aX, aY) {
		return (aX >= this.xPos-this.pointSizeX) &&
			(aX <= this.xPos+this.pointSizeX) &&
			(aY >= this.yPos-this.pointSizeY) &&
			(aY <= this.yPos+this.pointSizeY);
	}

	willRender() {
		return canvasIsInFrustum(
			this.xPos - this.pointSizeX/2,
			this.yPos - this.pointSizeY/2, 
			this.pointSizeX, this.pointSizeY)
	}
};

class RotatedStandardPoint extends StandardPoint {
	rotation = 0;

	constructor(axPos = 0, ayPos = 0, aRotation = 0) {
		super(axPos, ayPos);
		this.rotation = aRotation;
	}
};

//TODO change abstract to base!

function abstractDrawPoint(aStyle, aObject, aFunction = null, aNoBaseRect = false) {
	if(!aObject.willRender()) { return; }

	canvasData.context.translate(aObject.xPos, aObject.yPos);
	if(aObject.rotation !== undefined) {
		//if has rotation - use rotation
		canvasData.context.rotate(toRadians(aObject.rotation));
	}
	canvasData.context.translate(-aObject.pointSizeX/2,-aObject.pointSizeY/2);

	canvasData.context.fillStyle = aStyle;
	canvasData.context.strokeStyle = SELECT_COLOR;

	if(!aNoBaseRect) {
		//draw basic point
		canvasData.context.fillRect(0, 0, aObject.pointSizeX, aObject.pointSizeY);
	}

	if(aFunction) {
		aFunction.call(aObject);
	}

	//reset back
	canvasData.context.translate(aObject.pointSizeX/2,aObject.pointSizeY/2);
	if(aObject.rotation !== undefined) {
		canvasData.context.rotate(-toRadians(aObject.rotation));
	}
	canvasData.context.translate(-aObject.xPos, -aObject.yPos);
}

class RotatedStandardBigPoint extends RotatedStandardPoint {
	xsize = 0;
	ysize = 0;

	pointSizeX = undefined;
	pointSizeY = undefined;

	constructor(axPos = 0, ayPos = 0, aRotation = 0) {
		super(axPos, ayPos, aRotation);
		this.xsize = 0;
		this.ysize = 0;
	}
};

function abstractDrawBigPoint(aStyle, aObject, aFunction = null) {
	if(!aObject.willRender()) { return; }

	canvasData.context.translate(aObject.xPos, aObject.yPos);
	canvasData.context.rotate(toRadians(aObject.rotation));
	canvasData.context.translate(-aObject.xsize/2, -aObject.ysize/2);

	canvasData.context.fillStyle = aStyle;
	canvasData.context.strokeStyle = SELECT_COLOR;

	aFunction.call(aObject);

	//reset back
	canvasData.context.translate(aObject.xsize/2, aObject.ysize/2);
	canvasData.context.rotate(-toRadians(aObject.rotation));
	canvasData.context.translate(-aObject.xPos, -aObject.yPos);
}

//Input adding utils

function addHiddenInput(aID, aValue, aElement = canvasData.edit) {
	let element = document.createElement("input");
	element.type = "hidden";
	element.id = aID;
	element.name = aID;
	element.setAttribute("value", aValue);
	aElement.appendChild(element);
}

function addInput(aID, aValue, aType, aElement = canvasData.edit) {
	let element = document.createElement("input");
	element.type = aType;
	element.id = aID;
	element.name = aID;
	element.setAttribute("value", aValue);
	aElement.appendChild(element);
	aElement.appendChild(document.createElement("br"));
}

function addInputPlaceholder(aID, aValue, aType, aplaceholder, aElement = canvasData.edit) {
	let element = document.createElement("input");
	element.type = aType;
	element.id = aID;
	element.name = aID;
	element.placeholder = aplaceholder;
	element.setAttribute("value", aValue);
	aElement.appendChild(element);
	aElement.appendChild(document.createElement("br"));
}

function addInputCheckbox(aID, achecked, aElement = canvasData.edit) {
	let element = document.createElement("input");
	element.type = "checkbox";
	element.id = aID;
	element.name = aID;
	if(achecked) {
		element.setAttribute("checked", "");
	}
	aElement.appendChild(element);
	aElement.appendChild(document.createElement("br"));
}

function addHiddenIdInput(aID, aElement = canvasData.edit) {
	let idinput = document.createElement("input");
	idinput.type = "hidden";
	idinput.id = "idinput";
	idinput.setAttribute("value", aID);
	aElement.appendChild(idinput);
}

function addBasicEditInputs(aObject, aElement = canvasData.edit) {
	let editxinput = document.createElement("input");
	editxinput.type = "number";
	editxinput.id = "editxinput";
	editxinput.name = editxinput.id;
	editxinput.setAttribute("value", aObject.xPos);

	aElement.appendChild(document.createTextNode("X: "));
	aElement.appendChild(editxinput);
	aElement.appendChild(document.createElement("br"));
	
	let edityinput = document.createElement("input");
	edityinput.type = "number";
	edityinput.id = "edityinput";
	edityinput.name = edityinput.id;
	edityinput.setAttribute("value", aObject.yPos);
	
	aElement.appendChild(document.createTextNode("Y: "));
	aElement.appendChild(edityinput);
	aElement.appendChild(document.createElement("br"));

	let editheightinput = document.createElement("input");
	editheightinput.type = "number";
	editheightinput.id = "editheightinput";
	editheightinput.name = editheightinput.id;
	editheightinput.setAttribute("value", aObject.height);
	
	aElement.appendChild(document.createTextNode("Height: "));
	aElement.appendChild(editheightinput);
	aElement.appendChild(document.createElement("br"));

	let editcodeinput = document.createElement("input");
	editcodeinput.type = "text";
	editcodeinput.id = "editcodeinput";
	editcodeinput.name = editcodeinput.id;
	editcodeinput.placeholder = "XXXX";
	editcodeinput.maxLength = 4;
	editcodeinput.setAttribute("value", aObject.stationCode);

	aElement.appendChild(document.createTextNode("Station code: "));
	aElement.appendChild(editcodeinput);
	aElement.appendChild(document.createElement("br"));
}

function getDataFromBasicInputs(aObject) {
	aObject.xPos = Number(document.getElementById("editxinput").value);
	aObject.yPos = Number(document.getElementById("edityinput").value);
	aObject.height = Number(document.getElementById("editheightinput").value);
	aObject.stationCode = String(document.getElementById("editcodeinput").value);
}

function addTrackEditInputs(aObject, aID, aElement = canvasData.edit) {
	let df = new DocumentFragment();

	df.appendChild(document.createTextNode("Bezier: "));

	let editbezinput = document.createElement("input");
	editbezinput.type = "checkbox";
	editbezinput.id = "editbezinput";
	editbezinput.name = editbezinput.id;
	if(aObject.bezier) {
		editbezinput.setAttribute("checked", "");
	}
	df.appendChild(editbezinput);
	df.appendChild(document.createElement("hr"));

	let em = document.createElement("em");
	em.textContent = "Control point values have no effect if Bezier curves are disabled.";
	df.appendChild(em);

	df.appendChild(document.createElement("hr"));

	df.appendChild(document.createTextNode("CP1;X: "));
	let cp1x = document.createElement("input");
	cp1x.type = "number";
	cp1x.id = "editcp1xinput";
	cp1x.name = cp1x.id;
	cp1x.setAttribute("value", aObject.controlPoint1.x);
	df.appendChild(cp1x);
	df.appendChild(document.createElement("br"));

	df.appendChild(document.createTextNode("CP1;Y: "));
	let cp1y = document.createElement("input");
	cp1y.type = "number";
	cp1y.id = "editcp1yinput";
	cp1y.name = cp1y.id;
	cp1y.setAttribute("value", aObject.controlPoint1.y);
	df.appendChild(cp1y);
	df.appendChild(document.createElement("br"));

	df.appendChild(document.createTextNode("CP2;X: "));
	let cp2x = document.createElement("input");
	cp2x.type = "number";
	cp2x.id = "editcp2xinput";
	cp2x.name = cp2x.id;
	cp2x.setAttribute("value", aObject.controlPoint2.x);
	df.appendChild(cp2x);
	df.appendChild(document.createElement("br"));

	df.appendChild(document.createTextNode("CP2;Y: "));
	let cp2y = document.createElement("input");
	cp2y.type = "number";
	cp2y.id = "editcp2yinput";
	cp2y.name = cp2y.id;
	cp2y.setAttribute("value", aObject.controlPoint2.y);
	df.appendChild(cp2y);
	df.appendChild(document.createElement("br"));

	let btnCP2CP1 = document.createElement("button");
	btnCP2CP1.setAttribute("onclick", "trackEditMoveCP2toCP1("+aID+")");
	btnCP2CP1.textContent = "Move CP2 to CP1";
	df.appendChild(btnCP2CP1);
	df.appendChild(document.createElement("br"));

	let btnHeight = document.createElement("button");
	btnHeight.setAttribute("onclick", "trackEditRecalcHeight("+aID+")");
	btnHeight.textContent = "Recalculate height points";
	df.appendChild(btnHeight);
	df.appendChild(document.createElement("br"));

	let btnRecalc = document.createElement("button");
	btnHeight.setAttribute("onclick", "trackEditRecalcCP("+aID+")");
	btnRecalc.textContent = "Reset control points";
	df.appendChild(btnRecalc);
	df.appendChild(document.createElement("br"));

	df.appendChild(document.createElement("hr"));

	let h4 = document.createElement("h4");
	h4.textContent = "Edit heightpoints manually";
	df.appendChild(h4);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		df.appendChild(document.createTextNode("HgPt"+i+": "));

		let hpinput = document.createElement("input");
		hpinput.type = "number";
		hpinput.id = "edithght"+i;
		hpinput.name = hpinput.id;
		hpinput.setAttribute("value", aObject.heightpoints[i]);
		df.appendChild(hpinput);
		df.appendChild(document.createElement("br"));
	}

	df.appendChild(document.createElement("hr"));

	aElement.append(df);
}

function getDataFromTrackInputs(aObject) {
	aObject.bezier = document.getElementById("editbezinput").checked;
	aObject.controlPoint1.x = Number(document.getElementById("editcp1xinput").value);
	aObject.controlPoint1.y = Number(document.getElementById("editcp1yinput").value);
	aObject.controlPoint2.x = Number(document.getElementById("editcp2xinput").value);
	aObject.controlPoint2.y = Number(document.getElementById("editcp2yinput").value);

	for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
		aObject.heightpoints[i] = Number(document.getElementById("edithght"+i).value);
	}
}

//Button adding utils



//File reading utils

function readBytesAsNumber(aarrayref, abytes) {
	let number = aarrayref.value.slice(0, abytes);

	//add two's complement rule
	//blank first bit, subtract 2^(abytes*8) at end
	let hastwos = (number[number.length - 1] & 0b10000000) != 0;
	if(hastwos) {
		number[number.length - 1] &= 0b01111111;
	}

	number = number.map((v) => {
		return Number(v).toString(16).padStart(2, '0');
	});

	number.reverse(); //convert from little endian encoding
	number = parseInt(number.join(''), 16);

	if(hastwos) {
		number -= Math.pow(2, (abytes*8)-1);
	}

	aarrayref.value = aarrayref.value.slice(abytes);

	return number;
}

function disregardBytes(aarrayref, abytes) {
	aarrayref.value = aarrayref.value.slice(abytes);
}

function readBytesAsString(aarrayref, abytes) {
	let string = aarrayref.value.slice(0, abytes);
	string = string.filter((v) => {
		return v != 0;
	})
	aarrayref.value = aarrayref.value.slice(abytes);
	return String.fromCodePoint(...string);
}

//reads until null terminator found
function readNullTerminatedString(aarrayref) {
	let ntid = aarrayref.value.indexOf(0);
	if(ntid <= 0) { 
		aarrayref.value = aarrayref.value.slice(ntid+1);
		return "";
	} //empty string or no string

	let string = aarrayref.value.slice(0, ntid);

	string = String.fromCodePoint(...string);

	aarrayref.value = aarrayref.value.slice(ntid+1); //to include NT
	return string;
}

//X, Y position + height, writes values to object
function readXYH(aarrayref, object) {
	object.xPos = readBytesAsNumber(aarrayref, 4); //32bit
	object.yPos = readBytesAsNumber(aarrayref, 4); //32bit
	object.height = readBytesAsNumber(aarrayref, 4); //32bit
}

//read X,Y,H + rotation, writes values to object
function readXYHR(aarrayref, object) {
	readXYH(aarrayref, object);
	object.rotation = readBytesAsNumber(aarrayref, 2); //16bit rotation
}

function readStationCode(aarrayref) {
	return readBytesAsString(aarrayref, 4);
}