// source file for shader class
#include "shaders.h"

constexpr uint16_t log_length = 512;


// constructor
Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath, bool& success) {
	// initialize success to true before reset of function
	success = true;

	// strings to read file contents into
	std::string vertexShaderString, fragmentShaderString;

	// read files to string checking return values
	if (
		!readFileToString(vertexShaderPath, vertexShaderString) ||
		!readFileToString(fragmentShaderPath, fragmentShaderString))
	{
		success = false;
		return;
	}

	// convert C++ strings to C const char* pointers
	const char* vertexShaderCstr = vertexShaderString.c_str();
	const char* fragmentShaderCstr = fragmentShaderString.c_str();

	// create empty shader objects
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// check objects were created
	if (!vertexShader || !fragmentShader) {
		std::cout << "Failed to create Shader objects\n";
		success = false;
		goto clean;
	}

	// attach source code to shader objects
	glShaderSource(vertexShader, 1, &vertexShaderCstr, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderCstr, NULL);

	// compile shader source code
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// check for any compile errors and post errors in console
	int32_t vertexStatus, fragmentStatus;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

	// log errors to user
	// do not immediately return if errors detected
	if (!vertexStatus) {
		char log[log_length];
		// get full error message
		glGetShaderInfoLog(vertexShader, log_length, NULL, log);
		std::cout << "Vertex shader compile failed:\n" << log << "\n";
		success = false;
	}
	if (!fragmentStatus) {
		char log[log_length];
		// get full error message
		glGetShaderInfoLog(fragmentShader, log_length, NULL, log);
		std::cout << "fragment shader compile failed:\n" << log << "\n";
		success = false;
	}

	// once errors of both programs have been reported,
	// return
	if (!success) goto clean;


	// create shader program object
	shaderProgram = glCreateProgram();
	// return if null
	if (!shaderProgram) {
		std::cout << "Failed to create Shader program object\n";
		success = false;
		goto clean;
	}

	// attach compiled files to program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	
	// link program, then check for errors
	glLinkProgram(shaderProgram);

	int32_t linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus) {
		char log[log_length];
		std::cout << "Shader program linking failed:\n" << log << "\n";
		success = false;
		goto clean;
	}

	// I know goto is 'bad practice'
	// but I find it extremely useful for jumping to a cleanup section
clean:
	// clean up before exiting
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// program destructor
// detetes shader program
Shader::~Shader() {
	glDeleteProgram(shaderProgram);
}


void Shader::use() {
	glUseProgram(shaderProgram);
}


// procedures for setting uniforms on the shader program
// each of these procedures gets the location of the selected uniform
// and then sets its value

void Shader::setFloat(const char* name, float value) {
	glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setVec2(const char* name, float* value) {
	glUniform2fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setVec3(const char* name, float* value) {
	glUniform3fv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setVec4(const char* name, float* value) {
	glUniform4fv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setInt(const char* name, int32_t value) {
	glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setIvec2(const char* name, int32_t* value) {
	glUniform2iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setIvec3(const char* name, int32_t* value) {
	glUniform3iv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setIvec4(const char* name, int32_t* value) {
	glUniform4iv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setUint(const char* name, uint32_t value) {
	glUniform1ui(glGetUniformLocation(shaderProgram, name), value);
}

void Shader::setUvec2(const char* name, uint32_t* value) {
	glUniform2uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setUvec3(const char* name, uint32_t* value) {
	glUniform3uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}

void Shader::setUvec4(const char* name, uint32_t* value) {
	glUniform4uiv(glGetUniformLocation(shaderProgram, name), 1, value);
}


void Shader::setMat2(const char* name, float* value) {
	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::setMat3(const char* name, float* value) {
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}

void Shader::setMat4(const char* name, float* value) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, value);
}