
#include "OpenGLContext.h"

#include <vector>
#include <fstream>
#include <sstream>

#include <wx/wx.h>
#include <csGL3DGrid.h>
#include <ClothMesh.h>

struct ShaderInfo {
	GLuint ShaderID;
	GLuint MxProjectionID;
	GLuint MxViewID;
	GLuint MxModelID;

	GLuint ShaderColorID;
	GLuint LightPositionID;
};

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

	//glEnable(GL_POLYGON_SMOOTH);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// these map direct to vertex shader variables
	m_initialized = true;

	return 1;
}

void OpenGLContext::setGrid(float width, float depth, float major)
{
	csGL3DGrid *grid = new csGL3DGrid();
	grid->create(width, depth, major);
	m_rendered_objects.push_back(grid);
}

ClothMesh* OpenGLContext::createCloth(std::vector<glm::vec2> &vertices, std::vector<Polygon2>& polygons, float segment_length, float tensile_strength)
{
	ClothMesh* cloth = new ClothMesh();
	cloth->create(vertices, polygons, segment_length, tensile_strength);
	m_rendered_objects.push_back(cloth);

	return cloth;
}

void OpenGLContext::reCreateCloth(ClothMesh* mesh, std::vector<glm::vec2>& vertices, std::vector<Polygon2>& polygons, float segment_length, float tensile_strength)
{
	mesh->reCreate(vertices, polygons, segment_length, tensile_strength);
}

void OpenGLContext::render(World &world)
{
	// Render
	glClearColor(.8f, .8f, .8f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float color[3];
	for (csGL3DObject *ro : m_rendered_objects) {
		ro->get_color(color);

		if (ro->get_shader_type() == SHADER_LIT) {
			glUseProgram(LitShader.ShaderID);
			glUniformMatrix4fv(LitShader.MxProjectionID, 1, GL_FALSE, world.get_projection_matrix_address());
			glUniformMatrix4fv(LitShader.MxViewID, 1, GL_FALSE, world.get_view_matrix_address());
			glUniformMatrix4fv(LitShader.MxModelID, 1, GL_FALSE, world.get_model_matrix_address());
			
			glUniform3f(LitShader.ShaderColorID, color[0], color[1], color[2]);
			//glUniform3f(LitShader.LightPositionID, Light.x, Light.y, Light.z);
			glUniform3fv(LitShader.LightPositionID, 1, world.get_light_vector_address());
		}
		else {
			glUseProgram(UnlitShader.ShaderID);
			glUniformMatrix4fv(LitShader.MxProjectionID, 1, GL_FALSE, world.get_projection_matrix_address());
			glUniformMatrix4fv(LitShader.MxViewID, 1, GL_FALSE, world.get_view_matrix_address());
			glUniformMatrix4fv(LitShader.MxModelID, 1, GL_FALSE, world.get_model_matrix_address());

			glUniform3f(UnlitShader.ShaderColorID, color[0], color[1], color[2]);
		}

		//glEnable(GL_FLAT);
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
