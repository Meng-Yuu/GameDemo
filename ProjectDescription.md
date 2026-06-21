# RougeFPS 游戏 Demo 作品说明

## GitHub 仓库

项目地址：[https://github.com/Meng-Yuu/GameDemo.git](https://github.com/Meng-Yuu/GameDemo.git)

## 题目说明

本项目基于 Unreal Engine 5 官方 First Person 模板进行扩展，实现了一个第一人称射击游戏 Demo。Demo 包含玩家移动、视角控制、武器开火、敌人生命值、敌人行为、多人联机以及游戏胜利判定等基础玩法功能。

## 技术实现过程

### 1. 第一人称角色与输入

项目使用 UE5 Enhanced Input 系统处理玩家输入，在 `ARougeFPSCharacter` 中绑定移动、跳跃和视角控制等输入事件。角色使用第一人称相机作为视角核心，并将第一人称手臂 Mesh 挂载到相机下，使手臂和武器能够跟随玩家视角上下左右移动。

武器通过角色中的 `AddWeapon` 方法挂载到第一人称手臂 Mesh 的 `WeaponPoint` Socket 上。这样玩家视角变化时，武器会跟随手臂同步旋转，避免武器只能朝角色正前方开火的问题。

### 2. 武器开火与命中反馈

武器蓝图负责执行实际开火逻辑，包括从武器射击点生成子弹、播放开火特效与音效，并在命中敌人时触发伤害逻辑。敌人受到伤害后会调用 C++ 基类中的 `TakeDamage`，从而更新血量并通知蓝图播放命中特效、声音或更新血条 UI。

### 3. 敌人基类与派生敌人

项目创建了 C++ 敌人基类 `AEnemyBase`，用于统一管理敌人的通用属性和生命系统，包括：

- 最大生命值 `MaxHealth`
- 当前生命值 `CurrentHealth`
- 攻击伤害 `AttackDamage`
- 死亡状态 `bIsDead`
- 受伤事件 `BP_OnDamaged`
- 死亡事件 `BP_OnDeath`
- 攻击结束事件分发器 `ED_AttackEnd`

在此基础上创建了两种派生敌人 `AEnemy1` 和 `AEnemy2`，便于在蓝图中继续扩展不同敌人的模型、动画、AI 行为和数值配置。

### 4. 敌人 AI 行为

敌人使用 AIController、Behavior Tree、Blackboard 和感知组件实现基础 AI 流程。敌人默认会执行巡逻或随机移动逻辑，当感知到玩家后，会把目标写入黑板变量，并切换到追踪玩家的行为分支。

攻击逻辑通过动画蒙太奇和事件分发器配合实现：攻击动画结束时触发 `ED_AttackEnd`，AI 或蓝图逻辑可以监听该事件，从而恢复移动、继续追击或再次发起攻击。

### 5. 血条 UI 与受伤反馈

敌人血条通过 UMG Widget 实现，并由 Widget Component 挂载到敌人身上。血条 Widget 持有敌人引用，通过调用 `GetHealthPercent()` 获取当前生命百分比，并根据敌人受伤后的血量变化刷新显示。（敌人碰撞检测可能有问题）

### 6. 多人网络对战与胜利条件

项目加入了基础多人网络流程。胜利条件由服务器权威管理，避免客户端各自计算导致状态不一致。

核心实现包括：

- `ARougeFPSGameState`：同步总开火次数、开火上限和胜利状态
- `ARougeFPSGameMode`：在服务器端统计玩家总开火次数
- `ARougeFPSPlayerController`：通过 RPC 上报开火行为，并在胜利时显示 UI

当所有玩家累计成功开火达到 30 次后，服务器判定游戏胜利，并通知所有客户端显示胜利界面。胜利 UI 中间显示“游戏胜利”，下方提供退出按钮。（敌人碰撞检测可能有问题，所以临时把胜利条件改成了开火次数，检测联机质量）

## 项目特点

- 基于 UE5 First Person 模板进行功能扩展
- 使用 C++ 与蓝图结合开发
- 使用 Enhanced Input 实现玩家输入
- 使用 Socket 挂载实现第一人称武器表现
- 使用 C++ 敌人基类统一管理生命值、伤害和死亡逻辑
- 使用 Behavior Tree 与 Blackboard 实现基础敌人 AI
- 使用 UMG 实现敌人血条和游戏胜利界面
- 使用 GameMode、GameState 和 RPC 实现服务器权威的多人胜利判定

## 总结

本 Demo 实现了第一人称射击游戏的核心基础流程，包括玩家控制、武器开火、敌人受击、AI 追踪、血条显示、多人同步和胜利 UI。项目结构以 C++ 提供基础能力，蓝图负责表现层和玩法细节，便于后续继续扩展更多敌人类型、关卡目标、得分系统、动画表现和完整游戏流程。
