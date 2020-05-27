#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

using namespace lesson_1n5;

bool CShader::Init(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // 1. Получаем исходный код шейдера из filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Удостоверимся, что ifstream объекты могут выкидывать исключения
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);

    try
    {
        // Открываем файлы
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Считываем данные в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Закрываем файлы
        vShaderFile.close();
        fShaderFile.close();
        // Преобразовываем потоки в массив GLchar
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return false;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    // 2. Сборка шейдеров
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Если есть ошибки - вывести их
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    };

    // Вершинный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Если есть ошибки - вывести их
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    };

    this->mProgramID = glCreateProgram();
    glAttachShader(this->mProgramID, vertex);
    glAttachShader(this->mProgramID, fragment);
    glLinkProgram(this->mProgramID);

    //Если есть ошибки - вывести их
    glGetProgramiv(this->mProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->mProgramID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // Удаляем шейдеры, поскольку они уже в программу и нам больше не нужны.
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    bInit = true;
    return true;
}

void CShader::Use()
{
    if(bInit)
     glUseProgram(this->mProgramID);
    else
     std::cout << "ERROR::SHADER::USE::PROGRAM_DONT_INILIZATED\n" << std::endl;
}

GLuint CShader::GetProgramID()
{
    return this->mProgramID;
}

void CShader::setFloat(const char* uniformString, GLfloat value)
{
    glUniform1f(glGetUniformLocation(this->mProgramID, uniformString), value);
}

void CShader::setInt(const char* uniformString, GLint value)
{
    glUniform1i(glGetUniformLocation(this->mProgramID, uniformString), value);
}

void CShader::setMatrix4fv(const char* uniformString, glm::mat4 value)
{
    GLuint Loc = glGetUniformLocation(this->mProgramID, uniformString);
    glUniformMatrix4fv(Loc, 1, GL_FALSE, glm::value_ptr(value));
}