#include "Signal.hpp"

Signal::Signal() noexcept : signalBoard(512, 512, GL_RGBA8, GL_RGBA) {
	this->signalRenderer.bindTexture(this->signalBoard, GL_RGBA, true, true);
}

void Signal::updateTexture() noexcept {

}
