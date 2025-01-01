function scenarioClear() {
	nodeList.length = 0;
	radioList.length = 0;
	trackList.length = 0;
	buildingList.length = 0;
	treeList.length = 0;
	lightList.length = 0;
	landmarkList.length = 0;

	canvasClear();
}

//point in timetable which the tram must follow
//can be different for each line
//time as second offset from beginning of scenario
function ControlPoint(acode, atime) {
	this.point = acode;
	this.time = atime;
	return this;
}

class Line {
	stationCodes = [];
	controlpoints = [];
	switches = []; //how must switches be set

	loops = 0; //loops of line

	startStation = []; //start station

	maintenanceRouteStart = []; //if starting at depot
	maintenanceRouteEnd = []; //go to depot at end

	constructor() {

	}
};

let lineList = [];

let fileFormatVersion = 0;

// utils

function clamp(anum, min, max) {
	return (anum < min) ? min : ((anum > max) ? max : anum);
}

function numberToByteArray(anum, bits) {
	let bytes = Math.trunc(bits/8);

	let byteArray = new Array(bytes).fill(0);

	let binary = BigInt(anum).toString(16).padStart(bytes*2, '0');
	console.log(binary);

	//flip to little endian
	for(let i = 0; i < byteArray.length; i++) {
		byteArray[i] = Number("0x"+binary[binary.length-1-1-2*i]+binary[binary.length-1-2*i]);
	}

	console.log(byteArray);

	return byteArray;
}

//8-bit ints
function numberToByte(anum) {
	return clamp(Math.trunc(anum), -128, 127);
}

// funcs

function scenarioNewLine() {

}

function scenarioSerialize() {
	let numberValuesArray = [];

	//file signature
	numberValuesArray.push(...("ETSC".split('').map((v) => { return v.charCodeAt(0); })));

	console.log(numberValuesArray);

	//header numbers

	numberValuesArray.push(...numberToByteArray(fileFormatVersion, 16)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 64)); //D - unix time in ms
	
	console.log(numberValuesArray);

	//N - node+switch amount
	numberValuesArray.push(...numberToByteArray(nodeList.length, 64));

	//T - track amount
	numberValuesArray.push(...numberToByteArray(trackList.length, 64));

	//W - sw signal amount
	numberValuesArray.push(...numberToByteArray(switchSignalList.length, 64));

	//S - signal amount
	numberValuesArray.push(...numberToByteArray(signalList.length, 64));

	//R - radioboxes amount
	numberValuesArray.push(...numberToByteArray(radioList.length, 64));

	//P - station pillars amount
	numberValuesArray.push(...numberToByteArray(stationPillarList.length, 64));

	//L - lampposts amount
	numberValuesArray.push(...numberToByteArray(lightList.length, 64));

	//G - trees amount
	numberValuesArray.push(...numberToByteArray(treeList.length, 64));

	//B - buildings amount
	numberValuesArray.push(...numberToByteArray(buildingList.length, 64));

	//M - monuments/landmarks amount
	numberValuesArray.push(...numberToByteArray(landmarkList.length, 64));

	//X - texparcels amount
	numberValuesArray.push(...numberToByteArray(texparcelList.length, 64));

	//J - scenario name
	numberValuesArray.push(...document.getElementById("scenarioname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//A - scenario author
	numberValuesArray.push(...document.getElementById("authorname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//E - units per meter
	numberValuesArray.push(...numberToByteArray(UNITS_PER_METER, 8));

	numberValuesArray.push(0b11111111);

	//serialize lists

	//nodes
	let nodeBytes = "PT".split('').map((v) => { return v.charCodeAt(0); });

	nodeList.forEach((v) => {
		//TODO test
		numberValuesArray.push(...nodeBytes);
		numberValuesArray.push(...numberToByteArray(v.xpos, 64));
		numberValuesArray.push(...numberToByteArray(v.ypos, 64));
		numberValuesArray.push(...numberToByteArray(v.height, 64));
		numberValuesArray.push((v.statiooCode.split('').map((v) => { return v.charCodeAt(0); })));
		numberValuesArray.push(0b11111111);
	});

	//TODO check if switch doesnt self-reference
	//TODO trunate values
	//convert to blob

	let binaryArray = new Uint8Array(numberValuesArray.length);
	for(let i = 0; i < binaryArray.length; i++) {
		binaryArray[i] = numberValuesArray[i];
	}

	console.log(binaryArray);

	//blobs are immutable
	let blob = new Blob([binaryArray], {type: "application/octet-stream"}); //arbitrary binary data

	return blob;
}

function scenarionDeserialize() {

}

function lineSerialize() {
	
}