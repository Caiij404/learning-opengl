#ifndef UTILS_H
#define UTILS_H

// 顶点着色器
static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// 片段着色器
// out声明输入变量 Fragment
static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


// 03_shader
// 顶点着色器
// c硬代码，换行符\n要放在注释后面，否则会报错
static const char* vs03 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0 \n"
    "out vec4 vertexColor03; // 为片段着色器指定一个颜色输出\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0); // 注意我们如何把一个vec3作为vec4的构造器的参数\n"
    "    vertexColor03 = vec4(0.5, 0.0, 0.0, 1.0); // 把输出变量设置为暗红色\n"
    "}\n\0";

// 片段着色器
static const char* fs03 = "#version 330 core \n"
    "out vec4 fragColor03;\n"
    "in vec4 vertexColor03;  // 从顶点着色器传来的输入变量(名称相同、类型相同) \n"
    "void main()\n"
    "{\n"
    "    fragColor03 = vertexColor03;\n"
    "}\n\0";

static const char* fs031 = "#version 330 core \n"
    "out vec4 fragColor03;\n"
    "uniform vec4 ourColor; // 声明ourColor变量 \n"
    "void main()\n"
    "{\n"
    "    fragColor03 = ourColor;\n"
    "}\n\0";

// 更多属性
// 顶点着色器 
static const char* vs032 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos; \n"
    "layout (location = 1) in vec3 aColor; \n"
    "out vec3 ourColor; \n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0); \n"
    "    ourColor = aColor; //将ourColor设置为我们从顶点数据那得到的输入颜色 \n"
    "}\n\0";

// 片段着色器
static const char* fs032 = "#version 330 core \n"
    "out vec4 fragColor03;\n"
    "in vec3 ourColor; // 声明ourColor变量 \n"
    "void main()\n"
    "{\n"
    "    fragColor03 = vec4(ourColor, 1.0);\n"
    "}\n\0";


enum logType{
    SHADER,
    SHADER_PROGRAM,
};

void compileInfoLog(unsigned int id, logType t);


#endif
