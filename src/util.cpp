// source file for utility functions

#include "util.h"


//template <typename T>
//bool readFileToString(T filePath, std::string& writeback) {
//	// open filestream using standard library
//	std::ifstream f(filePath);
//	// check if file successfully opened
//	// therefore check if the file exists
//	if (!f.is_open()) {
//		std::cout << "Error: could not find file: " << filePath << "\n";
//		return false;
//	}
//	// use stringstream as a go between
//	std::stringstream buffer;
//	buffer << f.rdbuf();
//	// close file
//	f.close();
//	// convert to string and populate writeback variable
//	writeback = buffer.str();
//	
//	return true;
//}