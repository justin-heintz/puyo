#pragma once
#include <glm/glm.hpp>
class slime {
	public:
		drawOBJ image;
		glm::vec2 cords = glm::vec2(1.5, 4.0);
		slime() { }
		void create() {
			image.create();
		}
		void updateCords(glm::vec2 newCords) {
			cords = cords + newCords;
		}
};