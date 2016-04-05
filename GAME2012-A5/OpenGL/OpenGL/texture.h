#pragma once

#include <string>

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& Filename);
	bool Load();
	void Bind(GLenum TextureUnit);

private:
	std::string m_filename;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	Magick::Image m_image;
	Magick::Blob m_blob;
};