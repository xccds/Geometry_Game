# Geometry_Game

一个用C++开发的2D射击小游戏

安装：
- 下载game.zip，解压到本地
- 先安装微软runtime环境
- 可以去官网(https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170) 下载安装x86版本
- 解压包中也附带了VC_redist.x86.exe文件，运行安装任一即可。
- 双击game.exe 开始游戏。

攻略：
- WASD四个方向键用于移动
- 鼠标左键射击
- P键暂停
- ESC退出
- 出生点在中间安全屋，安全屋只有玩家可以进入，敌方无法进入。但安全屋内和周围范围无法射击
- 玩家接触到敌方，生命值减一，回到安全屋。生命值为0，游戏重置，分值归0
