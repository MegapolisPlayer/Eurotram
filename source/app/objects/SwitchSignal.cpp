#include "SwitchSignal.hpp"

SwitchSignal::SwitchSignal() noexcept : switchSignalBoard(512, 512, GL_RGBA8, GL_RGBA)  {
	this->switchSignalRenderer.bindTexture(this->switchSignalBoard, GL_COLOR_ATTACHMENT4, true, true);
}

void SwitchSignal::updateTexture() noexcept {

}
