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
	permeter: null,
};

function statUpdate() {
	statInfo.node.textContent = nodeList.length;
	statInfo.switch.textContent = switchList.length;
	statInfo.radio.textContent = radioList.length;
	statInfo.track.textContent = trackList.length;
	statInfo.build.textContent = buildingList.length;
	statInfo.tree.textContent = treeList.length;
	statInfo.light.textContent = lightList.length;
	statInfo.landmark.textContent = landmarkList.length;
	statInfo.pillar.textContent = stationPillarList.length;
	statInfo.signal.textContent = signalList.length;
	statInfo.swsignal.textContent = switchSignalList.length;
	statInfo.wall.textContent = wallList.length;

	statInfo.units = UNITS_PER_METER;
	statInfo.permeter.textContent = UNITS_PER_METER;
}

function statInit() {
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

	statInfo.units = UNITS_PER_METER;
	statInfo.permeter = document.getElementById("permeter");

	statUpdate();
	window.setInterval(statUpdate, 2000);
}