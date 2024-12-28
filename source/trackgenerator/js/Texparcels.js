//when saving - set different depth to each to avoid z-fighting
//add button - "download texture preps" - saves into zip file with station name

class Texparcel {
	x = 0; //original top left corner
	y = 0;
	width = 0;
	height = 0;
	rotation = 0;

	heightPointTL = 0; //top-left
	heightPointTR = 0; //top-right
	heightPointBL = 0; //bottom-left
	heightPointBR = 0; //bottom-right

	//can be in 2 stations since can be on track between the two
	stationCode1 = "";
	stationCode2 = "";
}

let texparcelList = [];

function renderTextureParcels() {
	
}

/*
TODO remove when texparcels code done
function renderGrid() {
	let xAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.width/GRID_SIZE)+1;
	let yAmountToDraw = Math.trunc((1.0/canvasData.scale)*canvasData.element.height/GRID_SIZE)+1;
	let xStart = Math.floor(-canvasData.shiftX / GRID_SIZE) * GRID_SIZE;
	let yStart = Math.floor(-canvasData.shiftY / GRID_SIZE) * GRID_SIZE;

	canvasData.context.fillStyle = "#ff0000";
	canvasData.context.globalAlpha = 0.25;

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
*/