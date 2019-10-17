#include "Cloth.h"

#include "wx/wx.h"

#include <map>
#include <stdlib.h> // rand()

class IPInfo
{
public:
	~IPInfo() {
		if (points)
			delete points;
	}
	IPInfo(std::map<wxString, int>* pts, int cnt) { points = pts; count = cnt; }

	std::map<wxString, int> *points;
	int count;
};

class Face
{
public:
	Face() {}
	Face(int a, int b, int c, int d) { indices.push_back(a); indices.push_back(b); indices.push_back(c); indices.push_back(d); }
	std::vector<int> indices;
};

float intersects(float a, float b, float c, float d, float p, float q, float r, float s) {
	float det, gamma, lambda;
	det = (c - a) * (s - q) - (r - p) * (d - b);
	if (det != 0) {
		lambda = ((s - q) * (r - a) + (p - r) * (s - b)) / det;
		gamma = ((b - d) * (r - a) + (c - a) * (s - b)) / det;
		if ((0 < lambda && lambda <= 1) && (0 < gamma && gamma < 1))
			return lambda;
	}
	else {
		return -2; // parallel
	}

	return -1;
};

// to compare 
bool equals(glm::vec3 &a, glm::vec3 &b) {
	return std::abs(a.x - b.x) < 0.0001f && std::abs(a.y - b.y) < 0.0001f && std::abs(a.z - b.z) < 0.0001f;
}

IPInfo *intersection_points(std::vector<glm::vec3>& verts, Face& face, glm::vec2& line0, glm::vec2& line1) {
	std::map<wxString, int> *ips = new std::map<wxString, int>(); // indices of the intersecting points between edges
	int count = 0;

	int lidx = face.indices.size() - 1; // last index
	//console.log(face);
	glm::vec3 *fp = &verts[face.indices[lidx]]; // first point
	//console.log(fp);
	for (size_t i = 0; i < face.indices.size(); i++) {
		glm::vec3* sp = &verts[face.indices[i]]; // second point
		float dist = intersects(fp->x, fp->y, sp->x, sp->y, line0.x, line0.y, line1.x, line1.y);
		if (dist == 0) { // same as the first point
			(*ips)[wxString::Format(wxT("%d-%d"), lidx, i)] = face.indices[lidx];
			count++;
			//console.log('same as first!');
		}
		else if (dist == 1) { // same as the last point
			(*ips)[wxString::Format(wxT("%d-%d"), lidx, i)] = face.indices[i];
			count++;
			//console.log('same as second!');
		}
		else if (dist > 0 && dist < 1) { // add a new point and use its index
	   // check previous point
			glm::vec3* nv = new glm::vec3(fp->x + (sp->x - fp->x) * dist, fp->y + (sp->y - fp->y) * dist, 0);
			int found = false;
			for (size_t v = 0; v < verts.size(); v++) {
				//if (nv->x==verts[v].x && nv->y == verts[v].y && nv->z == verts[v].z)
				if(equals(*nv, verts[v]))
				{
					(*ips)[wxString::Format(wxT("%d-%d"), lidx, i)] = v;
					found = true;
					break;
				}
			}
			if (!found) {
				(*ips)[wxString::Format(wxT("%d-%d"), lidx, i)] = verts.size();
				verts.push_back(*nv);
			}
			delete nv;
			count++;
		}
		else {
			(*ips)[wxString::Format(wxT("%d-%d"), lidx, i)] = -1;
		}

		// make second point the first
		fp = sp;
		lidx = i;
	}

	return new IPInfo(ips, count);
}

void cut_faces_Greiner_Hormann(std::vector<glm::vec3> &verts, std::vector<Face> &faces, glm::vec2 &line0, glm::vec2 &line1)
{
	std::vector<Face> newFaces;

	// loop thru' all the current faces to be cut by line 0, 1
	for (size_t f = 0; f < faces.size(); f++) {
		Face *face = &faces[f];
		std::vector<int> indices;
		Face newFace;
		bool old = true;
		bool hasSplit = false;

		// check if the line intersects this face
		IPInfo *idata = intersection_points(verts, *face, line0, line1);
		int icount = idata->count;

		// if there are intersections
		if (icount > 1) {
			int lidx = face->indices.size() - 1; // last index
			// create a new face and adjust the old one
			for (size_t i = 0; i < face->indices.size(); i++) {
				if (old)
					indices.push_back(face->indices[lidx]); // push the first point to the current polygon
				else
					if ((newFace.indices.size() == 0) || (newFace.indices.size() > 0 && newFace.indices[newFace.indices.size() - 1] != face->indices[lidx]))
						newFace.indices.push_back(face->indices[lidx]);

				int ipidx = (*idata->points)[wxString::Format(wxT("%d-%d"), lidx, i)];
				if (ipidx >= 0) {
					indices.push_back(ipidx);
					if ((newFace.indices.size() == 0) || (newFace.indices.size() > 0 && newFace.indices[newFace.indices.size() - 1] != ipidx))
						newFace.indices.push_back(ipidx);
					old = !old;
					hasSplit = true;
				}
				lidx = i;
			}
		}
		if (hasSplit && icount > 1) { // if the face was split, update the old face and add the new one
		  //console.log('has split');
			face->indices = indices;
			newFaces.push_back(newFace);
		}

		delete idata;
	}

	// add the new faces
	for (size_t f = 0; f < newFaces.size(); f++)
		faces.push_back(newFaces[f]);
}

void Cloth::create(float x1, float y1, float x2, float y2, float z, float segment)
{
	std::vector<glm::vec3> vertices;
	std::vector< unsigned int > indices;
	std::vector<Face> faces;

	float minx = glm::min(x1, x2);
	float maxx = glm::max(x1, x2);
	float miny = glm::min(y1, y2);
	float maxy = glm::max(y1, y2);

	vertices.push_back(glm::vec3(minx, miny, z));
	vertices.push_back(glm::vec3(maxx, miny, z));
	vertices.push_back(glm::vec3(maxx, maxy, z));
	vertices.push_back(glm::vec3(minx, maxy, z));

	Face face(0, 1, 2, 3);
	faces.push_back(face);

	//cut_faces_Greiner_Hormann(vertices, faces, glm::vec2(-1e10, 0), glm::vec2(1e10, 0));
	for (float y = miny + segment; y <= maxy; y += segment) {
		cut_faces_Greiner_Hormann(vertices, faces, glm::vec2(-1e10, y), glm::vec2(1e10, y));
	}
	// cut polygon on the grid/vertical
	for (float x = minx + segment; x <= maxx; x += segment) {
		cut_faces_Greiner_Hormann(vertices, faces, glm::vec2(x, -1e10), glm::vec2(x, 1e10));
	}

	/*
	int wobble = 10;
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].x += rand() % wobble - wobble / 2;
		vertices[i].y += rand() % wobble - wobble / 2;
		vertices[i].z += rand() % wobble - wobble / 2;
		//wxLogDebug("vert %d: %f, %f, %f", i, vertices[i].x, vertices[i].y, vertices[i].z);
	}
	*/

	for (Face f : faces) {
		//wxLogDebug("face");
		if (f.indices.size() > 2)
		{
			for (size_t i = 1; i < f.indices.size()-1; i++) {
				indices.push_back(f.indices[0]);
				//wxLogDebug("ind %d", f.indices[0]);
				for (size_t j = i+1; j >= i; j--) {
					indices.push_back(f.indices[j]);
					//wxLogDebug("ind %d", f.indices[j]);
				}
			}
		}
	}

	m_draw_mode = GL_TRIANGLES;

	creategl(vertices, indices);

	vertices.clear();
	indices.clear();
}
