#include <stdio.h>
#include <fstream>
#include <string>
#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "graphicsmath.h"
#include "transformations.h"
#include "camera.h"
#include "texture.h"

using namespace std;

struct Vertex
{
	Vector3f pos;
	Vector2f uv;

	Vertex(){}
	Vertex(Vector3f _pos, Vector2f _uv)
	{
		pos = _pos;
		uv = _uv;
	}
};

GLuint VBO;
GLuint IBO;
GLuint gSampler;
Texture* pTexture = NULL;

//Memory position of World Matrix on the GPU
GLuint gWorldLocation;

Camera* gameCamera = NULL;

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

static bool ReadFile(const char* pFileName, string& outFile)
{
	ifstream f(pFileName);
	bool ret = false;

	if (f.is_open())
	{
		string line;
		while (getline(f, line))
		{
			outFile.append(line);
			outFile.append("\n");
		}
		f.close();
		ret = true;
	}
	else
	{
		fprintf(stderr, "unable to open file '%s'\n", pFileName);
	}

	return ret;
}

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//Static scale variable
	static float Scale = 0.0f;

	static float Rotation = 0.0f;

	Transform transform;
	transform.Position(-1.5f, -1.0f, 5.0f);
	transform.Rotation(0.f, 0.f, 0.f);
	transform.SetPerspective(90.f, 1024, 768, 1.f, 1000.f);
	transform.SetCamera(*gameCamera);

	//Update world matrix value on GPU
	
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)transform.GetWorldViewProjectionTransform());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	//Bind the Index buffer to the pipeline
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	pTexture->Bind(GL_TEXTURE0);
	//Draw the indices
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

static void AddShader(GLuint ShaderProgram, const char* ShaderText,
	GLenum ShaderType)
{
	//Create the shader for storage
	//GLuint used for communication with GPU
	//Position on GPU
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0)
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* ShaderSource[1];
	ShaderSource[0] = ShaderText;
	GLint lengths[1];
	lengths[0] = strlen(ShaderText);
	//Provide GL with shader source
	glShaderSource(ShaderObj, 1, ShaderSource, lengths);
	//Compile shader using shader source (stored in ShaderObj)
	glCompileShader(ShaderObj);

	//Confirm shader compiled successfully
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, infoLog);
		fprintf(stderr, "Error compiling shader type: %d: '%s'",
			ShaderType, infoLog);
	}

	//Attach shader to shader program (collection of shaders)
	glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
	//Create shader program (collection of shaders)
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string vs;
	string fs;
	//Read vertex shader from file system
	if (!ReadFile(pVSFileName, vs))
	{
		exit(1);
	}
	//Read fragment shader from file system
	if (!ReadFile(pFSFileName, fs))
	{
		exit(1);
	}
	//Compile vertex and fragment shaders and add to the shader program
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };
	//Tell OpenGL about the shader program and confirm its status
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(errorLog),
			NULL, errorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", 
			errorLog);
		exit(1);
	}
	//Ensure shader program is valid, and confirm status
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(errorLog),
			NULL, errorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n",
			errorLog);
		exit(1);
	}
	//Tell OpenGL to use this shader
	glUseProgram(ShaderProgram);

	//Allocate and get memory location of World Matrix on GPU
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
	/*Vertex Vertices[4] = {
		Vertex(Vector3f(-1.f, -1.f, 0.5773f), Vector2f(0.f, 0.f)),
		Vertex(Vector3f(0.f, -1.f, -1.15475f), Vector2f(0.5f, 0.f)),
		Vertex(Vector3f(1.f, -1.f, 0.5773f), Vector2f(1.f, 0.f)),
		Vertex(Vector3f(0.f, 1.f, 0.f), Vector2f(0.5f, 1.f))
	};*/

	Vertex Vertices[4] = {
		Vertex(Vector3f(0.f, 0.f, 0.f), Vector2f(0.f, 0.f)),
		Vertex(Vector3f(1.f, 0.f, 0.f), Vector2f(1.f, 0.f)),
		Vertex(Vector3f(1.f, 1.f, 0.f), Vector2f(1.f, 1.f)),
		Vertex(Vector3f(0.f, 1.f, 0.f), Vector2f(0.f, 1.f))
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
	//List of Indices to draw
	unsigned int Indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	//Generate Index Buffer
	glGenBuffers(1, &IBO);
	//Bind buffer to GL Element Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//Send Indices to GL Element Array
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 03");

	InitializeGlutCallbacks();

	gameCamera = new Camera(1024, 768);

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	//Cull the back face of geometry
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	CreateVertexBuffer();

	CreateIndexBuffer();

	CompileShaders();

	glUniform1i(gSampler, 0);

	glEnable(GL_TEXTURE_2D);

	pTexture = new Texture(GL_TEXTURE_2D, "../Content/test.png");

	if (!pTexture->Load())
	{
		return 1;
	}

	glutMainLoop();

	return 0;
}