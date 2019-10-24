#include "ClothMesh.h"

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
bool equals(Vertex* a, Vertex* b) {
	return std::abs(a->x - b->x) < 0.0001f && std::abs(a->y - b->y) < 0.0001f && std::abs(a->z - b->z) < 0.0001f;
}

IPInfo *intersection_points(std::vector<Vertex*>& verts, Face& face, glm::vec2& line0, glm::vec2& line1) {
	std::map<wxString, int> *ips = new std::map<wxString, int>(); // indices of the intersecting points between edges
	int count = 0;

	size_t lidx = face.indices.size() - 1; // last index
	Vertex *fp = verts[face.indices[lidx]]; // first point
	//console.log(fp);
	for (size_t i = 0; i < face.indices.size(); i++) {
		Vertex* sp = verts[face.indices[i]]; // second point
		float dist = intersects(fp->x, fp->y, sp->x, sp->y, line0.x, line0.y, line1.x, line1.y);
		if (dist == 0) { // same as the first point
			wxString key;
			key << lidx << "-" << i;
			(*ips)[key] = face.indices[lidx];
			count++;
		}
		else if (dist == 1) { // same as the last point
			wxString key;
			key << lidx << "-" << i;
			(*ips)[key] = face.indices[i];
			count++;
		}
		else if (dist > 0 && dist < 1) { // add a new point and use its index
	   // check previous point
			Vertex* nv = new Vertex(fp->x + (sp->x - fp->x) * dist, fp->y + (sp->y - fp->y) * dist, 0);
			int found = false;
			for (size_t v = 0; v < verts.size(); v++) {
				if(equals(nv, verts[v]))
				{
					wxString key;
					key << lidx << "-" << i;
					(*ips)[key] = v;
					found = true;
					break;
				}
			}
			if (!found) {
				wxString key;
				key << lidx << "-" << i;
				(*ips)[key] = verts.size();
				verts.push_back(nv);
			} else
				delete nv;

			count++;
		}
		else {
			wxString key;
			key << lidx << "-" << i;
			(*ips)[key] = -1;
		}

		// make second point the first
		fp = sp;
		lidx = i;
	}

	return new IPInfo(ips, count);
}

void cut_faces_Greiner_Hormann(std::vector<Vertex*> &verts, std::vector<Face*> &faces, glm::vec2 &line0, glm::vec2 &line1)
{
	std::vector<Face*> newFaces;

	// loop thru' all the current faces to be cut by line 0, 1
	for (size_t f = 0; f < faces.size(); f++) {
		Face *face = faces[f];
		std::vector<int> indices;
		Face *newFace = new Face();
		bool old = true;
		bool hasSplit = false;

		// check if the line intersects this face
		IPInfo *idata = intersection_points(verts, *face, line0, line1);
		int icount = idata->count;

		// if there are intersections
		if (icount > 1) {
			size_t lidx = face->indices.size() - 1; // last index
			// create a new face and adjust the old one
			for (size_t i = 0; i < face->indices.size(); i++) {
				if (old)
					indices.push_back(face->indices[lidx]); // push the first point to the current polygon
				else
					if ((newFace->indices.size() == 0) || (newFace->indices.size() > 0 && newFace->indices[newFace->indices.size() - 1] != face->indices[lidx]))
						newFace->indices.push_back(face->indices[lidx]);
				wxString key;
				key << lidx << "-" << i;
				int ipidx = (*idata->points)[key];
				if (ipidx >= 0) {
					indices.push_back(ipidx);
					if ((newFace->indices.size() == 0) || (newFace->indices.size() > 0 && newFace->indices[newFace->indices.size() - 1] != ipidx))
						newFace->indices.push_back(ipidx);
					old = !old;
					hasSplit = true;
				}
				lidx = i;
			}
		}
		if (hasSplit && icount > 1) { // if the face was split, update the old face and add the new one
			face->indices = indices;
			newFaces.push_back(newFace);
		}
		else
			delete newFace;

		delete idata;
	}

	// add the new faces
	for (size_t f = 0; f < newFaces.size(); f++)
		faces.push_back(newFaces[f]);
}

ClothMesh::~ClothMesh()
{
	clean();
}

void ClothMesh::updateNormals()
{
	// reset all vertices and normals
	for (auto v : m_vertices) {
		v->m_face_count = 0;
	}
	for (auto v : m_normals) {
		v->x = v->y = v->z = 0.0f;
	}

	// calculate the face normal
	for (auto f : m_faces) {
		// create normals
		if (f->indices.size() < 2)
			wxLogError("Found %i sided face!", (int)f->indices.size());

		Vertex* v0 = m_vertices.at(f->indices.at(0));
		Vertex* v1 = m_vertices.at(f->indices.at(1));
		Vertex* v2 = m_vertices.at(f->indices.at(2));

		glm::vec3 v10 = glm::normalize(*v1 - *v0);
		glm::vec3 v20 = glm::normalize(*v2 - *v0);
		f->normal = glm::cross(v10, v20);

		// add normals to vertices
		// we'll average out the normals on the vertices later
		for (int ni : f->indices) {
			Vertex* v = m_vertices.at(ni);
			glm::vec3* vn = m_normals.at(ni);
			vn->x += f->normal.x;
			vn->y += f->normal.y;
			vn->z += f->normal.z;
			v->m_face_count += 1;
		}
	}

	// average out the normals
	for (size_t i = 0; i < m_vertices.size();i++) {
		Vertex* v = m_vertices.at(i);
		glm::vec3* vn = m_normals.at(i);

		assert(v->m_face_count > 0);
		vn->x /= v->m_face_count;
		vn->y /= v->m_face_count;
		vn->z /= v->m_face_count;
	}
}

void ClothMesh::create(std::vector<glm::vec2> &vertices, float segment_length, float tensile_strength)
{
	m_tensile_strength = tensile_strength;
	m_segment_length = segment_length;

	float minx = 9999999;// std::numeric_limits<float>::max();
	float maxx = -9999999;// std::numeric_limits<float>::min();
	float miny = 9999999;// std::numeric_limits<float>::max();
	float maxy = -9999999;// std::numeric_limits<float>::min();

	int i = 0;
	Face* face = new Face();
	m_faces.push_back(face);
	for (auto v : vertices) {
		if (v.x < minx)
			minx = v.x;
		if (v.x > maxx)
			maxx = v.x;
		if (v.y < miny)
			miny = v.y;
		if (v.y > maxy)
			maxy = v.y;
		
		m_vertices.push_back(new Vertex(v));
		face->indices.push_back(i++);
	}

	// cut polygon on the grid/horizontal
	for (float y = miny + segment_length; y <= maxy; y += segment_length) {
		cut_faces_Greiner_Hormann(m_vertices, m_faces, glm::vec2(-1e10, y), glm::vec2(1e10, y));
	}
	// cut polygon on the grid/vertical
	for (float x = minx + segment_length; x <= maxx; x += segment_length) {
		cut_faces_Greiner_Hormann(m_vertices, m_faces, glm::vec2(x, -1e10), glm::vec2(x, 1e10));
	}

	// just wiggle in the z coords to give cloth a better initial flow.
	// also add same amount of normals
	for (auto v: m_vertices) {
		v->z += (rand() % 10 - 5)/10.f;
		m_normals.push_back(new glm::vec3());
	}

	// create structural links for CLOTH
	// also calculate the face normal
	for (Face *f : m_faces) {
		// structural links and add normals to vertices
		// we'll average out the normals on the vertices later
		int pi = f->indices[f->indices.size() - 1];
		for (int ni : f->indices) {
			createLink(pi, ni);
			pi = ni;
		}
	}

	// update normals
	updateNormals();

	// create indices for OPENGL
	std::vector< unsigned int > indices;
	for (Face *f : m_faces) {
		if (f->indices.size() > 2)
		{
			for (size_t i = 1; i < f->indices.size()-1; i++) {
				indices.push_back(f->indices[0]);
				for (size_t j = i+1; j >= i; j--) {
					indices.push_back(f->indices[j]);
				}
			}
		}
	}

	m_draw_mode = GL_TRIANGLES;

	creategl(m_vertices, m_normals, indices, GL_DYNAMIC_DRAW);
}

void ClothMesh::reCreate(std::vector<glm::vec2>& vertices, float segment_length, float tensile_strength)
{
	clean();
	create(vertices, segment_length, tensile_strength);
}

void ClothMesh::constraint()
{
	int iterations = 6;

	for (int i = 0; i < iterations; i++) {
		for (Link* link: m_links) {
			Vertex *v1 = m_vertices[link->m_v1];
			Vertex* v2 = m_vertices[link->m_v2];

			glm::vec3 correction(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z);// = (glm::vec3) * v2 - (glm::vec3) * v1;
			float length = glm::length(correction);

			if (length > 0) {
				float adj = ((1 - link->m_length / length) * 0.5 * m_tensile_strength);
				correction.x *= adj;
				correction.y *= adj;
				correction.z *= adj;

				if (!v1->m_pinned) {
					//v1->operator+=(correction);
					v1->x += correction.x;
					v1->y += correction.y;
					v1->z += correction.z;
				}

				if (!v2->m_pinned) {
					//v1->operator+=(correction);
					v2->x -= correction.x;
					v2->y -= correction.y;
					v2->z -= correction.z;
				}

			}
		}
	}
}

void ClothMesh::update()
{
	float DAMPING = 0.01f;
	for (Vertex *v: m_vertices) {
		if (v->m_pinned)
			continue;

		glm::vec3 temp = *v;

		v->x += (v->x - v->m_previous.x)* (1 - DAMPING) + m_acceleration.x;
		v->y += (v->y - v->m_previous.y)* (1 - DAMPING) + m_acceleration.y;
		v->z += (v->z - v->m_previous.z)* (1 - DAMPING) + m_acceleration.z;

		v->m_previous.x = temp.x;
		v->m_previous.y = temp.y;
		v->m_previous.z = temp.z;
	}

	m_acceleration = glm::vec3();

	// update normals
	updateNormals();

	updategl(m_vertices, m_normals);
}

void ClothMesh::createLink(int v1, int v2)
{
	float len = glm::distance((glm::vec3)*m_vertices[v1], (glm::vec3) *m_vertices[v2]);
	// wxLogDebug("distance: %f", len);
	m_links.push_back(new Link(v1, v2, len));
}

void ClothMesh::clean()
{
	for (size_t i = 0; i < m_faces.size(); i++)
		delete m_faces[i];
	m_faces.clear();

	for (size_t i = 0; i < m_vertices.size(); i++)
		delete m_vertices[i];
	m_vertices.clear();

	for (size_t i = 0; i < m_normals.size(); i++)
		delete m_normals[i];
	m_normals.clear();

	for (size_t i = 0; i < m_links.size(); i++)
		delete m_links[i];
	m_links.clear();
}

