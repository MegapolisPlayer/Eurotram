function scenarioClear() {
	nodeList.length = 0;
	trackList.length = 0;
	//TODO add trees, light, etc...
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

function scenarioNewLine() {

}

function scenarioSerialize() {

}
function scenarioLoad() {
	
}