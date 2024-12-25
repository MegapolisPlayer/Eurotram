let buildingType = {
	NORMAL: 0,
	CORNER: 1,
	FAMILY: 2,
	PREFAB: 3
};

//building types
//NORMAL
//
//CORNER
//
//FAMILY
//
//PREFAB
//

class Building {
	xpos = 0;
	ypos = 0;
	xsize = 0;
	ysize = 0;
	height = 0;
	type = 0;

	constructor(axpos, aypos) {

	}

	updateFromType() {
		switch(this.type) {
			case(buildingType.NORMAL):

			break;
			case(buildingType.CORNER):

			break;
			case(buildingType.FAMILY):

			break;
			case(buildingType.PREFAB):

			break;
		}
	}

	draw(style = "#ffff00") {
		
	}

	collision(ax, ay) {
		
	}

	willRender() {
		
	}
};

let buildingList = [];

function buildingEditMenu() {

}

function buildingUpdate() {

}

function buildingRemove() {
	
}