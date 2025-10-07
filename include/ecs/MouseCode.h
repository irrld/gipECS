//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_MOUSECODE_H
#define GECS_MOUSECODE_H

#include <cstdint>

namespace gecs {

using MouseCode = int32_t;// Define MouseCode as int32_t

enum : MouseCode {
	MOUSE_BUTTON_0 = 0,
	MOUSE_BUTTON_1 = 1,
	MOUSE_BUTTON_2 = 2,
	MOUSE_BUTTON_3 = 3,
	MOUSE_BUTTON_4 = 4,
	MOUSE_BUTTON_5 = 5,
	MOUSE_BUTTON_6 = 6,
	MOUSE_BUTTON_7 = 7,

	MOUSE_BUTTON_LEFT = MOUSE_BUTTON_0,
	MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_1,
	MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_2
  };

}
#endif//GECS_MOUSECODE_H
