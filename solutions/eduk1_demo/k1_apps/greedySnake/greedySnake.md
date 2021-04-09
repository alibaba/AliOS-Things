@page greedySnake 贪吃蛇
#  实验介绍
**贪吃蛇**是一个起源于1976年的街机游戏 Blockade。此类游戏在1990年代由于一些具有小型屏幕的移动电话的引入而再度流行起来，在现在的手机上基本都可安装此小游戏。版本亦有所不同。
在游戏中，玩家操控一条细长的蛇，它会不停前进，玩家只能操控蛇的头部朝向（上下左右），一路拾起触碰到食物，并要避免触碰到自身或者其他障碍物。每次贪吃蛇吃掉一件食物，它的身体便增长一些。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01I9rGrL1cdq0ojzFbz_!!6000000003624-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

# 涉及知识点
OLED绘图
按键事件

# 开发环境准备
## 硬件
    开发用电脑一台
    HAAS EDU K1 开发板一块
    USB2TypeC 数据线一根

## 软件
### AliOS Things开发环境搭建
    开发环境的搭建请参考 @ref HaaS_EDU_K1_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### HaaS EDU K1 DEMO 代码下载
    HaaS EDU K1 DEMO 的代码下载请参考 @ref HaaS_EDU_K1_Quick_Start (创建工程章节)，其中，
    选择解决方案: 基于教育开发板的示例
    选择开发板: haaseduk1 board configure

### 代码编译、烧录
    参考 @ref HaaS_EDU_K1_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。
    参考 @ref HaaS_EDU_K1_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。

# 设计思路
## 游戏空间映射到逻辑空间
当玩家在体验游戏时，他们能操作的都是游戏空间，包括按键的上下左右，对象物体的运动等等。对于开发者而言，我们需要将这些设想的游戏空间映射到逻辑空间中，做好对用户输入的判断，对象运动的处理，对象间交互的判定，游戏整体进程的把控，以及最终将逻辑空间再次映射回游戏空间，返回给玩家。
## 对象定义
这一步是将游戏空间中涉及到的对象抽象化。在C语言的实现中，我们将对象抽象为结构体，对象属性抽象为结构体的成员。
### 蛇
```c
typedef struct
{
    uint8_t length;		// 当前长度
    int16_t *XPos;		// 逻辑坐标x 数组
    int16_t *YPos;		// 逻辑坐标y 数组
    uint8_t cur_dir;	// 蛇头的运行方向
    uint8_t alive;		// 存活状态
} Snake;
```
### 食物
```c
typedef struct
{
    int16_t x;
    int16_t y;			// 食物逻辑坐标
    uint8_t eaten;		// 食物是否被吃掉 
} Food;
```
### 地图
```c
typedef struct
{
    int16_t border_top;
    int16_t border_right;
    int16_t border_botton;
    int16_t border_left;	// 边界像素坐标
    int16_t block_size;		// 网格大小 在本实验的实现中 蛇身和食物的大小被统一约束进网格的大小中
} Map;
```
### 游戏
```c
typedef struct
{
    int16_t score;			// 游戏记分
    int16_t pos_x_max;		// 逻辑最大x坐标	pos_x_max = (map.border_right - map.border_left) / map.block_size;
    int16_t pos_y_max;		// 逻辑最大y坐标	pos_y_max = (map.border_botton - map.border_top) / map.block_size;
} snake_game_t;
```
通过Map和snake_game_t的定义，我们将屏幕的 (border_left, border_top, border_bottom, border_right) 部分设定为游戏区域，并且将其切分为 pos_x_max* pos_y_max 个大小为 block_size 的块。继而，我们可以在每个块中绘制蛇、食物等对象。
## 对象初始化
在游戏每一次开始时，我们需要给对象一些初始的属性，例如蛇的长度、位置、存活状态，食物的位置、状态， 地图的边界、块大小等等。
```c
Food food = {-1, -1, 1};
Snake snake = {4, NULL, NULL, 0, 1};
Map map = {2, 128, 62, 12, 4};
snake_game_t snake_game = {0, 0, 0};

int greedySnake_init(void)
{
    // 计算出游戏的最大逻辑坐标 用于约束游戏范围
    snake_game.pos_x_max = (map.border_right - map.border_left) / map.block_size;
    snake_game.pos_y_max = (map.border_botton - map.border_top) / map.block_size;
    // 为蛇的坐标数组分配空间 蛇的最大长度是填满整个屏幕 即 pos_x_max* pos_y_max
    snake.XPos = (int16_t *)malloc(snake_game.pos_x_max * snake_game.pos_y_max * sizeof(int16_t));
    snake.YPos = (int16_t *)malloc(snake_game.pos_x_max * snake_game.pos_y_max * sizeof(int16_t));
    // 蛇的初始长度设为4
    snake.length = 4;
    // 蛇的初始方向设为 右
    snake.cur_dir = SNAKE_RIGHT;
    // 生成蛇的身体 蛇头在逻辑区域最中间的坐标上 即 (pos_x_max/2, pos_y_max/2)
    for (uint8_t i = 0; i < snake.length; i++)
    {
        snake.XPos[i] = snake_game.pos_x_max / 2 + i;
        snake.YPos[i] = snake_game.pos_y_max / 2;
    }
    // 复活这条蛇
    snake.alive = 1;
	
	// 将食物设置为被吃掉
    food.eaten = 1;
    // 生成食物 因为食物需要反复生成 所以封装为函数
    gen_food();

    // 游戏开始分数为0
    snake_game.score = 0;
    
    return 0;
}
```
```c
void gen_food()
{
    int i = 0;
    // 如果食物被吃了
    if (food.eaten == 1)
    {
        while (1)
        {
            // 随机生成一个坐标
            food.x = rand() % snake_game.pos_x_max;
            food.y = rand() % snake_game.pos_y_max;

            // 开始遍历蛇身 检查坐标是否重合
            for (i = 0; i < snake.length; i++)
            {
                // 如果生成的食物坐标和蛇身重合 不合法 重新随机生成
                if ((food.x == snake.XPos[i]) && (food.y == snake.YPos[i]))
                    break;
            }
            // 遍历完蛇身 并未发生重合
            if (i == snake.length)
            {
                // 生成有效 终止循环
                food.eaten = 0;
                break;
            }
        }
    }
}
```
## 对象绘画
这一步其实是将逻辑空间重新映射到游戏空间，理应是整个游戏逻辑的最后一步，但是在我们开发过程中，也需要来自游戏空间的反馈，来验证我们的实现是否符合预期。因此我们在这里提前实现它。
### 蛇

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01PLODHI1CWnv7gzRRc_!!6000000000089-2-tps-682-137.png" style="zoom:50%;" />
</div>

```c
static uint8_t icon_data_snake1_4_4[] = {0x0f, 0x0f, 0x0f, 0x0f};	// 纯色方块
static icon_t icon_snake1_4_4 = {icon_data_snake1_4_4, 4, 4, NULL};

static uint8_t icon_data_snake0_4_4[] = {0x09, 0x09, 0x03, 0x03};	// 纹理方块
static icon_t icon_snake0_4_4 = {icon_data_snake0_4_4, 4, 4, NULL};

void draw_snake()
{
    uint16_t i = 0;

    OLED_Icon_Draw(
        map.border_left + snake.XPos[i] * map.block_size, 
        map.border_top + snake.YPos[i] * map.block_size, 
        &icon_snake0_4_4, 
        0
    );	// 蛇尾一定使用纹理方块

    for (; i < snake.length - 2; i++)
    {
        OLED_Icon_Draw(
            map.border_left + snake.XPos[i] * map.block_size, 
            map.border_top + snake.YPos[i] * map.block_size, 
            ((i % 2) ? &icon_snake1_4_4 : &icon_snake0_4_4), 
            0);
    }	// 蛇身交替使用纯色和纹理方块 来模拟蛇的花纹

    OLED_Icon_Draw(
        map.border_left + snake.XPos[i] * map.block_size, 
        map.border_top + snake.YPos[i] * map.block_size, 
        &icon_snake1_4_4, 
        0
    );	// 蛇头一定使用纯色方块
}
```
### 食物

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01nNY7wm1TIvNus5RHz_!!6000000002360-2-tps-137-137.png" style="zoom:50%;" />
</div>

```c
static uint8_t icon_data_food_4_4[] = {0x06, 0x09, 0x09, 0x06};
static icon_t icon_food_4_4 = {icon_data_food_4_4, 4, 4, NULL};

void draw_food()
{
    if (food.eaten == 0)	// 如果食物没被吃掉
    {
        OLED_Icon_Draw(
            map.border_left + food.x * map.block_size, 
            map.border_top + food.y * map.block_size, 
            &icon_food_4_4, 
            0);
    }
}
```
## 对象行为
### 蛇的运动
在贪吃蛇中，对象蛇发生运动，有两种情况，一是在用户无操作的情况下，蛇按照目前的方向继续运动，而是用户按键触发蛇的运动。总而言之，都是蛇的运动，只是运动的方向不同，所以我们可以将蛇的行为抽象为 
void Snake_Run(uint8_t dir)。
这里以向上走为例。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01l6K3Ls292AlGKKn17_!!6000000008009-1-tps-3288-1188.gif" style="zoom:25%;" />
</div>

```c
void Snake_Run(uint8_t dir)
{
    switch (dir)
    {
    	// 对于右移
        case SNAKE_UP:
            // 如果当前方向是左则不响应 因为不能掉头
            if (snake.cur_dir != SNAKE_DOWN)
            {
                // 将蛇身数组向前移
                // 值得注意的是，这里采用数组起始(XPos[0],YPos[0])作为蛇尾，
                // 而使用(XPos[snake.length - 1], YPos[snake.length - 1])作为蛇头
                // 这样实现会较为方便
                for (uint16_t i = 0; i < snake.length - 1; i++)
                {
                    snake.XPos[i] = snake.XPos[i + 1];
                    snake.YPos[i] = snake.YPos[i + 1];
                }
                // 将蛇头位置转向右侧 即 snake.XPos[snake.length - 2] + 1
                snake.XPos[snake.length - 1] = snake.XPos[snake.length - 2];
                snake.YPos[snake.length - 1] = snake.YPos[snake.length - 2] - 1;
                snake.cur_dir = dir;
            }
            break;
        case SNAKE_LEFT:
            ...
        case SNAKE_DOWN:
			...
        case SNAKE_RIGHT:
			...
            break;
    }
    
	// 检查蛇是否存活
    check_snake_alive();
    // 检查食物状态
    check_food_eaten();
    // 更新完所有状态后绘制蛇和食物
    draw_snake();
    draw_food();
}
```
### 死亡判定
在蛇每次运动的过程中，都涉及到对整个游戏新的更新，包括上述过程中出现的 check_snake_alive check_food_eaten 等。
对于 check_snake_alive， 分为两种情况：蛇碰到地图边界/蛇吃到自己。
```c
void check_snake_alive()
{
    // 判断蛇头是否接触边界
    if (snake.XPos[snake.length - 1] < 0 ||
        snake.XPos[snake.length - 1] >= snake_game.pos_x_max ||
        snake.YPos[snake.length - 1] < 0 ||
        snake.YPos[snake.length - 1] >= snake_game.pos_y_max)
    {
        snake.alive = 0;
    }
    
    // 判断蛇头是否接触自己
    for (int i = 0; i < snake.length - 1; i++)
    {
        if (snake.XPos[snake.length - 1] == snake.XPos[i] && snake.YPos[snake.length - 1] == snake.YPos[i])
        {
            snake.alive = 0;
            break;
        }
    }
}
```
### 吃食判定
在贪吃蛇中，食物除了被吃的份，还有就是随机生成。生成食物在上一节已经实现，因此这一节我们就来实现检测食物是否被吃。
```c
void check_food_eaten()
{
    // 如果蛇头与食物重合 
    if (snake.XPos[snake.length - 1] == food.x && snake.YPos[snake.length - 1] == food.y)
    {
        // 说明吃到了食物
        food.eaten = 1;
        // 增加蛇的长度
        snake.length++;
        // 长度增加表现为头的方向延伸
        snake.XPos[snake.length - 1] = food.x;
        snake.YPos[snake.length - 1] = food.y;
        // 游戏得分增加
        snake_game.score++;
        // 重新生成食物
        gen_food();
    }
}
```
## 绑定用户操作
在贪吃蛇中，唯一的用户操作就是用户按键触发蛇的运动。好在我们已经对这个功能实现了良好的封装，即void Snake_Run(uint8_t dir)
我们只需要在按键回调函数中，接收来自底层上报的key_code即可。
```c
#define SNAKE_UP 	EDK_KEY_2
#define SNAKE_LEFT 	EDK_KEY_1
#define SNAKE_RIGHT EDK_KEY_3
#define SNAKE_DOWN 	EDK_KEY_4

void greedySnake_key_handel(key_code_t key_code)
{
    Snake_Run(key_code);
}
```
## 游戏全局控制
在这个主循环里，我们需要对游戏整体进行刷新、绘图，对玩家的输赢、得分进行判定，并提示玩家游戏结果。
```c
void greedySnake_task(void)
{
    while (1)
    {
        if (snake.alive)
        {
 			// 清除屏幕memory
            OLED_Clear();
            // 绘制地图边界
            OLED_DrawRect(11, 1, 118, 62, 1);
            // 绘制“SCORE”
            OLED_Icon_Draw(3, 41, &icon_scores_5_21, 0);
            // 绘制玩家当前分数
            draw_score(snake_game.score);
            // 让蛇按当前方向运行
            Snake_Run(snake.cur_dir);
            // 将屏幕memory输出
            OLED_Refresh_GRAM();
			// 间隔200ms
            aos_msleep(200);
        }
        else
        {
            // 清除屏幕memory
            OLED_Clear();
        	// 提示 GAME OVER
            OLED_Show_String(30, 24, "GAME OVER", 16, 1);
            // 将屏幕memory输出
            OLED_Refresh_GRAM();
            // 间隔500ms
            aos_msleep(500);
        }
    }
}
```
## 实现效果
接下来请欣赏笔者的操作。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01pMnXKQ1eoUSHcZoof_!!6000000003918-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

