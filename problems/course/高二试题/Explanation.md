# 第 19 题（17 分）解析：仿射坐标系

## 核心模型

`xOy(α)` 仿射坐标系中，坐标 $(x,y)$ 表示向量

$\vec{OA}=x\,e_1+y\,e_2,\qquad |e_1|=|e_2|=1,\qquad e_1\cdot e_2=\cos\alpha .$$

它是一般斜角坐标系，因此两点积公式为

$(x_1,y_1)\cdot(x_2,y_2)=x_1x_2+y_1y_2+(x_1y_2+x_2y_1)\cos\alpha ,$$

模长

$|(x,y)|^2=x^2+y^2+2xy\cos\alpha .$$

当 $\alpha=\frac{\pi}{2}$ 时即退化为我们熟悉的直角坐标系，故 $\cos\alpha$ 项是“斜”带来的修正。

---

## (1) 求 $|\vec a|$

$xOy(\tfrac{\pi}{4})$ 中 $\cos\alpha=\cos\frac{\pi}{4}=\frac{\sqrt2}{2}$，$\vec a=(2,1)$。

$
|\vec a|^2=2^2+1^2+2\cdot2\cdot1\cdot\frac{\sqrt2}{2}
=5+2\sqrt2 .
$


$\boxed{|\vec a|=\sqrt{5+2\sqrt2}}$$

---

## (2) 求 $\cos\alpha$

$\vec a=(3,-1),\;\vec b=(1,-3)$，夹角为 $\frac{\pi}{3}$。

$|\vec a|^2=3^2+(-1)^2+2\cdot3\cdot(-1)\cos\alpha=10-6\cos\alpha ,$$

$|\vec b|^2=1^2+(-3)^2+2\cdot1\cdot(-3)\cos\alpha=10-6\cos\alpha .$$

故 $|\vec a|=|\vec b|$。内积

$\vec a\cdot\vec b=3\cdot1+(-1)(-3)+\big(3\cdot(-3)+(-1)\cdot1\big)\cos\alpha
=6-10\cos\alpha .
$

$\cos\frac{\pi}{3}=\frac{\vec a\cdot\vec b}{|\vec a||\vec b|}
=\frac{6-10\cos\alpha}{10-6\cos\alpha}=\frac12 .
$

$\Rightarrow 2(6-10\cos\alpha)=10-6\cos\alpha
\Rightarrow 2=14\cos\alpha
\Rightarrow \boxed{\cos\alpha=\frac17}
$

---

## (3) 求 $\vec{OE}\cdot\vec{OF}$ 的取值范围

$xOy(\tfrac{\pi}{3})$ 中 $\cos\alpha=\cos\frac{\pi}{3}=\frac12$。

设 $B=(b,0),\;C=(0,c)$，$b,c>0$。则

$\vec{BC}=\vec{OC}-\vec{OB}=-b\,e_1+c\,e_2 ,$$

$|\vec{BC}|^2=b^2+c^2-2bc\cos\alpha=b^2+c^2-bc=1 .$$

约束就是 $b^2+c^2-bc=1$。

由 $\vec{OD}=-3\vec{OC}$ 得 $D=(0,-3c)$。$E,F$ 分别为 $BD,BC$ 中点：

$E=\left(\frac b2,\,-\frac{3c}{2}\right),\qquad
F=\left(\frac b2,\,\frac c2\right).
$

于是

$\vec{OE}\cdot\vec{OF}
=\left(\frac b2\right)\!\left(\frac b2\right)
+\left(-\frac{3c}{2}\right)\!\left(\frac c2\right)
+\left[\left(\frac b2\right)\!\left(\frac c2\right)
+\left(-\frac{3c}{2}\right)\!\left(\frac b2\right)\right]\frac12
$

$=\frac{b^2}{4}-\frac{3c^2}{4}-\frac{bc}{4}
=\frac{b^2-3c^2-bc}{4}.
$

代约束 $b^2-bc=1-c^2$：

$\vec{OE}\cdot\vec{OF}=\frac{(1-c^2)-3c^2}{4}
=\frac{1-4c^2}{4}=\frac14-c^2 .$

下面确定 $c$ 的范围。把约束看成关于 $b$ 的二次方程

$b^2-bc+(c^2-1)=0 .$$

有实根需判别式非负：

$\Delta=c^2-4(c^2-1)=4-3c^2\ge0
\Rightarrow c^2\le\frac43
\Rightarrow 0<c\le\frac{2}{\sqrt3} .$

（端点 $c=\frac{2}{\sqrt3}$ 时 $b=\frac c2=\frac1{\sqrt3}>0$，合法；$c>0$ 是严格开区间。）

故

$\frac14-c^2\in\left[\frac14-\frac43,\;\frac14\right)
=\left[-\frac{13}{12},\;\frac14\right).$

$\boxed{\vec{OE}\cdot\vec{OF}\in\left[-\frac{13}{12},\,\frac14\right)}$$

---

## 小结

- 仿射坐标系点积比直角坐标多出一项 $(x_1y_2+x_2y_1)\cos\alpha$，这是“斜轴”带来的本质修正。
- (1)(2) 直接套用模长/点积公式即可，(2) 中 $|\vec a|=|\vec b|$ 是关键化简。
- (3) 是“约束 + 范围”问题：先用 $|\vec{BC}|=1$ 建立 $b,c$ 约束，再消元把目标写成一个仅含 $c$ 的式子，最后用判别式确定 $c$ 的取值范围。注意端点开闭：$c>0$ 使上界开，$c=\frac{2}{\sqrt3}$ 可达使下界闭。

## 数值验证

- (1) $\sqrt{5+2\sqrt2}\approx2.798$。
- (2) $\cos\alpha=\frac17\Rightarrow$ 两向量夹角为 $\frac{\pi}{3}$。
- (3) 在约束上遍历 $c\in(0,\frac{2}{\sqrt3}]$ 得 $\vec{OE}\cdot\vec{OF}\in[-1.0833,\,0.25)$，即 $\left[-\frac{13}{12},\frac14\right)$。
