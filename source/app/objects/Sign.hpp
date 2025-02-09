#ifndef EUROTRAM_OBJECTS_SIGN
#define EUROTRAM_OBJECTS_SIGN
#include "Wire.hpp"

enum struct SignType {
	NONE = 0,
	//zajistovaci vyhybka
	LOCKABLE_SWITCH,
	//stuj
	STOP,
	//prednost tramvaje
	MAIN_ROAD,
	//rychlostnik - rychlost nizsi nez cekana
	MAX_SPEED_UNEXPECTED,
	//konec toho
	MAX_SPEED_UNEXPECTED_END,
	//pomalu
	SLOW,
	//omezeni rychlosti
	MAX_SPEED,
	//konec toho
	MAX_SPEED_END,
	//max. rychlost danym mistem
	MAX_SPEED_POINT,
	//pomalu pri odb. ve vyhybce - 10km/h
	SLOW_SWITCH,
	//uzka zatacka - max. 15
	TIGHT_CURVE,
	//zona 30
	ZONE_30,
	//zona 30 konec
	ZONE_30_END,
	//zona 40
	ZONE_40,
	//zona 40 konec
	ZONE_40_END,
	//usekovy delic
	SEGMENT_DIVIDER,
	//usekovy delic - magnetovy, mozne odebirat proud
	SEGMENT_DIVIDER_MAGNET,
	//stahni sberace
	PANTOGRAPH_DOWN,
	//zvedni sberac
	PANTOGRAPH_UP
};

struct Sign {
	ObjectLocation location;
	SignType type;
	StationCode code;
};

#endif
