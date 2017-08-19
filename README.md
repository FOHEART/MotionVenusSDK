# MotionVenusSDK 1.3.0 For MotionVenus

<div align=center>
<img src="https://raw.githubusercontent.com/FOHEART/MotionVenusSDK/master/help/img/motionvenussdk.png"/>
</div>

<div align=center>
<img src="https://raw.githubusercontent.com/FOHEART/MotionVenusSDK/master/help/img/sdkrunning.png"/>
</div>

## 1. 功能描述
1. 通过UDP接收来自一个或多个MotionVenus客户端的实时数据流。<br>
2. 实时的23段人体骨骼的空间位置与旋转。<br>
3. SDK已封装为dll库，支持多线程处理。<br>
4. 配有DEMO程序。
## 2. 使用方法
1. 解压或clone到本地硬盘的任意位置。
2. 设置MotionVenusSDK_Test工程为启动程序。
3. 编译运行。
## 3.使用场景
本SDK提供了基于人体动力学的23段骨骼模型，同时给出了每段骨骼的旋转与空间位置，可应用于例如：
1. 医学应用，例如肘、肩关节运动分析，脊柱弯曲度分析，下肢动力学分析等等方面。
2. 体育分析，例如投掷动作中手、小臂、大臂的连锁动作分析；落地过程中脚、大腿小腿的冲击力分析[注1]等等。
3. 道具映射，例如可将手部节点固定到枪械类道具上，该道具就有了空间360°旋转及位移信息；或将节点放置到需要测量旋转信息的任意物体上，就可以获取该物体实时的旋转信息。
4. 第三方软件集成，可利用本SDK与任意第三方软件集成，快速获得标准的人体动力学数据。

注1：此种分析需要同时用到加速度计以及陀螺仪底层数据，请联系我们以获得底层接口。