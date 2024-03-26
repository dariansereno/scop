#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "Vec3.hpp"
#include "Material.hpp"
#include "Face.hpp"
#include "Scene.hpp"

class Parser {
	private:
		using ParseFunc = void (Parser::*)(std::string, Scene&);
		using MaterialParseFunc = void (Parser::*)(std::string, Material*);
		
		std::ifstream	_file;
		std::string		_path;
		static const std::map<std::string, ParseFunc> _lineType;
		static const std::map<std::string, MaterialParseFunc> _materialLineType;

		/* ------------------------------- parseObject ------------------------------ */
		void 	_parseMaterial(std::string line, Scene &scene);
		void 	_parseComment(std::string line, Scene &scene);
		void	_parseVertex(std::string line, Scene &scene);
		void	_parseTexture(std::string line, Scene &scene);
		void	_parseNormals(std::string line, Scene &scene);
		void	_parseGroup(std::string line, Scene &scene);
		void	_parseName(std::string line, Scene &scene);
		void	_useMaterial(std::string line, Scene &scene);
		void	_parseShading(std::string line, Scene &scene);
		void	_parseFace(std::string line, Scene &scene);
		void	_parseLine(std::string line, Scene &scene);

		/* ------------------------------ parseMaterial ----------------------------- */
		void	_parseMaterialName(std:: string line, Material *material);
		void	_parseShiny(std:: string line, Material *material);
		void	_parseAmbiant(std:: string line, Material *material);
		void	_parseDiffuse(std:: string line, Material *material);
		void	_parseSpecular(std:: string line, Material *material);
		void	_parseRefraction(std:: string line, Material *material);
		void	_parseTransparency(std:: string line, Material *material);
		void	_parseIllum(std:: string line, Material *material);
		void	_parseComment(std::string line, Material *material);


		/* ---------------------------------- utils --------------------------------- */
		std::string _nextLine(void);
		std::string _getFirstWord(std::string line);
		std::vector<std::string> _tokenize(std::string str, const char* delim);
		Vec3	*_parseVec3(std::string line);


	public:
		Parser(std::string path);
		~Parser(void);

		void	parseObj(Scene &scene);
		void	parseMaterial(Scene &scene);

		class FileError : public std::exception{
			public:
				virtual const char 	*what() const throw()
				{
					return ("Error while opening the file");
				}
		};

};