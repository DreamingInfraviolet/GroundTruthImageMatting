#include "vertex.h"
Vertex3D::Vertex3D()
{
	x = 0;
	y = 0;
	z = 0;
	normx = 0;
	normy = 0;
	normz = 0;
	u = 0;
	v = 0;
}

Vertex3D::Vertex3D(float x_, float y_, float z_, float nx, float ny, float nz, float u_, float v_)
{
	x = x_;
	y = y_;
	z = z_;
	normx = nx;
	normy = ny;
	normz = nz;
	u = u_;
	v = v_;
}

Vertex2D::Vertex2D()
{
	x = 0;
	y = 0;
	u = 0;
	v = 0;
}

Vertex2D::Vertex2D(float x_, float y_, float u_, float v_)
{
	x = x_;
	y = y_;
	u = u_;
	v = v_;
}