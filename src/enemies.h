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
        std::vector<float> velocities;
        std::vector<float> colors;  
        std::vector<glm::vec3> colors_vec;  

        float yOffset;
        float velocity;

        VAO vertices_array;
        VAO normals_array;
        VAO translations_array;
        VAO colors_array;
        // VAO velocities_array;

        Material material;    

        VBO vertices_buffer;
        VBO normals_buffer;
        VBO translations_buffer;
        VBO colors_buffer;
        // VBO velocities_buffer;

        Shader shader;

        std::vector<glm::vec3> positions;

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
        void DrawInstanced(GLsizei primcount, glm::mat4 *model, glm::mat4 *view, glm::mat4 *projection, DRAWING_MODE drawing_mode, bool transparent, glm::vec3 camera_position);
        void DrawInstanced(GLsizei primcount);
        void loadUniforms();
        
    private:
        void loadData(const char* filename, std::vector<float> &data, float scale);
        bool loadFromObjectFile(const char* filename);
        bool replace(std::string& str, const std::string& from, const std::string& to);
        void sortPositions(glm::vec3 player_position);
};
