#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Vec3.hpp"
#include <glm/glm.hpp>

struct MaterialProperties {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float refraction;
    int illumination;
    float shininess;
    float transparency;
};

class Material {
    private:
		std::ifstream	_file;
        std::string	_name;
		std::string	_textureMap;
        Vec3		*_ambiantColor;
		Vec3		*_diffuseColor;
		Vec3		*_specularColor;
		double		_refraction;
		int			_illumination;
		double		_shininess;
		double		_transparency;

    public:
		Material(std::string path);
		~Material(void);

		/* ------------------------------ getter/setter ----------------------------- */
		void	setName(std::string name);
		void	setTextureMap(std::string textureMap);
		void	setAmbiant(Vec3 *ambiant);
		void	setDiffuse(Vec3 *diffuse);
		void	setSpecular(Vec3 *specular);
		void	setRefraction(double refraction);
		void	setIllumination(int illumination);
		void	setShininess(double shininess);
		void 	setTransparency(double transparency);
		std::string	getName(void);
		
		bool		isFileEmpty(void);
		std::string	nextLine(void);
		void		setPath(std::string path);
		void		print();

		MaterialProperties getProperties();
};