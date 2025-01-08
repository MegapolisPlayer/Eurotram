function scenarioClear() {
	nodeList.length = 0;
	switchList.length = 0;
	radioList.length = 0;
	trackList.length = 0;
	buildingList.length = 0;
	treeList.length = 0;
	lightList.length = 0;
	landmarkList.length = 0;
	texparcelList.length = 0;
	wallList.length = 0;

	canvasClear();
}

// utils

function clamp(anum, min, max) {
	return (anum < min) ? min : ((anum > max) ? max : anum);
}

function numberToByteArray(anum, abytes) {
	let byteArray = new Array(abytes).fill(0);

	//faking unsigned bit-shift leads to correct translation - convert to two's complement
	//truncating to sanitize input
	let binary = (Math.trunc(anum) >>> 0).toString(16).padStart(abytes*2, '0');

	//flip to little endian - ignore if 1 byte
	byteArray.forEach((v, i, a) => {
		a[i] = Number("0x"+binary[binary.length-1-1-2*i]+binary[binary.length-1-2*i]);
	});

	return byteArray;
}

//8-bit ints
function numberToByte(anum) {
	return clamp(Math.trunc(anum), -128, 127);
}

function stationCodeToArray(astationcode) {
	let arr = astationcode.split('').map((v) => { return v.charCodeAt(0); });
	for(let i = arr.length; i < 4; i++) {
		arr.push(0);
	}
	return arr;
}

// funcs

const TRACK_TYPE = "TK";
const STATION_TRACK_TYPE = "ST";

const TRACK_BYTES = TRACK_TYPE.split('').map((v) => { return v.charCodeAt(0); });
const STATION_TRACK_BYTES = STATION_TRACK_TYPE.split('').map((v) => { return v.charCodeAt(0); });

const TRACK_FLAG_FIRST_SWITCH = 0b00000001;
const TRACK_FLAG_SECOND_SWITCH = 0b00000010;
const TRACK_FLAG_BEZIER = 0b00000100;

function scenarioSerialize() {
	let numberValuesArray = [];

	//file signature
	numberValuesArray.push(...("ETMP".split('').map((v) => { return v.charCodeAt(0); })));

	console.log(numberValuesArray);

	//header numbers

	numberValuesArray.push(...numberToByteArray(fileFormatVersion, 2)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 8)); //D - unix time in ms
	
	console.log(numberValuesArray);

	//N - node
	numberValuesArray.push(...numberToByteArray(nodeList.length, 4));

	//I - switch amount
	numberValuesArray.push(...numberToByteArray(nodeList.length, 4));

	//T - track amount
	numberValuesArray.push(...numberToByteArray(trackList.length, 4));

	//Y - sw signal amount
	numberValuesArray.push(...numberToByteArray(switchSignalList.length, 4));

	//S - signal amount
	numberValuesArray.push(...numberToByteArray(signalList.length, 4));

	//R - radioboxes amount
	numberValuesArray.push(...numberToByteArray(radioList.length, 4));

	//P - station pillars amount
	numberValuesArray.push(...numberToByteArray(stationPillarList.length, 4));

	//L - lampposts amount
	numberValuesArray.push(...numberToByteArray(lightList.length, 4));

	//G - trees amount
	numberValuesArray.push(...numberToByteArray(treeList.length, 4));

	//B - buildings amount
	numberValuesArray.push(...numberToByteArray(buildingList.length, 4));

	//M - monuments/landmarks amount
	numberValuesArray.push(...numberToByteArray(landmarkList.length, 4));

	//W - wall amount
	numberValuesArray.push(...numberToByteArray(wallList.length, 4));

	//X - texparcels amount
	numberValuesArray.push(...numberToByteArray(texparcelList.length, 4));

	//J - scenario name
	numberValuesArray.push(...document.getElementById("scenarioname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//A - scenario author
	numberValuesArray.push(...document.getElementById("authorname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//E - units per meter
	numberValuesArray.push(UNITS_PER_METER);

	//serialize lists

	//nodes
	nodeList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));

		numberValuesArray.push(...stationCodeToArray(v.stationCode));
		numberValuesArray.push(...stationCodeToArray(v.bordersWith));
	});

	//switch
	switchList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
		
		numberValuesArray.push(...numberToByteArray(v.beforeId, 4));
		numberValuesArray.push(...numberToByteArray(v.leftId, 4));
		numberValuesArray.push(...numberToByteArray(v.frontId, 4));
		numberValuesArray.push(...numberToByteArray(v.rightId, 4));
		numberValuesArray.push(...numberToByteArray(v.radioBoxId, 4));
		numberValuesArray.push(...numberToByteArray(v.signalId, 4));
		numberValuesArray.push(v.signalLetter.length == 0 ? " " : v.signalLetter.charCodeAt(0));
	});

	//tracks
	trackList.forEach((v) => {
		let isStation = v instanceof StationTrack;

		if(isStation) {
			numberValuesArray.push(...STATION_TRACK_BYTES);
		}
		else {
			numberValuesArray.push(...TRACK_BYTES);
		}

		numberValuesArray.push(...numberToByteArray(v.nodeIdFirst, 4));
		numberValuesArray.push(...numberToByteArray(v.nodeIdSecond, 4));

		let flagData = 0;
		if(v.firstIsSwitch) {
			flagData |= TRACK_FLAG_FIRST_SWITCH;
		}
		if(v.secondIsSwitch) {
			flagData |= TRACK_FLAG_SECOND_SWITCH;
		}
		if(v.bezier) {
			flagData |= TRACK_FLAG_BEZIER;
		}

		numberValuesArray.push(flagData);

		numberValuesArray.push(...numberToByteArray(v.controlPoint1.x, 4));
		numberValuesArray.push(...numberToByteArray(v.controlPoint1.y, 4));
		numberValuesArray.push(...numberToByteArray(v.controlPoint2.x, 4));
		numberValuesArray.push(...numberToByteArray(v.controlPoint2.y, 4));

		for(let i = 0; i < TRACK_HEIGHTPOINTS_AMOUNT; i++) {
			numberValuesArray.push(...numberToByteArray(Math.trunc(v.heightpoints[i])*100, 4))
		}

		if(isStation) {
			numberValuesArray.push(...stationCodeToArray(v.stationCode));
		}
	});

	//switch signals
	switchSignalList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));

		numberValuesArray.push(v.unitIds.length);
		v.unitIds.forEach((u) => {
			numberValuesArray.push(u.charCodeAt(0));
		});
	});

	//signals
	signalList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
		numberValuesArray.push(...numberToByteArray(v.poleHeight, 2));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//radioboxes
	radioList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//station pillars
	stationPillarList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//lightposts
	lightList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//trees
	treeList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//buildings
	buildingList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
		numberValuesArray.push(v.type);
		numberValuesArray.push(...stationCodeToArray(v.stationCode));
	});

	//landmarks
	landmarkList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.xpos, 4));
		numberValuesArray.push(...numberToByteArray(v.ypos, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 4));
		numberValuesArray.push(...numberToByteArray(v.rotation, 2));
	});

	//walls
	wallList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.x1, 4));
		numberValuesArray.push(...numberToByteArray(v.y1, 4));
		numberValuesArray.push(...numberToByteArray(v.h1, 4));
		numberValuesArray.push(...numberToByteArray(v.x2, 4));
		numberValuesArray.push(...numberToByteArray(v.y2, 4));
		numberValuesArray.push(...numberToByteArray(v.h2, 4));
		numberValuesArray.push(...numberToByteArray(v.height, 2));

		numberValuesArray.push(...stationCodeToArray(v.stationCode));

		numberValuesArray.push(...(v.materialName.split('').map((v) => { return v.charCodeAt(0); })));
		numberValuesArray.push(0); //null terminator
	});

	//texparcels
	texparcelList.forEach((v) => {
		numberValuesArray.push(...numberToByteArray(v.x1, 4));
		numberValuesArray.push(...numberToByteArray(v.x2, 4));
		numberValuesArray.push(...numberToByteArray(v.x3, 4));
		numberValuesArray.push(...numberToByteArray(v.x4, 4));

		numberValuesArray.push(...numberToByteArray(v.y1, 4));
		numberValuesArray.push(...numberToByteArray(v.y2, 4));
		numberValuesArray.push(...numberToByteArray(v.y3, 4));
		numberValuesArray.push(...numberToByteArray(v.y4, 4));
		
		numberValuesArray.push(...numberToByteArray(v.h1, 4));
		numberValuesArray.push(...numberToByteArray(v.h2, 4));
		numberValuesArray.push(...numberToByteArray(v.h3, 4));
		numberValuesArray.push(...numberToByteArray(v.h4, 4));

		numberValuesArray.push(...stationCodeToArray(v.stationCode1));
		numberValuesArray.push(...stationCodeToArray(v.stationCode2));

		numberValuesArray.push(...(v.materialName.split('').map((v) => { return v.charCodeAt(0); })));
		numberValuesArray.push(0); //null terminator
	});

	//convert to blob

	let binaryArray = new Uint8Array(numberValuesArray.length);
	binaryArray.forEach((v, i, a) => {
		a[i] = numberValuesArray[i];
	})

	//blobs are immutable
	let blob = new Blob([binaryArray], {type: "application/octet-stream"}); //arbitrary binary data

	return blob;
}

//utils

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
	object.xpos = readBytesAsNumber(aarrayref, 4); //32bit
	object.ypos = readBytesAsNumber(aarrayref, 4); //32bit
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

function scenarioDeserialize(afiledata) {
	let byteArray = new Uint8Array(afiledata);
	let numberArrayReference = {value:  Array.from(byteArray)};

	let head = readBytesAsString(numberArrayReference, 4); //remove ETSC
	if(head != "ETMP") {
		alert("File invalid - invalid head! ("+head+"/"+head.length+" != ETMP/4)");
		return;
	}

	let fmtver = readBytesAsNumber(numberArrayReference, 2);
	if(fmtver != fileFormatVersion) {
		alert("Wrong scenario format version - program supports version "+fileFormatVersion+" and file is of version "+fmtver);
		return;
	}

	//read date

	console.log("Scenario date: ", new Date(readBytesAsNumber(numberArrayReference, 8)*1000)); //argument in milliseconds

	//read amounts - 13 values
	
	let amounts = new Array(13).fill(0);
	
	amounts.forEach((v, i, a) => {
		a[i] = readBytesAsNumber(numberArrayReference, 4);
	});

	console.log(amounts);

	//read scenario and author name

	document.getElementById("scenarioname").setAttribute("value", readNullTerminatedString(numberArrayReference));
	document.getElementById("authorname").setAttribute("value", readNullTerminatedString(numberArrayReference));

	//read units per meters

	UNITS_PER_METER = readBytesAsNumber(numberArrayReference, 1);

	//nodes
	nodeList.length = 0;
	for(let i = 0; i < amounts[0]; i++) {
		nodeList.push(new TNode());
		readXYH(numberArrayReference, nodeList[i]);
		nodeList[i].stationCode = readStationCode(numberArrayReference);
		nodeList[i].bordersWith = readStationCode(numberArrayReference);
	}

	//switches
	switchList.length = 0;
	for(let i = 0; i < amounts[1]; i++) {
		switchList.push(new Switch());
		readXYH(numberArrayReference, switchList[i]);
		switchList[i].stationCode = readStationCode(numberArrayReference);
		switchList[i].beforeId = readBytesAsNumber(numberArrayReference, 4);
		switchList[i].leftId = readBytesAsNumber(numberArrayReference, 4);
		switchList[i].frontId = readBytesAsNumber(numberArrayReference, 4);
		switchList[i].rightId = readBytesAsNumber(numberArrayReference, 4);

		switchList[i].radioBoxId = readBytesAsNumber(numberArrayReference, 4);
		switchList[i].signalId = readBytesAsNumber(numberArrayReference, 4);
		switchList[i].signalLetter = readBytesAsString(numberArrayReference, 1);
	}

	//track (incl. station)
	trackList.length = 0;
	for(let i = 0; i < amounts[2]; i++) {
		let type = readBytesAsString(numberArrayReference, 2);

		if(type == TRACK_TYPE) {
			trackList.push(new Track());
		}
		else if (type == STATION_TRACK_TYPE) {
			trackList.push(new StationTrack());
		}
		else {
			console.log(numberArrayReference);
			alert("Invalid corrupt file - track type identificator invalid! ("+type+")");
			window.location.href = window.location.href; //reload webpage
			return;
		}

		trackList[i].nodeIdFirst = readBytesAsNumber(numberArrayReference, 4);
		trackList[i].nodeIdSecond = readBytesAsNumber(numberArrayReference, 4);

		let flags = readBytesAsNumber(numberArrayReference, 1);

		trackList[i].firstIsSwitch = (flags & TRACK_FLAG_FIRST_SWITCH) != 0;
		trackList[i].secondIsSwitch = (flags & TRACK_FLAG_SECOND_SWITCH) != 0;
		trackList[i].bezier = (flags & TRACK_FLAG_BEZIER) != 0;

		trackList[i].controlPoint1.x = readBytesAsNumber(numberArrayReference, 4);
		trackList[i].controlPoint1.y = readBytesAsNumber(numberArrayReference, 4);
		trackList[i].controlPoint2.x = readBytesAsNumber(numberArrayReference, 4);
		trackList[i].controlPoint2.y = readBytesAsNumber(numberArrayReference, 4);

		for(let j = 0; j < TRACK_HEIGHTPOINTS_AMOUNT; j++) {
			trackList[i].heightpoints[j] = readBytesAsNumber(numberArrayReference, 4)/100.0;
		}

		if(type == STATION_TRACK_TYPE) {
			trackList[i].stationCode = readStationCode(numberArrayReference);
		}
	}

	//switch signals
	switchSignalList.length = 0;
	for(let i = 0; i < amounts[3]; i++) {
		switchSignalList.push(new SwitchStateSignal());
		readXYHR(numberArrayReference, switchSignalList[i]);
		switchSignalList[i].stationCode = readStationCode(numberArrayReference);
		let n = readBytesAsNumber(numberArrayReference, 1);
		for(let j = 0; j < n; j++) {
			switchSignalList[i].unitIds.push(readBytesAsString(numberArrayReference, 1));
		}
	}

	//signals
	signalList.length = 0;
	for(let i = 0; i < amounts[4]; i++) {
		signalList.push(new Signal());
		readXYHR(numberArrayReference, signalList[i]);
		signalList[i].poleHeight = readBytesAsNumber(numberArrayReference, 2);
		signalList[i].stationCode = readStationCode(numberArrayReference);
	}

	//radioboxes
	radioList.length = 0;
	for(let i = 0; i < amounts[5]; i++) {
		radioList.push(new RadioBox());
		readXYH(numberArrayReference, radioList[i]);
		radioList[i].stationCode = readStationCode(numberArrayReference);
	}

	//station pillars
	stationPillarList.length = 0;
	for(let i = 0; i < amounts[6]; i++) {
		stationPillarList.push(new StationPillar());
		readXYHR(numberArrayReference, stationPillarList[i]);
		stationPillarList[i].stationCode = readStationCode(numberArrayReference);
	}

	//lampposts
	lightList.length = 0;
	for(let i = 0; i < amounts[7]; i++) {
		lightList.push(new Light());
		readXYHR(numberArrayReference, lightList[i]);
		lightList[i].stationCode = readStationCode(numberArrayReference);
	}
	
	//trees
	treeList.length = 0;
	for(let i = 0; i < amounts[8]; i++) {
		treeList.push(new Tree());
		readXYH(numberArrayReference, treeList[i]);
		treeList[i].stationCode = readStationCode(numberArrayReference);
	}

	//buildings
	buildingList.length = 0;
	for(let i = 0; i < amounts[9]; i++) {
		buildingList.push(new Building());
		readXYHR(numberArrayReference, buildingList[i]);
		buildingList[i].type = readBytesAsNumber(numberArrayReference, 1);
		buildingList[i].stationCode = readStationCode(numberArrayReference);
		buildingList[i].updateFromType();
	}

	//landmarks
	landmarkList.length = 0;
	for(let i = 0; i < amounts[10]; i++) {
		landmarkList.push(new Landmark());
		readXYHR(numberArrayReference, landmarkList[i]);
		landmarkList[i].type = readBytesAsNumber(numberArrayReference, 2);
		landmarkList[i].updateFromLandmarkCode();
	}

	//walls
	wallList.length = 0;
	for(let i = 0; i < amounts[11]; i++) {
		wallList.push(new Wall());
		wallList[i].x1 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].y1 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].h1 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].x2 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].y2 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].h2 = readBytesAsNumber(numberArrayReference, 4);
		wallList[i].height = readBytesAsNumber(numberArrayReference, 2);

		wallList[i].stationCode = readStationCode(numberArrayReference);
		wallList[i].materialName = readNullTerminatedString(numberArrayReference);
	}

	//texparcels
	texparcelList.length = 0;
	for(let i = 0; i < amounts[12]; i++) {
		texparcelList.push(new Texparcel());
		texparcelList[i].x1 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].x2 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].x3 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].x4 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].y1 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].y2 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].y3 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].y4 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].h1 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].h2 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].h3 = readBytesAsNumber(numberArrayReference, 4);
		texparcelList[i].h4 = readBytesAsNumber(numberArrayReference, 4);

		texparcelList[i].stationCode1 = readStationCode(numberArrayReference);
		texparcelList[i].stationCode2 = readStationCode(numberArrayReference);
		texparcelList[i].materialName = readNullTerminatedString(numberArrayReference);
	}

	canvasPosReset(); //reset to center
	canvasRedraw(); //redraw canvas
}
