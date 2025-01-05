let mapLayer = null;
let mapLayerBounds = null;
let mapInitial = [50.0778, 14.467];
let mapCenter = [50.0778, 14.467];

let mapShiftX = 0;
let mapShiftY = 0;

function mapLayerUpdate(adx = 0, ady = 0) {
	let p = mapLayer.latLngToContainerPoint(mapCenter);
	//invert since if we move left map moves right

	p.x -= adx;
	p.y -= ady;

	let ll = mapLayer.containerPointToLatLng(p);
	mapCenter[0] = ll.lat;
	mapCenter[1] = ll.lng;

	mapLayerBounds = L.latLng(ll).toBounds(
		canvasData.element.width*(1.0/canvasData.scale)/UNITS_PER_METER
	);

	mapLayer.fitBounds(mapLayerBounds);
}

function mapLayerReset() {
	mapShiftX = 0;
	mapShiftY = 0;

	mapCenter[0] = mapInitial[0]
	mapCenter[1] = mapInitial[1];

	mapLayerBounds = L.latLng(...mapInitial).toBounds(
		canvasData.element.width*(1.0/canvasData.scale)/UNITS_PER_METER
	);
	
	mapLayer.fitBounds(mapLayerBounds);
}

let mapHidden = false;

function toggleMap() {
	if(mapHidden) {
		mapHidden = false;
		document.getElementById("map").style.setProperty("display", "block");
	}
	else {
		mapHidden = true;
		document.getElementById("map").style.setProperty("display", "none");
	}
}

function mapLayerInit() {
	//disable all user input
	mapLayer = L.map('map', {
		keyboard: false,
		zoomControl: false,
		zoomSnap: 0,
		trackResize: false,
		scrollWheelZoom: false,
		tapHold: false,
		boxZoom: false,
		dragging: false,
		inertia: false,
		zoomAnimation: false,
	}).setView(mapInitial, 19,{
		animate: false,
		zoomAnimation: false,
	});

	//use OSM
	L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
		minZoom: 2,
		maxNativeZoom: 19,
		maxZoom: 30,
		attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
	}).addTo(mapLayer);
	
	mapLayerBounds = L.latLng(...mapInitial).toBounds(
		canvasData.element.width*(1.0/canvasData.scale)/UNITS_PER_METER
	);

	mapLayer.fitBounds(mapLayerBounds);
}