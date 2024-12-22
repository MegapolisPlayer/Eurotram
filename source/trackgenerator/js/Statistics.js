let statInfo = {
	node: null,
	radio: null,
	track: null,
	build: null,
	tree: null,
	light: null,
	landmark: null
};

function statUpdate() {
	statInfo.node.innerHTML = nodeList.length;
	statInfo.radio.innerHTML = radioList.length;
	statInfo.track.innerHTML = trackList.length;
	statInfo.build.innerHTML = buildingList.length;
	statInfo.tree.innerHTML = treeList.length;
	statInfo.light.innerHTML = lightList.length;
	statInfo.landmark.innerHTML = landmarkList.length;
}

function statInit() {
	statInfo.node = document.getElementById("amountnode");
	statInfo.radio = document.getElementById("amountradio");
	statInfo.track = document.getElementById("amounttrack");
	statInfo.build = document.getElementById("amountbuild");
	statInfo.tree = document.getElementById("amounttree");
	statInfo.light = document.getElementById("amountlight");
	statInfo.landmark = document.getElementById("amountlandmark");

	window.setInterval(statUpdate, 2000);
}