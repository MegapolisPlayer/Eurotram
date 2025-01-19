//utils

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

function stationCodeToArray(astationcode) {
	let arr = astationcode.split('').map((v) => { return v.charCodeAt(0); });
	for(let i = arr.length; i < 4; i++) {
		arr.push(0);
	}
	return arr;
}

const ANNUCIATOR_FILE_FORMAT_VERSION  = 0;

const STATION_ENTRY_METRO_TRANSFER    = 0b00000001;
const STATION_ENTRY_TRAIN_TRANSFER    = 0b00000010;
const STATION_ENTRY_FUNICULAR_CLOSURE = 0b00000100;
const STATION_ENTRY_ON_REQUEST        = 0b00001000;

function serialize() {
	let numberValuesArray = [];

	numberValuesArray.push(...("ETAN".split('').map((v) => { return v.charCodeAt(0); })));

	numberValuesArray.push(...numberToByteArray(ANNUCIATOR_FILE_FORMAT_VERSION, 2)); //V
	numberValuesArray.push(...numberToByteArray(Math.trunc(Date.now()/1000), 8)); //D - unix time in ms

	//J - scenario name
	numberValuesArray.push(...document.getElementById("annunciatorname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//A - scenario author
	numberValuesArray.push(...document.getElementById("authorname").value.split('').map((v) => { return v.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//P - line prefix
	numberValuesArray.push(...document.getElementById("lineprefix").value.split('').map((u) => { return u.charCodeAt(0); }));
	numberValuesArray.push(0); //null terminator

	//K
	let lines = document.getElementById("lines").value.split(',').filter(v => v.length > 0);
	numberValuesArray.push(lines.length);

	//L...L
	lines.forEach(v => numberValuesArray.push(v));

	//M
	let specialLines = document.getElementById("speciallines").value.split(',').filter(v => v.length > 0);
	numberValuesArray.push(specialLines.length);

	//N...N
	specialLines.forEach(v => {
		numberValuesArray.push(...v.split('').map((v) => { return v.charCodeAt(0); }));
		numberValuesArray.push(0); //null terminator
	});

	//Z - station amount
	let stations = document.querySelectorAll(".element");
	numberValuesArray.push(...numberToByteArray(stations.length, 4));

	//base files
	for(let i = 0; i < baseNames.length; i++) {
		numberValuesArray.push(...document.getElementById("b"+String(i)).value.split('').map((v) => { return v.charCodeAt(0); }));
		numberValuesArray.push(0); //null terminator
	}

	let stationCodes = document.querySelectorAll(".scode");
	let stationNames = document.querySelectorAll(".sname");
	let stationLines = document.querySelectorAll(".slines");
	let stationSpecialLines = document.querySelectorAll(".ssplines");
	let stationFilenames = document.querySelectorAll(".sfile");
	let stationOnRequest = document.querySelectorAll(".srequest");
	let stationMetroTransfer = document.querySelectorAll(".smetro");
	let stationRailTransfer = document.querySelectorAll(".srail");
	let stationFunicular = document.querySelectorAll(".sfunicular");

	stations.forEach((v, i, a) => {
		//for each station

		numberValuesArray.push(...stationCodeToArray(stationCodes[i].value)); //SSSS

		//N - name of stop
		numberValuesArray.push(...(stationNames[i].value.split('').map((u) => { return u.charCodeAt(0); })));
		numberValuesArray.push(0); //null terminator

		//A - filename
		numberValuesArray.push(...(stationFilenames[i].value.split('').map((u) => { return u.charCodeAt(0); })));
		numberValuesArray.push(0); //null terminator

		let lines = stationLines[i].value.split(',').map(u => Number(u)).filter(u => u != NaN && u != 0);

		//K - amount of lines - split by comma
		numberValuesArray.push(lines.length);

		//L - lines
		lines.forEach(u => {
			numberValuesArray.push(u);
		});

		let specialLines = stationSpecialLines[i].value.split(',').filter(v => v.length > 0);

		//M - amount of special lines
		numberValuesArray.push(specialLines.length);

		//N - special lines
		specialLines.forEach(u => {
			numberValuesArray.push(...(u.split('').map((t) => { return t.charCodeAt(0); })));
			numberValuesArray.push(0); //null terminator
		})

		//F - flags
		let flagValue = 0;

		if(stationOnRequest[i].checked) {
			flagValue |= STATION_ENTRY_ON_REQUEST;
		}
		if(stationMetroTransfer[i].checked) {
			flagValue |= STATION_ENTRY_METRO_TRANSFER;
		}
		if(stationRailTransfer[i].checked) {
			flagValue |= STATION_ENTRY_TRAIN_TRANSFER;
		}
		if(stationFunicular[i].checked) {
			flagValue |= STATION_ENTRY_FUNICULAR_CLOSURE;
		}
		
		numberValuesArray.push(flagValue);
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

function readBytesAsString(aarrayref, abytes) {
	let string = aarrayref.value.slice(0, abytes);
	string = string.filter((v) => {
		return v != 0;
	})
	aarrayref.value = aarrayref.value.slice(abytes);
	return String.fromCodePoint(...string);
}

function readStationCode(aarrayref) {
	return readBytesAsString(aarrayref, 4);
}

function deserialize(afiledata) {
	let byteArray = new Uint8Array(afiledata);
	let numberArrayReference = {value:  Array.from(byteArray)};

	let head = readBytesAsString(numberArrayReference, 4); //remove ETSC
	if(head != "ETAN") {
		alert("File invalid - invalid head! ("+head+"/"+head.length+" != ETAN/4)");
		return;
	}

	let fmtver = readBytesAsNumber(numberArrayReference, 2);
	if(fmtver != ANNUCIATOR_FILE_FORMAT_VERSION) {
		alert("Wrong annunciator format version - program supports version "+ANNUCIATOR_FILE_FORMAT_VERSION+" and file is of version "+fmtver);
		return;
	}

	console.log("Annunciator date: ", new Date(readBytesAsNumber(numberArrayReference, 8)*1000)); //argument in milliseconds

	//name
	document.getElementById("annunciatorname").value = readNullTerminatedString(numberArrayReference);
	//author
	document.getElementById("authorname").value = readNullTerminatedString(numberArrayReference);
	//line prefix
	document.getElementById("lineprefix").value = readNullTerminatedString(numberArrayReference);

	//lines
	let amountLines = readBytesAsNumber(numberArrayReference, 1);
	let string = "";
	for(let i = 0; i < amountLines; i++) {
		string += String(readBytesAsNumber(numberArrayReference, 1))+",";
	}
	document.getElementById("lines").value = string;

	//special lines
	let amountSpecialLines = readBytesAsNumber(numberArrayReference, 1);
	string = "";
	for(let i = 0; i < amountSpecialLines; i++) {
		string += readNullTerminatedString(numberArrayReference)+",";
	}
	document.getElementById("speciallines").value = string;

	//amount of stations
	let amountStops = readBytesAsNumber(numberArrayReference, 4);

	//base
	for(let i = 0; i < baseNames.length; i++) {
		document.getElementById("b"+String(i)).value = readNullTerminatedString(numberArrayReference);
	}

	resetElements();

	//stations
	for(let i = 0; i < amountStops; i++) {
		let elems = newElement();
		elems[0].value = readStationCode(numberArrayReference);
		elems[1].value = readNullTerminatedString(numberArrayReference);
		elems[2].value = readNullTerminatedString(numberArrayReference);

		let lineAmount =  readBytesAsNumber(numberArrayReference, 1);
		for(let j = 0; j < lineAmount; j++) {
			elems[3].value += (String(readBytesAsNumber(numberArrayReference, 1)) + ","); 
		}

		let specialLineAmount =  readBytesAsNumber(numberArrayReference, 1);
		for(let j = 0; j < specialLineAmount; j++) {
			elems[4].value += (readNullTerminatedString(numberArrayReference) + ","); 
		}

		let flags = readBytesAsNumber(numberArrayReference, 1);
		elems[5].checked = (flags & STATION_ENTRY_ON_REQUEST) > 0;
		elems[6].checked = (flags & STATION_ENTRY_METRO_TRANSFER) > 0;
		elems[7].checked = (flags & STATION_ENTRY_TRAIN_TRANSFER) > 0;
		elems[8].checked = (flags & STATION_ENTRY_FUNICULAR_CLOSURE) > 0;
	}
}

//actual saving and loading

function fileLoad(aFilename) {
	console.log("Loading annunciator");

	let input = document.createElement("input");
	input.id = "fileupload";
	input.type = "file";
	input.accept = ".etanc";

	input.addEventListener("change", (e) => {
		let fileread = new FileReader();
		fileread.addEventListener("loadend", (e) => {
			deserialize(e.target.result);
		});
		fileread.readAsArrayBuffer(input.files[0]);
	});

	input.click();
}

function fileSave() {
	console.log("Saving annunciator");

	let scenario = serialize();

	console.log(scenario);

	let url = URL.createObjectURL(scenario);
	let elem = document.createElement("a");
	elem.href = url;
	let name = document.getElementById("annunciatorname").value;
	elem.download = ((name.length == 0) ? "annunciator" : name)+".etanc";
	elem.click();

	URL.revokeObjectURL(url);
}