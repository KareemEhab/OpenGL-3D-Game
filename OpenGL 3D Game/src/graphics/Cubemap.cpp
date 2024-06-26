#include "Cubemap.h"

Cubemap::Cubemap()
    : hasTextures(false) {}

void Cubemap::loadTextures(string _dir,
   string right,
   string left,
   string top,
   string bottom,
   string front,
   string back) 
{
    dir = _dir;
    hasTextures = true;
    faces = { right, left, top, bottom, front, back };

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nChannels;

    for (unsigned int i = 0; i < 6; i++) 
    {
        unsigned char* data = stbi_load((dir + "/" + faces[i]).c_str(),
            &width, &height, &nChannels, 0);

        GLenum colorMode = GL_RED;
        switch (nChannels) {
        case 3:
            colorMode = GL_RGB;
            break;
        case 4:
            colorMode = GL_RGBA;
            break;
        }

        if (data) 
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
        }
        else
            cout << "Failed to load texture at " << faces[i] << endl;

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::init() 
{
    // Set up vertices
    float skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // generate/setup VAO
    VAO.generate();
    VAO.bind();

    // setup VBO
    VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
    VAO["VBO"].generate();
    VAO["VBO"].bind();
    VAO["VBO"].setData<float>(36 * 3, skyboxVertices, GL_STATIC_DRAW);

    // set attribute pointers
    VAO["VBO"].setAttribPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);

    VAO["VBO"].clear();

    ArrayObject::clear();
}

void Cubemap::render(Shader shader, Scene* scene) 
{
    glDepthMask(GL_FALSE); // Disable depth test to view objects behind the skybox to simulate that the skybox is actually big

    shader.activate();

    // Remove translation from view matrix
    glm::mat4 view = glm::mat4(glm::mat3(scene->getActiveCamera()->getViewMatrix()));
    shader.setMat4("view", view);
    shader.setMat4("projection", scene->projection);

    if (hasTextures)
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    VAO.bind();
    VAO.draw(GL_TRIANGLES, 0, 36);
    ArrayObject::clear();

    glDepthMask(GL_TRUE);
}

void Cubemap::cleanup() 
{
    VAO.cleanup();
}