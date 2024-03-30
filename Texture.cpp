#include "Texture.h"
#include "textures/stb_image.h"

Texture::Texture(const std::string& path, GLuint programID): 
	m_FilePath(path), m_localBuffer(nullptr), m_width(0), m_height(0), m_BPP(0), ProgramID(programID)
{
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);
	glGenTextures(1, &ProgramID);
	glBindTexture(GL_TEXTURE_2D, ProgramID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_localBuffer)
	{
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &ProgramID);
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ProgramID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


CubeTexture::CubeTexture(std::vector<std::string> faces):
    m_localBuffer(nullptr), m_faces(faces), m_width(0), m_height(0), m_nrChannels(0)
{
    stbi_set_flip_vertically_on_load(false);
    glGenTextures(1, &ProgramID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ProgramID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        m_localBuffer = stbi_load(faces[i].c_str(), &m_width, &m_height, &m_nrChannels, 0);
        if (m_localBuffer)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_localBuffer
            );
            stbi_image_free(m_localBuffer);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(m_localBuffer);
        }
    }
   
}

void CubeTexture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ProgramID);
}

void CubeTexture::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}