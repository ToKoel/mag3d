#include "shader.h"

void Shader::use() const { glUseProgram(programID); }
void Shader::setBool(const std::string &name, const bool value) const {
  glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, const int value) const {
  glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, const float value) const {
  glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, const float x, const float y) const {
  glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, const float x, const float y, const float z) const {
  glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, const float x, const float y,
                     const float z, const float w) const {
  glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
  glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}
