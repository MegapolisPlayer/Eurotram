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
	let byteArray = new Array(Math.trunc(bits/8)).fill(0);

	let binary = BigInt(anum).toString(2);

	if(binary[0] == '-') {
		binary = binary.split(''); //split here so we can use padStart in second branch
		binary[0] = 0; //delete minus
		binary = binary.map((v) => { return Number(v); }); //remove chars
		binary = [...new Array(bits-byteArray.length).fill(0), ...binary];
		binary[0] = 1; //twos complement
	}
	else {
		binary = binary.padStart(bits, "0").split('');
	}
	
	console.log(binary);

	for(let i = 0; i < bits; i++) {
		if(binary[i] == '1') {
			byteArray[Math.trunc(i/8)] |= (1 << 7-(i%8));
		}
	}

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
	//TODO check if switch doesnt self-reference
	//TODO check for erroneous input

	let numberValuesArray = [];

	//file signature
	numberValuesArray.push("ETSC".split('').map((v, i) => {
		return v.charCodeAt(0);
	}));

	//header numbers

	numberValuesArray.push(numberToByteArray(fileFormatVersion, 16)); //V
	numberValuesArray.push(numberToByteArray(Date.now(), 64)); //D - unix time in ms
	
	//N - node+switch amount

	//T - track amount

	//W - sw signal amount

	//S - signal amount

	//R - radioboxes amount

	//P - station pillars amount

	//L - lampposts amount

	//G - trees amount

	//B - buildings amount

	//M - monuments/landmarks amount

	//X - texparcels amount

	//J - scenario name
	numberValuesArray.push(0); //null terminator

	//A - scenario author
	numberValuesArray.push(0); //null terminator

	//E - units per meter


	numberValuesArray.push(0b11111111);

	console.log(numberValuesArray);

	//serialize lists

	let binaryArray = new Uint8Array(numberValuesArray.length);

	//blobs are immutable
	let blob = new Blob(binaryArray, {type: "application/octet-stream"}); //arbitrary binary data

}

function scenarionDeserialize() {

}

function lineSerialize() {
	
}