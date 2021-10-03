#pragma once
class Shader {
public:
    unsigned int vecShader;
    unsigned int fragShader;
    unsigned int ID;
    Shader(std::string vecFile, std::string fragFile) {
        std::ifstream vecIn;
        std::ifstream fragIn;
        std::stringstream vecStream;
        std::stringstream fragStream;

        vecIn.open(vecFile);
        vecStream << vecIn.rdbuf();
        vecIn.close();

        fragIn.open(fragFile);
        fragStream << fragIn.rdbuf();
        fragIn.close();

        std::string vecSourceTMP = vecStream.str();
        std::string fragSourceTMP = fragStream.str();

        GLchar const* vecSource = vecSourceTMP.c_str();
        GLchar const* fragSource = fragSourceTMP.c_str();

        GLint const vecLength = vecSourceTMP.size();
        GLint const fragLength = fragSourceTMP.size();

        vecShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vecShader, 1, &vecSource, &vecLength);
        glCompileShader(vecShader);
        checkCompileErrors(vecShader, "VERTEX");

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSource, &fragLength);
        glCompileShader(fragShader);
        checkCompileErrors(fragShader, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vecShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);
    }
    void use() {
        glUseProgram(ID);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type==: " << type << "\n" << infoLog << "\n --1 --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type=+: " << type << "\n" << infoLog << "\n --2 --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};