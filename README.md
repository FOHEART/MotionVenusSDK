# MotionVenusSDK 1.3.0 For MotionVenus

<div align=center>
<img src="https://raw.githubusercontent.com/FOHEART/MotionVenusSDK/master/help/img/motionvenussdk.png"/>
</div>

<div align=center>
<img src="https://raw.githubusercontent.com/FOHEART/MotionVenusSDK/master/help/img/sdkrunning.png"/>
</div>

## 0.开发工具
SDK及Demo程序所有部分均提供**源码**。<br>
SDK部分使用c语言开发，Demo程序使用C++开发。<br>
工程使用**Visual Studio 2013**打包。

## 1. 功能描述
1. 通过UDP接收来自一个或多个MotionVenus客户端的实时数据流。<br>
2. 实时的23段人体骨骼的空间位置与旋转。<br>
3. SDK已封装为dll库，支持多线程处理。<br>
4. 配有DEMO程序。
## 2.数据类型单位制

<div align=center>
<table>
  <tr><th>数据类型</th><th>单位</th></tr>
  <tr><th>位置</th><th>meter（米）</th></tr>
  <tr><th>欧拉角</th><th>angle（度）</th></tr>
</table>
</div>

## 3.骨骼对应关系
<div align=center>
<img src="https://raw.githubusercontent.com/FOHEART/MotionVenusSDK/v1.3.0/help/img/ctls.png"/>
</div>


默认的编号对应的人体骨骼关系是：
<div align=center>
<table>
   <tr><th>索引</th><th>名称</th><th>备注</th></tr>
   <tr><th>0</th><th>Spine或Hips</th><th>类人模型的根节点，控制整个人体的位移和根关节点的旋转</th></tr>
	<tr><th>1</th><th>Spine1</th><th>相当于人体L3~L5段骨骼</th></tr>
	<tr><th>2</th><th>Spine2</th><th>相当于人体T12~L2段骨骼</th></tr>
	<tr><th>3</th><th>Spine3</th><th>相当于人体T8~T11段骨骼</th></tr>
	<tr><th>4</th><th>Spine4</th><th>相当于人体T1~T7段骨骼</th></tr>
	<tr><th>5</th><th>Neck</th><th></th></tr>
	<tr><th>6</th><th>Head</th><th></th></tr>
	<tr><th>7</th><th>RightShoulder</th><th></th></tr>
	<tr><th>8</th><th>RightArm</th><th></th></tr>
	<tr><th>9</th><th>RightForeArm</th><th></th></tr>
	<tr><th>10</th><th>RightHand</th><th></th></tr>
	<tr><th>11</th><th>LeftShoulder</th><th></th></tr>
	<tr><th>12</th><th>LeftArm</th><th></th></tr>
	<tr><th>13</th><th>LeftForeArm</th><th></th></tr>
	<tr><th>14</th><th>LeftHand</th><th></th></tr>
	<tr><th>15</th><th>RightUpLeg</th><th></th></tr>
	<tr><th>16</th><th>RightLeg</th><th></th></tr>
	<tr><th>17</th><th>RightFoot</th><th></th></tr>
	<tr><th>18</th><th>RightToeBase </th><th>右脚尖</th></tr>
	<tr><th>19</th><th>LeftUpLeg</th><th></th></tr>
	<tr><th>20</th><th>LeftLeg</th><th></th></tr>
	<tr><th>21</th><th>LeftFoot</th><th></th></tr>
	<tr><th>22</th><th>LeftToeBase</th><th>左脚尖</th></tr>
</table>
</div>

## 4. 使用方法
1. 解压或clone到本地硬盘的任意位置。
2. 设置MotionVenusSDK_Test工程为启动程序。
3. 编译运行。
## 5.使用场景

本SDK提供了基于人体动力学的23段骨骼模型，同时给出了每段骨骼的旋转与空间位置，可应用于例如：
1. 医学应用，例如肘、肩关节运动分析，脊柱弯曲度分析，下肢动力学分析等等方面。
2. 体育分析，例如投掷动作中手、小臂、大臂的连锁动作分析；落地过程中脚、大腿小腿的冲击力分析[注1]等等。
3. 道具映射，例如可将手部节点固定到枪械类道具上，该道具就有了空间360°旋转及位移信息；或将节点放置到需要测量旋转信息的任意物体上，就可以获取该物体实时的旋转信息。
4. 第三方软件集成，可利用本SDK与任意第三方软件集成，快速获得标准的人体动力学数据。

注1：此种分析需要同时用到加速度计以及陀螺仪底层数据，请联系我们以获得底层接口。