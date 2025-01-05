let statInfo = {
	units: null,

	node: null,
	switch: null,
	radio: null,
	track: null,
	build: null,
	tree: null,
	light: null,
	landmark: null,
	pillar: null,
	signal: null,
	swsignal: null,
	wall: null,
};

//TODO also update units per meter

function statUpdate() {
	statInfo.units = UNITS_PER_METER;
	statInfo.node.innerHTML = nodeList.length;
	statInfo.switch.innerHTML = switchList.length;
	statInfo.radio.innerHTML = radioList.length;
	statInfo.track.innerHTML = trackList.length;
	statInfo.build.innerHTML = buildingList.length;
	statInfo.tree.innerHTML = treeList.length;
	statInfo.light.innerHTML = lightList.length;
	statInfo.landmark.innerHTML = landmarkList.length;
	statInfo.pillar.innerHTML = stationPillarList.length;
	statInfo.signal.innerHTML = signalList.length;
	statInfo.swsignal.innerHTML = switchSignalList.length;
	statInfo.wall.innerHTML = wallList.length;

	document.getElementById("permeter").innerHTML = UNITS_PER_METER;
}

function statInit() {
	statInfo.units = UNITS_PER_METER;
	statInfo.node = document.getElementById("amountnode");
	statInfo.switch = document.getElementById("amountswitch");
	statInfo.radio = document.getElementById("amountradio");
	statInfo.track = document.getElementById("amounttrack");
	statInfo.build = document.getElementById("amountbuild");
	statInfo.tree = document.getElementById("amounttree");
	statInfo.light = document.getElementById("amountlight");
	statInfo.landmark = document.getElementById("amountlandmark");
	statInfo.pillar = document.getElementById("amountpillar");
	statInfo.signal = document.getElementById("amountsignal");
	statInfo.swsignal = document.getElementById("amountswsignal");
	statInfo.wall = document.getElementById("amountwall");

	window.setInterval(statUpdate, 2000);
}