## [旋转](https://zhuanlan.zhihu.com/p/104677174)

这里讲两种旋转，`Fixed Angle`和[`Euler Angle`](https://zhuanlan.zhihu.com/p/626258420?utm_id=0)。前者绕以**世界坐标系基**为旋转轴旋转，后者绕以**局部坐标系基**为旋转轴旋转。

----
$R_{XYZ}(α,β,λ)$

这里$R$表示依次绕X、Y、Z轴旋转α，β，λ角度。那将此拆分开三个旋转矩阵，要如何书写呢？

### X-Y-Z Fixed Angle 
因为三次旋转$R_X(α)$、$R_Y(β)$、$R_Z(λ)$，对作用对象不会干扰其后续旋转的旋转方向，或者说某次旋转的方向，和之前之后是否旋转过，没差别，因此按顺序将旋转矩阵作用到对象上即可。

$R_{XYZ}(α,β,λ) = R_Z(λ) * R_Y(β) * R_X(α)$

<br>
<br>

### 绕局部坐标系基旋转
每一次旋转后，可能会改变局部坐标系基的方向。

那其实可以用`mapping`映射的的角度看这个旋转，因为每次都是绕自身局部坐标系基旋转，那旋转矩阵就是旋转前后两个坐标系的映射矩阵，由A坐标系旋转为B坐标系，旋转矩阵就是就是$^A_BR$，三次旋转就是三次映射 

*****
2024年6月17日

上述的理解不够透彻，详细可见[wiki的欧拉角](https://zh.wikipedia.org/wiki/%E6%AC%A7%E6%8B%89%E8%A7%92)

后面理解了再做更新

*****