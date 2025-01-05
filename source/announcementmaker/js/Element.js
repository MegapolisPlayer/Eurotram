const METRO_STATE = {
	NONE: 0,
	NORMAL: 1,
	ISSUE: 2
};

class StationEntry {
	stationName = "";
	stationCode = "";

	metroA = METRO_STATE.NONE;
	metroB = METRO_STATE.NONE;
	metroC = METRO_STATE.NONE;

	railTransfer = false;

	requestStop = false;
};

function newElement() {

}

function resetElement() {

}

//TODO add "test announcement" and "test next stop announcement"