
#include "OpenGLContext.h"

#include <vector>
#include <fstream>
#include <sstream>

#include <wx/wx.h>
#include <csGL3DGrid.h>
#include <csGLRectangle.h>
#include <ClothMesh.h>

struct ShaderInfo {
	GLuint ShaderID;
	GLuint MxProjectionID;
	GLuint MxViewID;
	GLuint MxModelID;

	GLuint ShaderColorID;
	GLuint LightPositionID;
};

// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;

glm::mat4 Projection;
glm::mat4 Model = glm::mat4(1.0f);

// Camera matrix
glm::mat4 View;
glm::vec3 CameraPosition = glm::vec3(0, 150, 700);
glm::vec3 CameraLookAt = glm::vec3(0, 0, 0);
glm::vec3 CameraDirection = glm::vec3(0, 0, 0);
glm::vec3 CameraRight = glm::vec3(1, 0, 0);
glm::vec3 CameraUp = glm::vec3(0, 1, 0);

glm::vec3 Light = glm::vec3(200, 50, 200);

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
ShaderInfo LitShader, UnlitShader;

OpenGLContext::~OpenGLContext()
{
	clearObjects();
}

void OpenGLContext::clearObjects()
{
	for (csGL3DObject* o : m_rendered_objects)
		delete o;
	m_rendered_objects.clear();
}

int OpenGLContext::init()
{
	if (m_initialized) // if already initialized
		return 2;

	GLenum err = glewInit(); // after the window is created
	if (err) {
		wxLogFatalError("glewInit Error! %d", err);
		return 0;
	}

	// LOAD SHADERS
	// unlit shader
	UnlitShader.ShaderID = LoadShaders("UnLitVS.glsl", "UnLitFS.glsl");
	UnlitShader.MxProjectionID = glGetUniformLocation(UnlitShader.ShaderID, "projection");
	UnlitShader.MxViewID = glGetUniformLocation(UnlitShader.ShaderID, "view");
	UnlitShader.MxModelID = glGetUniformLocation(UnlitShader.ShaderID, "model");
	UnlitShader.ShaderColorID = glGetUniformLocation(UnlitShader.ShaderID, "shaderColor");

	// lit shader
	LitShader.ShaderID = LoadShaders("LitVS.glsl", "LitFS.glsl");
	LitShader.MxProjectionID = glGetUniformLocation(LitShader.ShaderID, "projection");
	LitShader.MxViewID = glGetUniformLocation(LitShader.ShaderID, "view");
	LitShader.MxModelID = glGetUniformLocation(LitShader.ShaderID, "model");
	LitShader.ShaderColorID = glGetUniformLocation(LitShader.ShaderID, "shaderColor");
	LitShader.LightPositionID = glGetUniformLocation(LitShader.ShaderID, "lightPosition");

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// these map direct to vertex shader variables
	m_initialized = true;

	return 1;
}

void OpenGLContext::resize(int width, int height)
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);

	glViewport(0, 0, width, height);
}

void OpenGLContext::setGrid(float width, float depth, float major)
{
	csGL3DGrid *grid = new csGL3DGrid();
	grid->create(width, depth, major);
	m_rendered_objects.push_back(grid);
}

ClothMesh* OpenGLContext::createCloth(float x1, float y1, float x2, float y2, float z, float segment_length, float tensile_strength)
{
	ClothMesh* cloth = new ClothMesh();
	cloth->create(x1, y1, x2, y2, z, segment_length, tensile_strength);
	m_rendered_objects.push_back(cloth);

	return cloth;
}

void OpenGLContext::render()
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

	// Render
	glClearColor(.8f, .8f, .8f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float color[3];
	for (csGL3DObject *ro : m_rendered_objects) {
		ro->get_color(color);

		if (ro->get_shader_type() == SHADER_LIT) {
			glUseProgram(LitShader.ShaderID);
			glUniformMatrix4fv(LitShader.MxProjectionID, 1, GL_FALSE, &Projection[0][0]);
			glUniformMatrix4fv(LitShader.MxViewID, 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(LitShader.MxModelID, 1, GL_FALSE, &Model[0][0]);
			
			glUniform3f(LitShader.ShaderColorID, color[0], color[1], color[2]);
			glUniform3f(LitShader.LightPositionID, Light.x, Light.y, Light.z);
		}
		else {
			glUseProgram(UnlitShader.ShaderID);
			glUniformMatrix4fv(UnlitShader.MxProjectionID, 1, GL_FALSE, &Projection[0][0]);
			glUniformMatrix4fv(UnlitShader.MxViewID, 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(UnlitShader.MxModelID, 1, GL_FALSE, &Model[0][0]);

			glUniform3f(UnlitShader.ShaderColorID, color[0], color[1], color[2]);
		}

		// pass matrices to vertex shader

		ro->render();

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
	}

	glUseProgram(0);
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
