#include "includes.h"
#include "vao.h"
#include "vbo.h"
#include "shader.h"

class Enemies
{
    public:
        std::vector<float> vertices;
        std::vector<float> colors;
        std::vector<float> translations;
        std::vector<float> rotations;

        // std::vector<glm::vec3> enemies_position;
        std::vector<AABB> enemies_boxes;

        VAO vertices_array;
        VAO translations_array;
        VAO rotations_array;
        
        VBO vertices_buffer;
        VBO translations_buffer;
        VBO rotations_buffer;
        
        Shader shader;

    public:
        Enemies(const char* vertices_data_filename, const char* colors_data_filename,
            const char* vertex_shader_filename, const char* fragment_shader_filename, int seed, int boardsize);

    public:
        void Bind();
        void Unbind();
        void Draw();
        void DrawInstanced(GLsizei primcount, glm::mat4 *model, glm::mat4 *view, glm::mat4 *projection, DRAWING_MODE drawing_mode);
        void DrawInstanced(GLsizei primcount);

    private:
        void loadData(const char* filename, std::vector<float> &data, float scale);
};
