#pragma once

/** A class to store 2D vertices to be used by OpenGL. */
class Vertex2D
{
public:
	float x, y;
	float u, v;

	/**Initialises data to zero.*/
	Vertex2D();

	/**Initialised data to the inputs.
	@param x_ The x position.
	@param y_ The y position.
	@param z_ The z position.
	@param u_ The u UV coordinate.
	@param v_ The v UV coordinate.
	*/
	Vertex2D(float x_, float y_, float u_, float v_);
};