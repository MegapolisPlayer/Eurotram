#include "StationPillar.hpp"

StationPillar::StationPillar() noexcept : pillarBoard(512, 512, GL_RGBA8, GL_RGBA) {
	this->pillarRenderer.bindTexture(this->pillarBoard, GL_RGBA, true, true);
}

void StationPillar::updateTexture() noexcept {

}
