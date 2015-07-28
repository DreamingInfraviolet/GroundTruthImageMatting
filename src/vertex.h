#pragma once
/**A class to manage 3D vertices in the rendering pipeline.*/
class Vertex3D
{
public:
	float x, y, z;
	float normx, normy, normz;
	float u, v;
	
	/**Initialises data to zero.*/
	Vertex3D();

	/**Initialised data to the inputs.
	@param x_ The x position.
	@param y_ The y position.
	@param z_ The z position.
	@param nx The normal x coordinate.
	@param ny The normal y coordinate.
	@param nz The normal z coordiante.
	@param u_ The u UV coordinate.
	@param v_ The v UV coordinate.
	*/
	Vertex3D(float x_, float y_, float z_, float nx, float ny, float nz, float u_, float v_);
};

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