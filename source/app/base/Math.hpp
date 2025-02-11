#ifndef EUROTRAM_BASE_MATH
#define EUROTRAM_BASE_MATH
#include "FreeUtils.hpp"

//we "only" calculate 2D bezier - 3D height is stored in track's heightpoints'
glm::vec2 bezier(glm::vec2 aPoint1, glm::vec2 aCP1, glm::vec2 aCP2, glm::vec2 aPoint2)  noexcept;

glm::vec3 normals(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec3 aPoint4) noexcept;

#endif
