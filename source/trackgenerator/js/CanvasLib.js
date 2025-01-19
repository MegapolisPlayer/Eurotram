let UNITS_PER_METER = 20;

const TRACK_WIDTH = UNITS_PER_METER*(1.435+0.20); //track gauge + 20cm bonus
const LINE_WIDTH = 10;

const SELECT_COLOR = "#ff00ff";

let canvasData = {
    element: null,
    context: null,
	edit: null,
	scale: 1.0,
	shiftX: 0,
	shiftY: 0,
	mode: null,
	shiftXOut: null,
	shiftYOut: null,
	scaleOut: null,
	hideTPElem: null,
	hidden: false,
	canvasmenu: null,
};

function hideCanvas() {
	canvasData.hidden = true;
	document.getElementById("main").style.setProperty("display", "none");
	document.getElementById("lowerbg").style.setProperty("display", "none");
}

function showCanvas() {
	canvasData.hidden = false;
	document.getElementById("main").style.setProperty("display", "block");
	document.getElementById("lowerbg").style.setProperty("display", "block");
}

function onclickHandler(aEvent) {
	//early exits
	if(canvasData.hidden) return;
	if(currentMode === mode.VIEW) { return; }

	//here inverse since if scene moved to right we move to left (and same with Y axis)
	//also this is easier than calculating transformations by hand (again)
	let p = getPoint(aEvent);
    let x = p.x;
    let y = p.y;

	/*
	canvasData.context.fillStyle = SELECT_COLOR;
	canvasData.context.fillRect(
		x - NODE_SIZE/2,
		y - NODE_SIZE/2,
	NODE_SIZE, NODE_SIZE);
	*/

	switch(currentMode) {
		case(mode.VIEW):
			break; //handled in early exit, here for completeness sake
		case(mode.SCENARIO_NEW_LINE):
			onclickNewScenarioHandler(x, y);
			break;
		case(mode.NODE_ADD):
			onclickTNodeAddHandler(x, y);
			break;
		case(mode.SWITCH_ADD):
			onclickSwitchAddHandler(x, y);
			break;
		case(mode.SWITCH_SIG_ADD):
			onclickSwitchSignalAddHandler(x, y);
			break;
		case(mode.SIGNAL_ADD):
			onclickSignalAddHandler(x, y);
			break;
		case(mode.RADIO_ADD):
			onclickRadioAddHandler(x, y);
			break;
		case(mode.TRACK_ADD):
			onclickTrackAddHandler(x, y, false);
			break;
		case(mode.STATION_TRACK_ADD):
			onclickTrackAddHandler(x, y, true);
			break;
		case(mode.BUILDING_ADD):
			onclickBuildingAddHandler(x, y);
			break;
		case(mode.TREE_ADD):
			onclickTreeAddHandler(x, y);
			break;
		case(mode.LIGHT_ADD):
			onclickLightAddHandler(x, y);
			break;
		case(mode.LANDMARK_ADD):
			onclickLandmarkAddHandler(x, y);
			break;
		case(mode.WALL_ADD):
			onclickWallAddHandler(x, y);
			break;	
		case(mode.STATION_PILLAR_ADD):
			onclickStationPillarAddHandler(x, y);
			break;	
		case(mode.SIGN_ADD):
			onclickSignAddHandler(x, y);
			break;
		case(mode.TEXTURE_PARCEL_ADD):
			onclickTextureParcelAddHandler(x, y);
			break;
		case(mode.PRESIGNAL_ADD):
			onclickPreignalAddHandler(x, y);
			break;
		case(mode.EDIT):
			onclickEditHandler(x, y);
			break;
		case(mode.EDIT_TRACK):
			onclickEditTrackHandler(x, y);
			break;
		case(mode.EDIT_TEXTURE_PARCEL):
			onclickTextureParcelEditHandler(x, y);
			break;
		default:
			console.error("Unregistered onclick event!");
			break;
	}
}

function canvasClear() {
	canvasData.context.save();
	canvasData.context.resetTransform();
	canvasData.context.globalAlpha = 1.0;
	canvasData.context.clearRect(
		0, 0, 
		canvasData.element.width,
		canvasData.element.height
	);
	canvasData.context.restore();
}

const SCALE_MULTIPLIER = 1.2;

function canvasApplyScale(anewscale) {
	canvasData.context.resetTransform();

	canvasData.context.translate(
		(canvasData.element.width/2),
		(canvasData.element.height/2)
	);

	canvasData.context.scale(anewscale, anewscale);

	canvasData.context.translate(
		-(canvasData.element.width/2),
		-(canvasData.element.height/2)
	);

	canvasData.context.translate(
		canvasData.shiftX,
		canvasData.shiftY
	);

	canvasData.scale = anewscale;
}

function canvasZoomIn() {
	canvasApplyScale(canvasData.scale *= SCALE_MULTIPLIER);

	canvasRedraw();

	mapLayerUpdate();
}
function canvasZoomOut() {
	canvasApplyScale(canvasData.scale /= SCALE_MULTIPLIER);

	canvasRedraw();

	mapLayerUpdate();
}
function canvasZoomReset() {
	canvasData.context.resetTransform();
	canvasData.context.translate(canvasData.shiftX, canvasData.shiftY);
	canvasData.scale = 1.0;
	canvasRedraw();

	mapLayerUpdate();
}
function canvasPosReset() {
	canvasData.context.resetTransform();
	canvasData.shiftX = 0;
	canvasData.shiftY = 0;
	canvasData.scale = 1.0;
	canvasRedraw();

	mapLayerReset();
}

function mouseScrollHandler(event) {
	event.preventDefault(event);

	if(Math.sign(event.deltaY) <= 0) {
		//zoom in - scroll wheel up
		canvasApplyScale(canvasData.scale *= SCALE_MULTIPLIER);
	}
	else {
		//zoom out - scroll wheel down
		canvasApplyScale(canvasData.scale /= SCALE_MULTIPLIER);
	}

	canvasRedraw();

	mapLayerUpdate();
}

let dragStartX = 0;
let dragStartY = 0;

let oldDragX = 0;
let oldDragY = 0;

let dragEnabled = false;

function dragStartHandler(event) {
	if(canvasData.hidden) return;
	dragStartX = Math.trunc(event.pageX - event.target.offsetLeft);
	dragStartY = Math.trunc(event.pageY - event.target.offsetTop);
	dragEnabled = true;
}
function dragHandler(event) {
	if(canvasData.hidden) return;
	if(!dragEnabled) return;

	let dx = Math.trunc((event.pageX - event.target.offsetLeft))-dragStartX;
	let dy = Math.trunc((event.pageY - event.target.offsetTop))-dragStartY;

	mapLayerUpdate(dx, dy); //send clean data here

	dx = Math.trunc(dx * (1.0/canvasData.scale));
	dy = Math.trunc(dy * (1.0/canvasData.scale));

	canvasData.context.translate(dx, dy);

	canvasData.shiftX += dx;
	canvasData.shiftY += dy;

	dragStartX = Math.trunc(event.pageX - event.target.offsetLeft);
	dragStartY = Math.trunc(event.pageY - event.target.offsetTop);
	canvasRedraw();
}
function dragEndHandler(event) {
	if(canvasData.hidden) return;
	dragStartX = 0;
	dragStartY = 0;
	dragEnabled = false;
}

function canvasRedraw(aNoTPOverride = false) {
	canvasClear();
	
	//draw texture parcelling before nodes

	if(!canvasData.hideTPElem.checked && !aNoTPOverride) {
		renderTextureParcels();
	}
	
	//render tracks and buildings first so they dont block other (smaller) stuff
	[
		trackList,
		buildingList, 
		landmarkList, 
		nodeList, 
		switchList,
		radioList, 
		treeList, 
		lightList,
		stationPillarList,
		signalList,
		switchSignalList,
		wallList,
		signList,
	].forEach((v) => {
		v.forEach((w) => {
			w.draw();
		});
	})

	//update values
	canvasData.shiftXOut.replaceChildren(document.createTextNode(canvasData.shiftX + " (" + Math.trunc(canvasData.shiftX/UNITS_PER_METER) + "m)"));
	canvasData.shiftYOut.replaceChildren(document.createTextNode(canvasData.shiftY + " (" + Math.trunc(canvasData.shiftY/UNITS_PER_METER) + "m)"));
	canvasData.scaleOut.replaceChildren(document.createTextNode(canvasData.scale));
}

//params: x,y pos + x,y size
//returns false if outside frustum (therefore shouldnt be rendered)
//scale offset * scale - why does this work?
function canvasIsInFrustum(aX, aY, asx, asy) {
	return true;
	/*
	return !(
		(aX + canvasData.shiftX + asx < 0) ||
		(aY + canvasData.shiftY + asy < 0) ||
		(canvasData.shiftX + aX > (canvasData.element.width * (1.0/canvasData.scale))) ||
		(canvasData.shiftY + aY > (canvasData.element.height * (1.0/canvasData.scale)))
	);
	*/
}

function canvasInit() {
	statInit();

    canvasData.element = document.getElementById("main");
    canvasData.element.width = 1000;
    canvasData.element.height = 1000;
    canvasData.context = canvasData.element.getContext("2d");

    canvasData.element.addEventListener("click", onclickHandler);
	canvasData.element.addEventListener("mousedown", dragStartHandler);
	canvasData.element.addEventListener("mousemove", dragHandler);
	canvasData.element.addEventListener("mouseup", dragEndHandler);
	canvasData.element.addEventListener("wheel", mouseScrollHandler);

	canvasData.edit = document.getElementById("editoptions");
	canvasData.mode = document.getElementById("curmode");
	canvasData.shiftXOut = document.getElementById("shiftx");
	canvasData.shiftYOut = document.getElementById("shifty");
	canvasData.scaleOut = document.getElementById("scale");
	canvasData.hideTPElem = document.getElementById("hideter");

	canvasData.context.lineWidth = LINE_WIDTH;
	canvasData.context.textAlign = "center";
	canvasData.context.textBaseline = "middle";

	document.getElementById("fmtversion").textContent = mapFileFormatVersion;
	document.getElementById("lfmtversion").textContent = lineFileFormatVersion;
	
	canvasData.canvasmenu = document.getElementById("menucontainer");

	//make selectors - optimization and code-saving measure
	//to not generate large element packs at runtime but at "init time"

	makeBuildingTypeSelector();
	makeLandmarkSelector();
	makeSignTypeSelector();

	mapLayerInit();

	canvasClear();
}

//TODO remove code duplication in update, edit menu and remove functions
//TODO add utils for creating buttons
//TODO fix GUI of app
//TODO enforce coding conventions
//TODO abstract translations in draw calls
//TODO make all objects inherit from base XYHR/XYH