## 记录问题

1. 33_frame_buffers章节中，后视镜效果没实现


2. 34_cube_map，看readme.md


3. 还有一个问题没想清楚，天空盒的view矩阵，为什么把位移去掉就能有这样的效果？


4. `44_shadow_mapping`章节中，p182的未解之谜...
真tm的玄乎，都不用两行代码，只用shader.setBool()，参数任意，不管着色器是否有对应的`uniform`变量，都会出现相机往正z方向跑的问题。