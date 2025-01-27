#ifndef EUROTRAM_ANIMATION
#define EUROTRAM_ANIMATION
#include "../base/Base.hpp"

namespace Keyframe {
	struct Position {
		glm::vec3 positionChange;
		GLfloat time;
	};
	struct Rotation {
		glm::quat rotation; //quaternions...
		GLfloat time;
	};
	struct Scale {
		glm::vec3 scale;
		GLfloat time;
	};
}

class Bone {
public:
private:
};

class Animation {
public:
private:

};

#endif
