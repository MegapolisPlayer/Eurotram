//normal semaphore for trams
class Signal {
	xpos = 0;
	ypos = 0;
	height = 0;
	poleHeight = 2; //values <= 0 means that is hangs on catenary wire
	stationCode = "";

	//teal square
	draw(style = "#0000aa") {

	}
}



class SwitchStateSignalUnit {
	letter = "";
}

let signalUnits = []; //switches write here

//switch semaphore - shows
//can show state of multiple switches - every switch has letter
class SwitchStateSignal {
	xpos = 0;
	ypos = 0;
	height = 0;
	stationCode = "";

	unitIds = [];

	//teal square with red border + text showing how many units
	draw(style = "#0000aa", style2 = "#ff0000") {

	}
}
 