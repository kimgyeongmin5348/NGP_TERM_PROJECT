#pragma once

class Object
{
public:
	Object();
	~Object();

protected:
    /*
    float x_trans{}, y_trans{}, z_trans{};
    float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};

    float x_rotate{}, y_rotate{}, z_rotate{};
    float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};

    float x_scale{}, y_scale{}, z_scale{};
    float x_scale_aoc{}, y_scale_aoc{}, z_scale_aoc{};

    float color_r{}, color_g{}, color_b{};

    float r{};
    */

    GLuint VAO, VBO;

    glm::vec3 position;
    glm::vec3 position_aoc;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;

    Object* parent = nullptr;
    std::vector<Object*> children;

public:
    GLUquadricObj* qobj;
    GLenum obj_type{ GL_FILL };

public:
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetRotation(const glm::vec3& rot) { rotation = rot; }
    void SetScale(const glm::vec3& scl) { scale = scl; }
    void SetColor(const glm::vec3& col) { color = col; }

    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetRotation() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    glm::vec3 GetColor() const { return color; }

    void SetParent(Object* parent);
    void AddChild(Object* child);

    virtual void Render(GLuint program, const glm::mat4& parentTransform = glm::mat4(1.0f));
    virtual void Update(float deltaTime);
};

class Bullet : public Object
{
public:
    Bullet();
    ~Bullet();

    virtual void Update(float deltaTime) override;

    bool active = false;
};

class Building : public Object
{
public:
    Building();
    ~Building();

    void Setting();
};

class Ground : public Object
{
public:
    Ground();
    ~Ground();
};