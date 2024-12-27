const UNITS_PER_METER = 20;
const METERS_PER_GRID = 100;

const GRID_SIZE = UNITS_PER_METER*METERS_PER_GRID;
const LINE_WIDTH = UNITS_PER_METER*(1.435+0.20); //track gauge + 20cm bonus

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
	hideGridElem: null,
};

function onclickHandler(event) {
	//here inverse since if scene moved to right we move to left
    let x = Math.trunc(
		-canvasData.shiftX + (Math.trunc(event.pageX - event.target.offsetLeft) * (1.0/canvasData.scale))
	);
    let y = Math.trunc(
		-canvasData.shiftY + (Math.trunc(event.pageY - event.target.offsetTop) * (1.0/canvasData.scale))
	);

	console.log("Click "+x+" "+y);
	switch(currentMode) {
		case(mode.VIEW):
			break;
		case(mode.SCENARIO_NEW):
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
			onclickTrackAddHandler(x, y);
			break;
		case(mode.BUILDING_ADD):
			onclickBuildingAddHandler(x, y);
			break;
		case(mode.TREE_ADD):
			onclicktreeAddHandler(x, y);
			break;
		case(mode.LIGHT_ADD):
			onclicklightAddHandler(x, y);
			break;
		case(mode.LANDMARK_ADD):
			onclicklandmarkAddHandler(x, y);
			break;	
		case(mode.STATION_PLATFORM_ADD):
			onclickStationPlatformAddHandler(x, y);
			break;	
		case(mode.STATION_PILLAR_ADD):
			onclickStationPillarAddHandler(x, y);
			break;	
		case(mode.EDIT):
			onclickEditHandler(x, y);
			break;
		case(mode.EDIT_TRACK):
			onclickEditTrackHandler(x, y);
			break;
	}
}

function canvasClear() {
	canvasData.context.save();
	canvasData.context.resetTransform();
	canvasData.context.fillStyle = "#ffffff";
	canvasData.context.globalAlpha = 1.0;
	canvasData.context.fillRect(
		0, 0, 
		canvasData.element.width,
		canvasData.element.height
	);
	canvasData.context.restore();
}

const SCALE_MULTIPLIER = 1.2;

function canvasZoomIn() {
	canvasData.context.resetTransform(); //scale matrices multiply, reset them
	canvasData.scale *= SCALE_MULTIPLIER;
	canvasData.context.scale(canvasData.scale, canvasData.scale);
	canvasData.context.translate(canvasData.shiftX, canvasData.shiftY);
	canvasRedraw();
	console.log(canvasData.scale);
}
function canvasZoomOut() {
	canvasData.context.resetTransform();
	canvasData.scale /= SCALE_MULTIPLIER;
	canvasData.context.scale(canvasData.scale, canvasData.scale);
	canvasData.context.translate(canvasData.shiftX, canvasData.shiftY);
	canvasRedraw();
	console.log(canvasData.scale);
}
function canvasZoomReset() {
	canvasData.context.resetTransform();
	canvasData.context.translate(canvasData.shiftX, canvasData.shiftY);
	canvasData.scale = 1.0;
	canvasRedraw();
}
function canvasPosReset() {
	canvasData.context.resetTransform();
	canvasData.shiftX = 0;
	canvasData.shiftY = 0;
	canvasData.scale = 1.0;
	canvasRedraw();
}

function mouseScrollHandler(event) {
	event.preventDefault(event);
	canvasData.context.resetTransform();
	if(Math.sign(event.deltaY) <= 0) {
		//zoom in - scroll wheel up
		canvasData.scale *= SCALE_MULTIPLIER;
	}
	else {
		//zoom out - scroll wheel down
		canvasData.scale /= SCALE_MULTIPLIER;
	}
	canvasData.context.scale(canvasData.scale, canvasData.scale);
	canvasData.context.translate(canvasData.shiftX, canvasData.shiftY);
	canvasRedraw();
}

let dragStartX = 0;
let dragStartY = 0;

let oldDragX = 0;
let oldDragY = 0;

let dragEnabled = false;

function dragStartHandler(event) {
	console.log("Dragging...");
	dragStartX = Math.trunc(event.pageX - event.target.offsetLeft);
	dragStartY = Math.trunc(event.pageY - event.target.offsetTop);
	dragEnabled = true;
}
function dragHandler(event) {
	if(!dragEnabled) return;

	let dx = (Math.trunc((event.pageX - event.target.offsetLeft))-dragStartX) * (1.0/canvasData.scale);
	let dy = (Math.trunc((event.pageY - event.target.offsetTop))-dragStartY) * (1.0/canvasData.scale);

	canvasData.context.translate(dx, dy);

	canvasData.shiftX += dx;
	canvasData.shiftY += dy;

	console.log("Drag "+canvasData.shiftX+" "+canvasData.shiftY);

	dragStartX = Math.trunc(event.pageX - event.target.offsetLeft);
	dragStartY = Math.trunc(event.pageY - event.target.offsetTop);
	canvasRedraw();
}
function dragEndHandler(event) {
	dragStartX = 0;
	dragStartY = 0;
	console.log("Drag ended at "+canvasData.shiftX+" "+canvasData.shiftY);
	dragEnabled = false;
}

function canvasRedraw() {
	canvasClear();

	//draw texture parcelling  grid

	if(!canvasData.hideGridElem.checked) {
		let xAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.width/GRID_SIZE)+1;
		let yAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.height/GRID_SIZE)+1;

		let xStart = Math.floor(-canvasData.shiftX / GRID_SIZE) * GRID_SIZE;
		let yStart = Math.floor(-canvasData.shiftY / GRID_SIZE) * GRID_SIZE;

		canvasData.context.fillStyle = "#ff0000";
		canvasData.context.globalAlpha = 0.5;

		for(let i = 0; i < yAmountToDraw; i++) {
			for(let j = 0; j < xAmountToDraw; j++) {
				if((j % 2) == (i % 2 == 0)) {
					canvasData.context.fillRect(
						xStart+(j*GRID_SIZE),
						yStart+(i*GRID_SIZE),
						GRID_SIZE, GRID_SIZE
					);
				}
			}
		}

		canvasData.context.globalAlpha = 1;
	}
	
	//render tracks and buildings first so they dont block other (smaller) stuff
	[trackList, buildingList, landmarkList, nodeList, radioList, treeList, lightList].forEach((v) => {
		v.forEach((w) => {
			w.draw();
		});
	})

	//update values
	canvasData.shiftXOut.innerHTML = canvasData.shiftX;
	canvasData.shiftYOut.innerHTML = canvasData.shiftY;
	canvasData.scaleOut.innerHTML = canvasData.scale;
}

function getColliding(alist, ax, ay) {
	for(let i = 0; i < alist.length; i++) {
		if(alist[i].collision(ax, ay)) {
			return i;
		}
	}
	return -1;
}

//params: x,y pos + x,y size
//returns false if outside frustum (therefore shouldnt be rendered)
function canvasIsInFrustum(ax, ay, asx, asy) {
	return !(
		(ax + canvasData.shiftX + asx < 0) ||
		(ay + canvasData.shiftY + asy < 0) ||
		(canvasData.shiftX + ax > (canvasData.element.width * (1.0/canvasData.scale))) ||
		(canvasData.shiftY + ay > (canvasData.element.height * (1.0/canvasData.scale)))
	);
}

function canvasInit() {
    canvasData.element = document.getElementById("main");
    canvasData.element.width = 1000;
    canvasData.element.height = 800;
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
	canvasData.hideGridElem = document.getElementById("hidegrid");

	canvasData.context.lineWidth = LINE_WIDTH;

	document.getElementById("permeter").innerHTML = UNITS_PER_METER;

	statInit();

	canvasClear();
}