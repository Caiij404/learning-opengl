## 屏幕空间环境光遮蔽 Screen-Space Ambient Occlusion  SSAO

在前面的教程中，环境光（Ambient Lighting）是作为固定光照常量加入到场景总体光照计算中，被用来模拟光的**散射（Scattering）**，但现实中光线会以任意方向散射，强度也一直改变，被间接照射到的那部分场景也应该有变化的强度，而不是一成不变的环境光。

有一种间接光照的模拟叫**环境光遮蔽（Ambient Occlusion）**，原理是通过褶皱、孔洞和非常靠近的墙面变暗的方法近似模拟出间接光照，这些区域很大程度是被周围的几何体遮蔽，光线难以流失，因此看起来更暗一些。

-----------------

看不懂了。。。。。

后面再看吧