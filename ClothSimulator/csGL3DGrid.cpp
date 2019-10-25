
#include "csGL3DGrid.h"

#include "csGL3DObject.h"

#include "wx/wx.h"

void csGL3DGrid::create(float width, float depth, float major)
{
	//wxLogDebug("\tcreating grid");
	m_draw_mode = GL_LINES;

	std::vector< glm::vec3 > vertices;
	std::vector< unsigned int > indices;
	// create ground vertices

	float minx = -width / 2;
	float minz = -depth / 2;
	float maxx = width / 2;
	float maxz = depth / 2;
	int xdiv = (int)width / major;
	int zdiv = xdiv;
	float dx = (maxx - minx) / (xdiv + 1);
	float dz = (maxz - minz) / (zdiv + 1);

	for (float x = minx; x <= maxx; x += dx)
	{
		for (float z = minz; z <= maxz; z += dz)
		{
			vertices.push_back(glm::vec3(x, 0, z));
		}
	}

	// create ground indices
	int totlines = (xdiv + 1) + 1;
	int x = 0;

	for (int i = 0; i < totlines; i++) {
		indices.push_back(x);
		indices.push_back(x + zdiv + 1);

		x += zdiv + 1 + 1;
	}

	totlines = (zdiv + 1) + 1;
	int z = 0;

	for (int i = 0; i < totlines; i++) {
		indices.push_back(z);
		indices.push_back(z + (zdiv + 2) * (xdiv + 1));

		z += 1;
	}

	creategl(vertices, indices);
}
