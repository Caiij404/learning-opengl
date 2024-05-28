#ifndef UTILS_H
#define UTILS_H

// 顶点着色器
// extern const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in ver3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  
//     "}\0";


// 片段着色器
// out声明输入变量 Fragment
// extern const char *fragmentShaderSource = "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{\n"
//     "    FragColor = vec4(0.1f, 0.5f, 0.2f, 1.0f);\n"
//     "}\0";

// extern const char *fragmentShaderSource = "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{\n"
//     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//     "}\n\0";


enum logType{
    SHADER,
    SHADER_PROGRAM,
};

void compileInfoLog(unsigned int id, logType t);


#endif
