#pragma once
class Object
{
public:
	Object();
	~Object();

public:
    float x_trans{}, y_trans{}, z_trans{};
    float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};

    float x_rotate{}, y_rotate{}, z_rotate{};
    float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};

    float x_scale{}, y_scale{}, z_scale{};
    float x_scale_aoc{}, y_scale_aoc{}, z_scale_aoc{};

    float color_r{}, color_g{}, color_b{};

    float r{};

public:
    GLuint VAO[3], VBO[6];
    GLuint s_program;
    GLUquadricObj* qobj;

    GLuint shaderID;
    GLuint vertexShader;
    GLuint fragmentShader;

    GLenum obj_type{ GL_FILL };
    GLint g_window_w, g_window_h;

public:
    GLchar* filetobuf(const GLchar* file);
    GLvoid InitBuffer();

    void make_vertexShaders();
    void make_fragmentShader();
    void InitShader();


};

class Bullet : public Object
{
public:
    Bullet();
    ~Bullet();
};

class Building : public Object
{
public:
    Building();
    ~Building();
};

class Ground : public Object
{
public:
    Ground();
    ~Ground();
};