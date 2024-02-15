#include "Parser.hpp"
#include <cstring> 
#include <iostream>

const std::map<std::string, Parser::ParseFunc> Parser::_lineType = {
	{"vt", &Parser::_parseTexture},
	{"vn", &Parser::_parseNormals},
	{"mtllib", &Parser::_parseMaterial},
	{"usemtl", &Parser::_useMaterial},
	{"o", &Parser::_parseName},
	{"g", &Parser::_parseGroup},
	{"#", &Parser::_parseComment},
	{"v", &Parser::_parseVertex},
	{"f", &Parser::_parseFace},
	{"s", &Parser::_parseShading},
};


const std::map<std::string, Parser::MaterialParseFunc> Parser::_materialLineType = {
	{"newmtl", &Parser::_parseMaterialName},
	{"ns", &Parser::_parseShiny},
	{"#", &Parser::_parseComment},
	{"Ka", &Parser::_parseAmbiant},
	{"Kd", &Parser::_parseDiffuse},
	{"Ks", &Parser::_parseSpecular},
	{"Ni", &Parser::_parseRefraction},
	{"d", &Parser::_parseTransparency},
	{"illum", &Parser::_parseIllum},
};

Parser::Parser(std::string path) {
	_file.open(path);
	if (_file.good()) {
		_path = path.substr(0, path.find_last_of("/") + 1);
		return ;
	}
	throw new FileError();
}

Parser::~Parser(void) {
	_file.close();
}

/* ---------------------------------- utils --------------------------------- */
std::string Parser::_nextLine(void) {
	std::string str;

	std::getline(_file, str);
	return (str);
}

std::string	Parser::_getFirstWord(std::string line) {
	int lastSpaceIndex = line.find(" ");
	if (lastSpaceIndex == -1)
		throw new FileError();
	
	while (line[lastSpaceIndex] == ' ')
		lastSpaceIndex++;
	return line.substr(lastSpaceIndex, line.size());
}

std::vector<std::string> Parser::_tokenize(std::string str, const char* delim)
{ 
	std::vector<std::string>	out;

    char *token = strtok(const_cast<char*>(str.c_str()), delim); 
    while (token != nullptr) 
    { 
        out.push_back(std::string(token)); 
        token = strtok(nullptr, delim); 
    }
	return (out);
} 


Vec3	*Parser::_parseVec3(std::string line) {
	std::vector<std::string>	token;

	token = _tokenize(line, " ");
	if (token.size() == 1)
		throw new FileError();
	else if (token.size() == 3)
		return new Vec3(std::stod(token[1]),std::stod(token[2]));
	else if (token.size() == 4)
		return new Vec3(std::stod(token[1]),std::stod(token[2]), std::stod(token[3]));
	else if (token.size() == 5)
		return new Vec3(std::stod(token[1]),std::stod(token[2]), std::stod(token[4]));
	else 
		return nullptr;
}

/* ------------------------------ ObjectParsing ----------------------------- */
void	Parser::parseObj(Scene &scene) {
	std::string	line = _nextLine();

	while (!_file.eof()) {
		_parseLine(line, scene);
		line = _nextLine();
	}
}

void	Parser::_parseLine(std::string line, Scene &scene) {
	std::vector<std::string>	token;

	token = _tokenize(line, " ");
	for (auto it = _lineType.begin(); it != _lineType.end(); it++) {
		if (line.starts_with(it->first) && token[0].length() == it->first.length()) {
		std::cout << it->first << std::endl; 
			(this->*(_lineType.find(it->first)->second))(line, scene);
			break;
		}
	}
}

void	Parser::_parseName(std::string line, Scene &scene) {
	scene.setName(_getFirstWord(line));
}

void	Parser::_parseVertex(std::string line, Scene &scene) {
	scene.addVertex(_parseVec3(line));
}

void	Parser::_parseTexture(std::string line, Scene &scene) {
	scene.addTexture(_parseVec3(line));
}

void	Parser::_parseNormals(std::string line, Scene &scene) {
		scene.addNormal(_parseVec3(line));
}

void	Parser::_parseGroup(std::string line, Scene &scene) {
}

void	Parser::_useMaterial(std::string line, Scene &scene) {
}

void	Parser::_parseShading(std::string line, Scene &scene) {
}

void	Parser::_parseFace(std::string line, Scene &scene) {
	std::vector<std::string>	tokens = _tokenize(line, " ");
	int n = tokens.size() - 1;

	// iterate 1 time, and more if more than 3 vertices vertices.
	for (int j = 0; j < n - 2; j++) {
		std::vector<std::string> cuttedParts = _tokenize(tokens[1], "/");
		scene.addIndices(cuttedParts[0]);
		//add first vertice

		for (int i = 1; i < 3 && i <= n; i++) {
			// add the next 2 vertices
			cuttedParts = _tokenize(tokens[i + j + 1], "/");
			scene.addIndices(cuttedParts[0]);
		}
	}
}

void	Parser::_parseComment(std::string line, Scene &scene) {
}


/* ----------------------------- materialParsing ---------------------------- */
void	Parser::_parseMaterial(std::string line, Scene &scene) {
	std::string	path = _getFirstWord(line); //maybe bug if substr return nothing

	Material *material = new Material(_path + path);
	std::string lineMaterial = material->nextLine();
	while (material->isFileEmpty()) {
		for (auto it = _materialLineType.begin(); it != _materialLineType.end(); it++) {
			if (lineMaterial.starts_with(it->first)) {
				(this->*(_materialLineType.find(it->first)->second))(lineMaterial, material);
			}
		}
		lineMaterial = material->nextLine();
	}
	scene.addMaterial(material->getName(), material);
}

void	Parser::_parseMaterialName(std::string line, Material *material) {
	material->setName(_getFirstWord(line));
}

void	Parser::_parseShiny(std:: string line, Material *material) {
	std::string token = _getFirstWord(line);
	material->setShininess(std::stod(token));
}

void	Parser::_parseAmbiant(std:: string line, Material *material) {
	material->setAmbiant(_parseVec3(line));
}

void	Parser::_parseDiffuse(std:: string line, Material *material) {
	material->setDiffuse(_parseVec3(line));

}

void	Parser::_parseSpecular(std:: string line, Material *material) {
	material->setSpecular(_parseVec3(line));
}

void	Parser::_parseRefraction(std:: string line, Material *material) {
	std::string token = _getFirstWord(line);
	material->setRefraction(std::stod(token));
}

void	Parser::_parseTransparency(std:: string line, Material *material) {
	std::string token = _getFirstWord(line);
	material->setTransparency(std::stod(token));
}

void	Parser::_parseIllum(std:: string line, Material *material) {
	std::string token = _getFirstWord(line);
	material->setIllumination(std::stod(token));
}

void	Parser::_parseComment(std::string line, Material *material) {
}


