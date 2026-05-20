#include "GameObject.h"


GameObject::GameObject() {
    transform = glm::mat4 (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);

    localTransform = glm::mat4 (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    localPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    localRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    localScale = glm::vec3(1.0f, 1.0f, 1.0f);
    isDirty = false;
}

glm::vec3 GameObject::getPosition() const { return position; }
void GameObject::setPosition(const glm::vec3& p) {
    position = p;
    isDirty = true;
}

glm::vec3 GameObject::getRotation() const { return rotation; }
void GameObject::setRotation(const glm::vec3& r) {
    rotation = r;
    isDirty = true;
}

glm::vec3 GameObject::getScale() const { return scale; }
void GameObject::setScale(const glm::vec3& s) {
    scale = s;
    isDirty = true;
}

glm::vec3 GameObject::getLocalPosition() const { return localPosition; }
void GameObject::setLocalPosition(const glm::vec3& p) {
    localPosition = p;
    isDirty = true;
}

glm::vec3 GameObject::getLocalRotation() const { return localRotation; }
void GameObject::setLocalRotation(const glm::vec3& r) {
    localRotation = r;
    isDirty = true;
}

glm::vec3 GameObject::getLocalScale() const { return localScale; }
void GameObject::setLocalScale(const glm::vec3& s) {
    localScale = s;
    isDirty = true;
}
