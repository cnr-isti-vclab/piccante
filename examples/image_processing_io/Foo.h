#pragma once

#define PIC_ENABLE_INLINING

#include <image.hpp>

class Foo
{
public:
	Foo();
	~Foo();

	void process(pic::Image img);
};

