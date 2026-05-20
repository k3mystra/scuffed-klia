#pragma once

#include <glm/glm.hpp>

class GameObject {
public:
  GameObject();
  void recalcTransform();

  glm::vec3 getPosition() const;
  void setPosition(const glm::vec3 &newPosition);

  glm::vec3 getRotation() const;
  void setRotation(const glm::vec3 &newRotation);

  glm::vec3 getScale() const;
  void setScale(const glm::vec3 &newScale);

  glm::vec3 getLocalPosition() const;
  void setLocalPosition(const glm::vec3 &newLocalPosition);

  glm::vec3 getLocalRotation() const;
  void setLocalRotation(const glm::vec3 &newLocalRotation);

  glm::vec3 getLocalScale() const;
  void setLocalScale(const glm::vec3 &newLocalScale);

private:
  bool isDirty;

  glm::mat4 transform;
  glm::mat4 invTransform;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

  glm::mat4 localTransform;
  glm::mat4 localInvTransform;
  glm::vec3 localPosition;
  glm::vec3 localRotation;
  glm::vec3 localScale;
};
