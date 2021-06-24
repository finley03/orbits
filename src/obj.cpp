// source file for obj.h

#include "obj.h"

// Increment pointer to beyond whitespace
inline void ptrws(const char*& strptr) {
	while (*strptr == ' ') ++strptr;
}

// Find next white space on character pointer
inline void ptrnextws(const char*& strptr) {
	strptr = strchr(strptr, ' ');
}

// Find next character value on pointer
// increments past as many whitespace as required
inline void ptrnextvalue(const char*& strptr) {
	strptr = strchr(strptr, ' ');
	while (*strptr == ' ') ++strptr;
}

// Increment pointer beyond selected character to remove
inline void ptrrm(const char*& strptr, char rmchar) {
	while (*strptr == rmchar) ++strptr;
}

// find next character value on pointer with selected
// seperating value
// only increments past one of the separating value
inline void ptrnextvalue(const char*& strptr, char seperator) {
	strptr = strchr(strptr, seperator);
	++strptr;
}

// return folder path from complete file path
inline void getFolderPath(const char* path, char* writeback) {
	// use some simple pointer arithmetic to get length of path
	uint16_t pathLength = strrchr(path, '/') - path + 1;
	// copy string of path length to destination
	strncpy(writeback, path, pathLength);
}

std::vector<float> OBJ_GenMesh(const char* filePath, uint32_t& size, bool& status) {
	// read file to string and convert to C const char*
	std::string fileString;
	status = readFileToString(filePath, fileString);
	if (!status) return {};
	const char* file = fileString.c_str();


	// I have to use some form of dynamic data type for storing
	// interpreted data. There are faster methods than std::vector,
	// but it's probably not worth the effort.
	std::vector<float> vertices, vertexNormals, textureCoords;
	std::vector<uint32_t> vertexIndexes, normalIndexes, textureIndexes;

	// pointer to first character of line in string
	const char* lineptr = file;
	// iterate over lines in the file
	// I will use C types to improve preformance
	// get pointer to first character of line
	while (lineptr = strchr(lineptr, '\n')) {
		++lineptr;
		// remove whitespace at beginning of line
		ptrws(lineptr);
		// check if line is empty
		// i.e. check if next char is newline
		if (*lineptr == '\n') continue;

		// switch the first character of the line
		// only compare one character for speed
		switch (*lineptr) {
		// case for vertex data
		case 'v':
			// increment pointer to next character
			++lineptr;
			switch (*lineptr) {
				// case for vertex data
				// command v
			case ' ':
				// preemptively allocate memory in vector
				// improves speed of push_back()
				vertices.reserve(3);
				// push back three coordinates of vertex
				ptrnextvalue(lineptr);
				vertices.push_back(atof(lineptr));
				ptrnextvalue(lineptr);
				vertices.push_back(atof(lineptr));
				ptrnextvalue(lineptr);
				vertices.push_back(atof(lineptr));
				break;

				// case for vertex normal data
				// command vn
			case 'n':
				// allocate memory for push_back()
				vertexNormals.reserve(3);
				ptrnextvalue(lineptr);
				vertexNormals.push_back(atof(lineptr));
				ptrnextvalue(lineptr);
				vertexNormals.push_back(atof(lineptr));
				ptrnextvalue(lineptr);
				vertexNormals.push_back(atof(lineptr));
				break;

				// case for vertex texture data
				// command vt
			case 't':
				// reserve memory
				textureCoords.reserve(2);
				ptrnextvalue(lineptr);
				textureCoords.push_back(atof(lineptr));
				ptrnextvalue(lineptr);
				textureCoords.push_back(atof(lineptr));
				break;

				// case for unknown command
			default:
				std::cout << "Warning: " << filePath << " contains unknown parameters\n";
				break;
			}
			break;

			// case for polygonal face element
		case 'f':
			// in OBJ files, face elements are represented as
			// f vertex1/vertexTexture1/vertexNormal1 vertex1/.......
			// loop through face elements
			// subtract 1 from indexes, as indexing in .obj starts from 1
			for (uint8_t i = 0; i < 3; ++i) {
				ptrnextvalue(lineptr);
				vertexIndexes.push_back(atoi(lineptr) - 1);
				ptrnextvalue(lineptr, '/');
				textureIndexes.push_back(atoi(lineptr) - 1);
				ptrnextvalue(lineptr, '/');
				normalIndexes.push_back(atoi(lineptr) - 1);
			}
			break;

			// case for comment
		case '#':
			break;

			// case for mtllib command
		case 'm':
			// check command is acually mtllib
			if (strncmp(lineptr, "mtllib", 6) == 0) {

			}
			else {
				std::cout << "Warning: " << filePath << " contains unknown parameters\n";
			}
			break;

			// case for usemtl command
		case'u':
			// check command is acutally usemtl
			if (strncmp(lineptr, "usemtl", 6) == 0) {

			}
			else {
				std::cout << "Warning: " << filePath << " contains unknown parameters\n";
			}
			break;

			// case for object name declaration
		case 'o':			
			break;

			// case for group name
		case 'g':
			break;

			// case for smooth shading
		case 's':
			break;

		default:
			std::cout << "Warning: " << filePath << " contains unknown parameters\n";
			break;
		}
	}

	
	// final data structure
	std::vector<float> data;
	// reserve data in vector
	data.reserve(vertexIndexes.size() * 8);

	// return number of vertices
	size = vertexIndexes.size();
	// loop through all vertexes to arrange data
	for (uint32_t i = 0; i < size; ++i) {
		// push vertex coordinates
		data.push_back(vertices[vertexIndexes[i] * 3]);
		data.push_back(vertices[vertexIndexes[i] * 3 + 1]);
		data.push_back(vertices[vertexIndexes[i] * 3 + 2]);

		// push vertex normal vectors
		data.push_back(vertexNormals[normalIndexes[i] * 3]);
		data.push_back(vertexNormals[normalIndexes[i] * 3 + 1]);
		data.push_back(vertexNormals[normalIndexes[i] * 3 + 2]);

		// push vertex texture coordinates
		data.push_back(textureCoords[textureIndexes[i] * 2]);
		data.push_back(textureCoords[textureIndexes[i] * 2 + 1]);
	}

	status = true;
	return data;
}


std::string OBJ_GetMTLpath(const char* filePath, bool& status) {
	// open .obj file path
	std::string fileString;
	status = readFileToString(filePath, fileString);
	if (!status) return {};
	const char* file = fileString.c_str();

	status = false;
	// pointer to current working character
	const char* lineptr = file;

	std::string fileName;

	while (lineptr = strchr(lineptr, '\n')) {
		// advance to first character of line
		++lineptr;

		// remove whitespace at beginning of line
		ptrws(lineptr);
		// check if line is empty
		// i.e. check if next char is newline
		if (*lineptr == '\n') continue;

		if (*lineptr == 'm') {
			if (strncmp(lineptr, "mtllib", 6) == 0) {
				ptrnextvalue(lineptr);
				// copy file name back to writeback
				// strcspn gets size of string to copy
				fileName.assign(lineptr, strcspn(lineptr, " ") - 2);
				//strncpy(writeback, lineptr, strcspn(lineptr, " "));
				status = true;
				break;
			}
		}
	}

	return fileName;
}


std::vector<OBJ_Material> OBJ_GetMaterials(const char* filePath, bool& status) {
	return {};
}


std::vector<uint32_t> OBJ_GetMaterialIndexes(const char* filePath, bool& success) {
	return {};
}