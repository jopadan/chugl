#include "Material.h"

// base Material static defines ========================

// material options
const MaterialOptionParam Material::POLYGON_MODE = MaterialOptionParam::PolygonMode;
const MaterialOptionParam Material::LINE_WIDTH = MaterialOptionParam::LineWidth;
const MaterialOptionParam Material::POINT_SIZE = MaterialOptionParam::PointSize;


const MaterialPolygonMode Material::POLYGON_FILL = MaterialPolygonMode::Fill;
const MaterialPolygonMode Material::POLYGON_LINE = MaterialPolygonMode::Line;
const MaterialPolygonMode Material::POLYGON_POINT = MaterialPolygonMode::Point;

// normMat static defines ========================
const std::string NormalMaterial::USE_LOCAL_NORMALS_UNAME = "u_UseLocalNormal";

// phong static defines ========================
const std::string PhongMaterial::DIFFUSE_MAP_UNAME = "u_Material.diffuseMap";
const std::string PhongMaterial::SPECULAR_MAP_UNAME = "u_Material.specularMap";
const std::string PhongMaterial::DIFFUSE_COLOR_UNAME = "u_Material.diffuseColor";
const std::string PhongMaterial::SPECULAR_COLOR_UNAME = "u_Material.specularColor";
const std::string PhongMaterial::SHININESS_UNAME = "u_Material.shininess";