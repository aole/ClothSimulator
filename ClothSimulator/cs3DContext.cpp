
#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cs3DContext.h"

#include <vector>
#include <fstream>
#include <sstream>

#include <wx/wx.h>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
void populate();


// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;

glm::mat4 Projection;
glm::vec3 CameraPosition = glm::vec3(0, 10, 35);
glm::vec3 CameraLookAt = glm::vec3(0, 0, 0);
glm::vec3 CameraDirection = glm::vec3(0, 0, 0);
glm::vec3 CameraRight = glm::vec3(1, 0, 0);
glm::vec3 CameraUp = glm::vec3(0, 0, 1);

// Camera matrix
glm::mat4 View = glm::lookAt(
	CameraPosition, // Camera position, in World Space
	glm::vec3(0, 0, 0), // and looks at the origin
	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
);

// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp;

GLuint MatrixID;
GLuint ShaderColorID;
GLuint VertexArrayID;
GLuint vertexbuffer; // vertex buffer identifier

std::vector< glm::vec3 > opengl_vertices;
std::vector< unsigned int > opengl_indices;

unsigned int num_grid_indices;

cs3DContext::cs3DContext(): m_initialized(false), m_programID(0)
{
}

void cs3DContext::init()
{
	if (m_initialized)
		return;

	GLenum err = glewInit(); // after the window is created
	if (err)
		wxLogFatalError("glewInit Error! %d", err);
	ShaderColorID = glGetUniformLocation(m_programID, "shaderColor");

	m_programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// generate 1 buffer
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	// Generate a buffer for the indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// these map direct to vertex shader variables
	MatrixID = glGetUniformLocation(m_programID, "MVP");
	ShaderColorID = glGetUniformLocation(m_programID, "shaderColor");

	populate();

	m_initialized = true;
}

void cs3DContext::resize(int width, int height)
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);

	glViewport(0, 0, width, height);
}

void cs3DContext::render()
{
	// setup camera view
	CameraDirection = glm::vec3(std::cos(verticalAngle) * std::sin(horizontalAngle),
		std::sin(verticalAngle),
		std::cos(verticalAngle) * std::cos(horizontalAngle));

	CameraRight = glm::vec3(
		std::sin(horizontalAngle - 3.14f / 2.0f),
		0,
		std::cos(horizontalAngle - 3.14f / 2.0f)
	);
	CameraUp = glm::cross(CameraRight, CameraDirection);

	View = glm::lookAt(
		CameraPosition, // Camera position, in World Space
		CameraPosition + CameraDirection, // and looks at the origin
		CameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);
	mvp = Projection * View * Model;

	glUseProgram(m_programID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Render
	glClearColor(.8f, .8f, .8f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw the grid
	glUniform3f(ShaderColorID, .1f, .1f, .1f);

	glDrawElements(
		GL_LINES,      // mode
		num_grid_indices,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);

}

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		wxLogFatalError("Cannot to open '%s'.", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else
	{
		wxLogFatalError("Cannot to open '%s'.", fragment_file_path);
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	wxLogDebug("\tCompiling vertex shader");
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		wxLogDebug(&VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	std::cout << "Compiling shader : " << fragment_file_path << std::endl;
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		wxLogDebug(&FragmentShaderErrorMessage[0]);
	}

	// Link the program
	wxLogDebug("\tLinking program.");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		wxLogDebug(&ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void populate()
{
	// create ground vertices
	opengl_vertices.clear();

	float minx = -10.0f;
	float minz = -10.0f;
	float maxx = 10.0f;
	float maxz = 10.0f;
	int xdiv = 5;
	int zdiv = 5;
	float dx = (maxx - minx) / (xdiv + 1);
	float dz = (maxz - minz) / (zdiv + 1);

	for (float x = minx; x <= maxx; x += dx)
	{
		for (float z = minz; z <= maxz; z += dz)
		{
			opengl_vertices.push_back(glm::vec3(x, 0, z));
		}
	}

	// create ground indices
	opengl_indices.clear();

	int totlines = (xdiv + 1) + 1;
	int x = 0;

	for (int i = 0;i < totlines;i++) {
		opengl_indices.push_back(x);
		opengl_indices.push_back(x + zdiv + 1);

		x += zdiv + 1 + 1;
	}

	totlines = (zdiv + 1) + 1;
	int z = 0;

	for (int i = 0;i < totlines;i++) {
		opengl_indices.push_back(z);
		opengl_indices.push_back(z + (zdiv + 2) * (xdiv + 1));

		z += 1;
	}

	num_grid_indices = opengl_indices.size();

	// pass to OpenGL
	glBufferData(GL_ARRAY_BUFFER, opengl_vertices.size() * sizeof(glm::vec3), &opengl_vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, opengl_indices.size() * sizeof(unsigned int), &opengl_indices[0], GL_STATIC_DRAW);
}
