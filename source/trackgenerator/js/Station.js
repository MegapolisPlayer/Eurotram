class StationPillar {
	xpos = 0;
	ypos = 0;
	height = 0;

	//TODO support more types of pillars in the future

	constructor(axpos, aypos) {
		this.xpos = axpos;
		this.ypos = aypos;
	}
}

let stationPillarList = [];

function stationPillarEditMenu(aid) {
	
}

//uses some elements of track
//same system as node-switch
class StationTrack {

}

function stationTrackEditMenu(aid) {
	
}

//short: 30m (1 tram / 2xT3)
//long: 70m (2 trams / 4xT3)

let stationPlatformType = {
	NONE: 0, //no platform, e.g. Sumavska
	
	//slim, just small asphalt island - no fences, nothing

	OLD_SHORT: 1, //e.g. Jiriho z Podebrad (to center)
	OLD_LONG: 2, //e.g. Dlouha Trida

	//tiled, may have fences

	MIDDLE_SHORT: 3, //e.g. Ruska
	MIDDLE_SHORT_FENCE: 4, //e.g. Vyton (from center)
	MIDDLE_LONG: 5, //e.g. Sumavska, Cechovo nam.
	MIDDLE_LONG_FENCE: 6, //e.g. Flora

	//asphalt with red line

	NEW_SHORT: 7, //e.g. Vinohradska trznice
	NEW_SHORT_FENCE: 8, //e.g. Libus (exit only)
	NEW_LONG: 9, //e.g. Krejcarek
	NEW_LONG_FENCE: 10, //e.g. U Zvonu
};

class StationPlatform {
	xpos = 0;
	ypos = 0;
}

let stationPlatformList = [];

function stationPlatformEditMenu(aid) {
	
}
