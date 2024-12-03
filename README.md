## 几个有趣的现象
1. depth值的大小会影响渲染后块面亮度的过渡速度。depth越大，块面在z轴上的跨越就越大，就会更倾斜，那么光如果是z轴朝向的，那么得到的intensity就会倾向于更小。

## 一些问题
当换成Tutor的矩阵库代码后，图片发生了意料之外的切边，看起来摄像机变高了。最后发现是因为新的矩阵库中的proj方法（截取向量），在call的时候没有将x,y,z都除以w值，导致了变换的扭曲。

obj文件的顶点定义顺序是逆时针的。

如果使用Phong shading, 使用的是方法是法线插值，因此在对顶点应用一系列矩阵变换时，也需要对法线进行矩阵变换的逆转置
