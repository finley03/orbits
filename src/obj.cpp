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
inline std::string getFolderPath(const char* path) {
	// use some simple pointer arithmetic to get length of path
	UINT_T pathLength = strrchr(path, '/') - path + 1;
	// copy string of path length to destination
	std::string filePath;
	filePath.assign(path, pathLength);
	return filePath;
}

// function to warn user of unknown parameters in obj or mtl file
void warnUnknownParam(const char* filePath, const char* strptr) {
	// find length of command
	unsigned int length = strcspn(strptr, " \n");
	// print unknown command
	// I am using printf because it allows for easier formatting
	// than std::cout including %.*s which allows you to choose how many
	// characters of a string to print.
	printf("Warning: %s has unknown parameter: %.*s\n", filePath, length, strptr);
}

// function to warn user of unknown parameters in obj or mtl file
void warnUnknownParam(std::string filePath, const char* strptr) {
	// find length of command
	unsigned int length = strcspn(strptr, " \n");
	// print unknown command
	// I am using printf because it allows for easier formatting
	// than std::cout including %.*s which allows you to choose how many
	// characters of a string to print.
	printf("Warning: %s has unknown parameter: %.*s\n", filePath.c_str(), length, strptr);
}

// due to the massive number of shared variables, it just makes sense to
// put this code in one function rather than divide it up.
OBJ_Data OBJ_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status) {
	// read file to string and convert to C const char*
	std::string fileString;
	std::string mtlFileString;
	status = readFileToString(filePath, fileString);
	if (!status) return {};
	// make sure there is a newline character at the start of every line
	fileString.insert(0, "\n");
	const char* file = fileString.c_str();
	const char* mtlFile;


	// I have to use some form of dynamic data type for storing
	// interpreted data. There are faster methods than std::vector,
	// but it's probably not worth the effort.
	std::vector<float> vertices, vertexNormals, textureCoords;
	std::vector<UINT_T> vertexIndexes, normalIndexes, textureIndexes;

	// string to store mtl file path
	std::string mtlFilePath;

	// initialize size to zero
	size = 0;

	// data type to store material uses
	// first part of pair is material name
	// second is starting vertex index
	// this will used for linking with mtl file later
	//std::map<std::string, UINT_T> materialUses;
	std::vector<std::pair<std::string, UINT_T>> materialUses;
	// map for material declarations
	// first part is the material name
	// second part is the index in the material storing vector
	std::map<std::string, UINT_T> materialDeclarations;

	// folder path to obj root directory
	std::string folderPath = getFolderPath(filePath);

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

		// keep track of number of triangles in line
		UINT_T nrTriangles = 0;

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
				// decrement pointer to first command character
				warnUnknownParam(filePath, --lineptr);
				break;
			}
			break;

			// case for polygonal face element
		case 'f':
			// in OBJ files, face elements are represented as
			// f vertex1/vertexTexture1/vertexNormal1 vertex1/.......
			// loop through face elements
			// subtract 1 from indexes, as indexing in .obj starts from 1
			//for (INT_T i = 0; i < 3; ++i) {
			//	ptrnextvalue(lineptr);
			//	vertexIndexes.push_back(_INT atoi(lineptr) - 1);
			//	ptrnextvalue(lineptr, '/');
			//	textureIndexes.push_back(_INT atoi(lineptr) - 1);
			//	ptrnextvalue(lineptr, '/');
			//	normalIndexes.push_back(_INT atoi(lineptr) - 1);
			//}


			UINT_T vertexes[3], textures[3], normals[3];
			// get first values
			ptrnextvalue(lineptr);
			vertexes[0] = _INT(atoi(lineptr)) - 1;
			ptrnextvalue(lineptr, '/');
			textures[0] = _INT(atoi(lineptr)) - 1;
			ptrnextvalue(lineptr, '/');
			normals[0] = _INT(atoi(lineptr)) - 1;
			// get second values
			ptrnextvalue(lineptr);
			vertexes[1] = _INT(atoi(lineptr)) - 1;
			ptrnextvalue(lineptr, '/');
			textures[1] = _INT(atoi(lineptr)) - 1;
			ptrnextvalue(lineptr, '/');
			normals[1] = _INT(atoi(lineptr)) - 1;

			const char* spacePtr;

			do {
				// reserve memory
				vertexIndexes.reserve(3);
				textureIndexes.reserve(3);
				normalIndexes.reserve(3);

				// only shift data on repeats
				if (nrTriangles++) {
					vertexes[1] = vertexes[2];
					textures[1] = textures[2];
					normals[1] = normals[2];
				}

				// get third values
				ptrnextvalue(lineptr);
				vertexes[2] = _INT(atoi(lineptr)) - 1;
				ptrnextvalue(lineptr, '/');
				textures[2] = _INT(atoi(lineptr)) - 1;
				ptrnextvalue(lineptr, '/');
				normals[2] = _INT(atoi(lineptr)) - 1;

				// push data of triangle
				for (INT_T i = 0; i < 3; ++i) {
					vertexIndexes.push_back(vertexes[i]);
					textureIndexes.push_back(textures[i]);
					normalIndexes.push_back(normals[i]);
				}

				size += 3;

				spacePtr = strchr(lineptr, ' ');
				// repeat if there are more values
			} while (spacePtr != NULL && strpbrk(spacePtr, "0123456789.") < strchr(lineptr, '\n'));

			//size += 3;
			break;

			// case for comment (unused)
		case '#':
			break;

			// case for mtllib command
		case 'm':
			// check command is acually mtllib
			// mtllib declares a material library file
			if (strncmp(lineptr, "mtllib", 6) == 0) {
				// get mtl file path
				ptrnextvalue(lineptr);
				mtlFilePath = folderPath;
				mtlFilePath.append(lineptr, strcspn(lineptr, " \n"));
				// load file
				status = readFileToString(mtlFilePath.c_str(), mtlFileString);
				if (!status) return {};
				mtlFileString.insert(0, "\n");
			}
			else {
				warnUnknownParam(filePath, lineptr);
			}
			break;

			// case for usemtl command
		case'u':
			// check command is acutally usemtl
			if (strncmp(lineptr, "usemtl", 6) == 0) {
				ptrnextvalue(lineptr);
				// create data type for material name
				std::string materialName;
				materialName.assign(lineptr, strcspn(lineptr, " \n"));
				// create new entry
				materialUses.push_back(std::make_pair(materialName, size));
			}
			else {
				warnUnknownParam(filePath, lineptr);
			}
			break;

			// case for object name declaration (unused)
		case 'o':			
			break;

			// case for group name (unused)
		case 'g':
			break;

			// case for smooth shading (unused)
		case 's':
			break;

			// case for null terminator character (unused)
		case '\0':
			break;

		default:
			//std::cout << "Warning: " << filePath << " contains unknown parameters\n";
			warnUnknownParam(filePath, lineptr);
			break;
		}
	}


	// return data type
	OBJ_Data objectData;

	// create alias for materials
	std::vector<OBJ_Material>& materials = objectData.materials;


	// if material file was found, open file and find material definitions.
	if (!mtlFileString.empty()) {
		// assign mtlFile to new const char*
		mtlFile = mtlFileString.c_str();
		// assign scanning poniter to beginning of mtl file
		lineptr = mtlFile;

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
				// case for newmtl
			case 'n':
				if (strncmp(lineptr, "newmtl", 6) == 0) {
					// get name of new material
					ptrnextvalue(lineptr);
					// create data type for material name
					std::string materialName;
					materialName.assign(lineptr, strcspn(lineptr, " \n"));
					// push material declaration for linking later
					// index is current size of vector
					materialDeclarations[materialName] = materials.size();
					// push new material data structure
					materials.push_back(OBJ_Material());
				}
				else {
					warnUnknownParam(mtlFilePath, lineptr);
				}
				break;

				// case for color coefficients
			case 'K':
				++lineptr;
				switch (*lineptr) {
					// case for ambient
				case 'a':
					ptrnextvalue(lineptr);
					materials.back().ambient[0] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().ambient[1] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().ambient[2] = atof(lineptr);
					break;

					// case for diffuse
				case 'd':
					ptrnextvalue(lineptr);
					materials.back().diffuse[0] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().diffuse[1] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().diffuse[2] = atof(lineptr);
					break;

					// case for specular
				case 's':
					ptrnextvalue(lineptr);
					materials.back().specular[0] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().specular[1] = atof(lineptr);
					ptrnextvalue(lineptr);
					materials.back().specular[2] = atof(lineptr);
					break;

					// case for emmission (unused)
				case 'e':
					break;

				default:
					warnUnknownParam(mtlFilePath, --lineptr);
					break;
				}
				break;

				// case for N
			case 'N':
				++lineptr;
				switch (*lineptr) {
					// case for specular exponent
				case 's':
					ptrnextvalue(lineptr);
					materials.back().specularExponent = atof(lineptr);
					break;

					// case for index of refraction (unused)
				case 'i':
					break;

				default:
					warnUnknownParam(mtlFilePath, --lineptr);
					break;
				}
				break;


			case 'm':
				// check if command starts with 'map_K'
				if (strncmp(lineptr, "map_K", 5) == 0) {
					// advance pointer to next distinguishing character
					lineptr += 5;
					switch (*lineptr) {
						// case for ambient texture map
						// map_Ka
					case 'a':
					{
						ptrnextvalue(lineptr);
						//materials.back().ambientTexturePath.assign(folderPath);
						//materials.back().ambientTexturePath.append(lineptr, strcspn(lineptr, " \n"));
						std::string texPath;
						texPath.assign(folderPath);
						texPath.append(lineptr, strcspn(lineptr, " \n"));
						textures.loadImageToBuffer(texPath.c_str(), materials.back().ambientTexturePointer);
						materials.back().textureAmbient = true;
						break;
					}

						// case for diffuse texture map
						// map_Kd
					case 'd':
					{
						ptrnextvalue(lineptr);
						//materials.back().diffuseTexturePath.assign(folderPath);
						//materials.back().diffuseTexturePath.append(lineptr, strcspn(lineptr, " \n"));
						std::string texPath;
						texPath.assign(folderPath);
						texPath.append(lineptr, strcspn(lineptr, " \n"));
						textures.loadImageToBuffer(texPath.c_str(), materials.back().diffuseTexturePointer);
						//uint8_t* buffer = nullptr;
						//INT_T size;
						//readFileToByteArray(texPath, buffer, size);
						//textures.parseImageToBuffer(buffer, size, materials.back().diffuseTexturePointer);
						//textures.loadImageToBuffer(texPath.c_str(), materials.back().diffuseTexturePointer);
						materials.back().textureDiffuse = true;
						break;
					}

						// case for specular texture map
						// map_Ks
					case 's':
					{
						ptrnextvalue(lineptr);
						//materials.back().specularTexturePath.assign(folderPath);
						//materials.back().specularTexturePath.append(lineptr, strcspn(lineptr, " \n"));
						std::string texPath;
						texPath.assign(folderPath);
						texPath.append(lineptr, strcspn(lineptr, " \n"));
						textures.loadImageToBuffer(texPath.c_str(), materials.back().specularTexturePointer);
						materials.back().textureSpecular = true;
						break;
					}

					default:
						warnUnknownParam(mtlFilePath, lineptr - 5);
						break;
					}
				}
				// test of command starts with map_
				else if (strncmp(lineptr, "map_", 4) == 0) {
					lineptr += 4;
					switch (*lineptr) {
						// case for specular highlight map (unused)
						// map_Ns
					case 'N':
						break;

						// case for alpha map (unused)
						// map_d
					case 'd':
						break;

						// case for bump map (unused)
						// map_bump
					case 'b':
						break;

						// case for bump map (unused)
						// I found some files that used this unofficial implementation
						// map_Bump
					case 'B':
						break;

					default:
						warnUnknownParam(mtlFilePath, lineptr - 4);
						break;
					}
				}
				else {
					warnUnknownParam(mtlFilePath, lineptr);
				}
				break;

				// case for alpha
			case 'd':
				ptrnextvalue(lineptr);
				materials.back().alpha = atof(lineptr);
				break;

			case 'T':
				++lineptr;
				switch (*lineptr) {
					// case for alpha (second implementation)
				case 'r':
					ptrnextvalue(lineptr);
					materials.back().alpha = atof(lineptr);
					break;

					// case for transmission filter color (unused)
				case 'f':
					break;

				default:
					warnUnknownParam(mtlFilePath, --lineptr);
					break;
				}
				break;

			case 's': // NON_STANDARD .OBJ PARAMETER, ONLY SPECIFIC TO THIS PROGRAM
				// check if command starts with 'shader_'
				if (strncmp(lineptr, "shader_", 7) == 0) {
					lineptr += 7;
					switch (*lineptr) {
					case 'V': // case for vertex shader
						materials.back().customShader = true;
						ptrnextvalue(lineptr);
						materials.back().vertexShaderPath.assign(folderPath);
						materials.back().vertexShaderPath.append(lineptr, strcspn(lineptr, " \n"));
						std::cout << materials.back().vertexShaderPath << "\n";
						break;

					case 'F': // case for fragment shader
						materials.back().customShader = true;
						ptrnextvalue(lineptr);
						materials.back().fragmentShaderPath.assign(folderPath);
						materials.back().fragmentShaderPath.append(lineptr, strcspn(lineptr, " \n"));
						std::cout << materials.back().fragmentShaderPath << "\n";
						break;

					default:
						warnUnknownParam(mtlFilePath, lineptr - 7);
					}
				}
				break;

			default:
				warnUnknownParam(mtlFilePath, lineptr);
				break;
			}
		}

		// link .obj material uses to .mtl material declarations
		std::vector<std::pair<UINT_T, UINT_T>>& matIndexes = objectData.matIndexes;
		for (INT_T i = 0; i < materialUses.size(); ++i) {
			// check if material definition exists
			if (!materialDeclarations.count(materialUses[i].first)) {
				std::cout << "Error: " << filePath << " uses materials undefined in " << mtlFilePath << "\n";
				status = false;
				return {};
			}
			// create link data type
			std::pair<UINT_T, UINT_T> matLink;
			// set first value of link to index of starting vertex
			matLink.first = materialUses[i].second;
			// set second value of link to index of material
			matLink.second = materialDeclarations[materialUses[i].first];
			// push matlink to matIndexes
			matIndexes.push_back(matLink);
		}
	}

	
	// create alias
	std::vector<float>& mesh = objectData.mesh;
	// reserve data in vector
	mesh.reserve(vertexIndexes.size() * 8);

	// return number of vertices
	//size = vertexIndexes.size();
	// loop through all vertexes to arrange data
	for (UINT_T i = 0; i < size; ++i) {
		// push vertex coordinates
		mesh.push_back(vertices[vertexIndexes[i] * 3]);
		mesh.push_back(vertices[vertexIndexes[i] * 3 + 1]);
		mesh.push_back(vertices[vertexIndexes[i] * 3 + 2]);

		// push vertex normal vectors
		mesh.push_back(vertexNormals[normalIndexes[i] * 3]);
		mesh.push_back(vertexNormals[normalIndexes[i] * 3 + 1]);
		mesh.push_back(vertexNormals[normalIndexes[i] * 3 + 2]);

		// push vertex texture coordinates
		mesh.push_back(textureCoords[textureIndexes[i] * 2]);
		mesh.push_back(textureCoords[textureIndexes[i] * 2 + 1]);
	}


	status = true;
	return objectData;
}