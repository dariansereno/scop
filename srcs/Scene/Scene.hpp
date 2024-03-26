#pragma once
#include <vector>
#include <string>
#include "Face.hpp"
#include <map>


class Scene {
	private:
		std::string 			_name;
		std::vector<Vec3*>		_vertex;
		std::vector<Vec3*>		_textures;
		std::vector<Vec3*>		_normals;
		std::map<std::string, Material*>	_materials;
		std::vector<Face*>		_faces;
		bool					_shading;

		//TEMP
		std::vector<uint32_t>		_indices;

	public:
		void	setName(std::string name);
		void	addVertex(Vec3 *vertex);
		void	addTexture(Vec3 *texture);
		void	addNormal(Vec3 *texture);
		void	addMaterial(std::string name, Material *material);
		void	addFace(Face *face);
		void	shading(bool shading);
		void	addIndices(std::string indice);

		void				printVertex();
		std::vector<uint32_t> 	getIndices();
		std::vector<Vec3*>	getVertex();
		std::map<std::string, Material*> getMaterials();
};