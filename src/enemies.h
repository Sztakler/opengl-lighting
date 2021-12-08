#include "includes.h"
#include "vao.h"
#include "vbo.h"
#include "shader.h"

class Enemies
{
    public:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> translations;

        VAO vertices_array;
        VAO normals_array;
        VAO translations_array;

        Material material;        

        VBO vertices_buffer;
        VBO normals_buffer;
        VBO translations_buffer;
        
        Shader shader;

        std::vector<glm::vec3> positions;
        std::vector<float> velocities;

    public:
        Enemies(const char* vertices_data_filename, const char* colors_data_filename,
            const char* vertex_shader_filename, const char* fragment_shader_filename, 
            int seed, int boardsize);
        Enemies(const char* obj_data_filename,
            const char* vertex_shader_filename, const char* fragment_shader_filename,
            int seed, glm::vec3 boardsize, Material material, int n_enemies);

    public:
        void Bind();
        void Unbind();
        void Draw();
        void DrawInstanced(GLsizei primcount, glm::mat4 *model, glm::mat4 *view, glm::mat4 *projection, DRAWING_MODE drawing_mode);
        void DrawInstanced(GLsizei primcount);
        void loadUniforms();
        
    private:
        void loadData(const char* filename, std::vector<float> &data, float scale);
        bool loadFromObjectFile(const char* filename);
        bool replace(std::string& str, const std::string& from, const std::string& to);
};
