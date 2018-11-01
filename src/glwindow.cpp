#include "glwindow.h"

#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>
#include <gl/glew.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

SDL_Window* sdlWnd = NULL;
SDL_Window *dummyWnd = NULL;

int glwindow_width = 300;
int glwindow_height = 400;

bool initialized = FALSE;

static const GLfloat g_vertex_buffer_data[] = {
   -.5, 0, 0.0,
   0.5, 0, 0.0,
   0.0,  0.5, 0.0,
};

GLuint VertexArrayID;
GLuint vertexbuffer; // vertex buffer identifier

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection;

// Camera matrix
glm::mat4 View = glm::lookAt(
    glm::vec3(0,0,-5), // Camera position, in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp;

GLuint programID;
GLuint MatrixID;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		std::cout<<"Cannot to open '"<<vertex_file_path <<"'.\n" << std::endl;
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	std::cout<<"Compiling shader : "<<vertex_file_path<<std::endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		std::cout<<&VertexShaderErrorMessage[0]<<std::endl;
	}

	// Compile Fragment Shader
	std::cout<<"Compiling shader : "<<fragment_file_path<<std::endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		std::cout<< &FragmentShaderErrorMessage[0]<<std::endl;
	}

	// Link the program
	std::cout<<"Linking program\n";
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout<<&ProgramErrorMessage[0]<<std::endl;
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void init()
{
    programID = LoadShaders( "VertexShader.glsl", "FragmentShader.glsl" );

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	MatrixID = glGetUniformLocation(programID, "MVP");

	Projection = glm::perspective(glm::radians(45.0f), (float) 4 / (float) 3, 0.1f, 100.0f);
	mvp = Projection * View * Model;

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

	// generate 1 buffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// pass to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

}

void display()
{
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glClearColor(.8,.8,.8, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

    SDL_GL_SwapWindow(sdlWnd);
}

LRESULT CALLBACK GLProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
    {
        //Destroy window
        SDL_DestroyWindow( dummyWnd );
        dummyWnd = NULL;

        SDL_DestroyWindow( sdlWnd );
        sdlWnd = NULL;

        //Quit SDL subsystems
        SDL_Quit();

    }
    return 0;
    case WM_PAINT:
        if(initialized)
            display();
        break;
    case WM_SIZE:
        RECT r;
        GetClientRect(hwnd,&r);
        glwindow_width = r.right - r.left;
        glwindow_height = r.bottom - r.top;

        PostMessage(hwnd, WM_PAINT, 0, 0);
        return 0;

    case WM_LBUTTONDOWN:
        std::cout<<"mouse down"<<std::endl;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}


HWND GLWindow::create(HWND hWndParent, HINSTANCE hInstance)
{
    TCHAR szClassName[ ] = L"OpenGLWindow";

    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = GLProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS|CS_OWNDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    RegisterClassEx (&wincl);

    HWND hwnd = this->hwnd = CreateWindowEx (
                                 WS_EX_CLIENTEDGE,
                                 szClassName,
                                 szClassName,
                                 WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                 x,
                                 y,
                                 w,                 /* The programs width */
                                 h,                 /* and height in pixels */
                                 hWndParent,        /* The window is a child-window */
                                 NULL,
                                 GetModuleHandle(NULL),       /* Program Instance handler */
                                 NULL                 /* No Window Creation data */
                             );

    if (SDL_InitSubSystem(SDL_INIT_VIDEO)<0)
        //if (SDL_Init(SDL_INIT_VIDEO)<0)
        std::cout<<"SDL_InitSubSystem error"<<std::endl;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    dummyWnd = SDL_CreateWindow("a", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
                               );
    if(!dummyWnd)
        std::cout<<"Create dummy Window: "<<SDL_GetError()<<std::endl;

    char sBuf[32];
    std::sprintf(sBuf, "%p", dummyWnd);

    SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, sBuf);

    sdlWnd = SDL_CreateWindowFrom(hwnd);
    if(!sdlWnd)
        std::cout<<"SDL_CreateWindowFrom error"<<std::endl;

    SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, nullptr);

    // below crashes!!!
    //SDL_DestroyWindow(isdlw);

    SDL_GLContext context = SDL_GL_CreateContext(sdlWnd);
    if(!context)
        std::cout<<"Context:"<<SDL_GetError()<<std::endl;

    GLenum err  = glewInit();
    if(err)
        std::cout<<"glewInit error"<<std::endl;

    initialized = TRUE;

    init();

    ShowWindow (hwnd, SW_SHOWDEFAULT);

    return hwnd;
}
