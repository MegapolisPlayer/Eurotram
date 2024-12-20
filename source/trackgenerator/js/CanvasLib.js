let canvasData = {
    element: null,
    context: null,
	edit: null,
	scale: 1.0,
	shiftX: 0,
	shiftY: 0
};

function onclickHandler(event) {
	//here inverse since if scene moved to right we move to left
    let x = -canvasData.shiftX + Math.trunc((event.pageX - event.target.offsetLeft)) * (1.0/canvasData.scale);
    let y = -canvasData.shiftY + Math.trunc((event.pageY - event.target.offsetTop)) * (1.0/canvasData.scale);

	console.log("Click "+x+" "+y);
	switch(currentMode) {
		case(mode.VIEW): break;
		case(mode.SCENARIO_NEW):
			//open mode where we click stations and switches of line
			break;
		case(mode.NODE_ADD):
			onclickNodeAddHandler(x, y);
			break;
		case(mode.TRACK_ADD):
			onclickTrackAddHandler(x, y);
			break;
		case(mode.BUILDING_ADD):
			//add building - type to choose from
			break;
		case(mode.TREE_ADD):
			//add tree - just circle (handled in sim itself - random)
			break;
		case(mode.LIGHT_ADD):
			//just add streetlamp
			break;
		case(mode.EDIT):
			onclickEditHandler(x, y);
			break;
	}
}

function canvasClear() {
	canvasData.context.fillStyle = "#ffffff";
	canvasData.context.fillRect(
		0, 0, 
		canvasData.element.width*(1.0/canvasData.scale),
		canvasData.element.height*(1.0/canvasData.scale)
	);
}

const SCALE_MULTIPLIER = 1.2;

function canvasZoomIn() {
	canvasData.context.resetTransform(); //scale matrices multiply, reset them
	canvasData.scale *= SCALE_MULTIPLIER;
	canvasData.context.scale(canvasData.scale, canvasData.scale);
	canvasRedraw();
	console.log(canvasData.scale);
}
function canvasZoomOut() {
	canvasData.context.resetTransform();
	canvasData.scale /= SCALE_MULTIPLIER;
	canvasData.context.scale(canvasData.scale, canvasData.scale);
	canvasRedraw();
	console.log(canvasData.scale);
}
function canvasZoomReset() {
	canvasData.context.resetTransform();
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

	canvasData.shiftX += (Math.trunc((event.pageX - event.target.offsetLeft))-dragStartX) * (1.0/canvasData.scale);
	canvasData.shiftY += (Math.trunc((event.pageY - event.target.offsetTop))-dragStartY) * (1.0/canvasData.scale);
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

function canvasDrawNodes() {
	nodeList.forEach((v) => {
		v.draw();
	});
}

function canvasDrawTrack() {
	trackList.forEach((v) => {
		v.draw();
	});
}

function canvasRedraw() {
	canvasClear();
	canvasDrawNodes();
	canvasDrawTrack();
}

//params: x,y pos + x,y size
//returns false if outside frustum (therefore shouldnt be rendered)
function canvasIsInFrustum(ax, ay, asx, asy) {
	//TODO add frustum culling (dont draw if outside of canvas)
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
}