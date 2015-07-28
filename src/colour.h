#pragma once

/**
* A colour class that can store a colour in a variety of formats (rgba, hsva, etc.) as can be found in the Type enum.
* */
class Colour
{
public:
	enum class Type {RGBA, HSVA};

	union
	{
		unsigned char r,g,b,a;
		unsigned char h,s,v,a_;
	};
};