@page aircraftBattle 飞机大战
# 实验介绍
    飞机大战作为一款经典的街机游戏，是很多人的童年回忆。我们的 HaaS EDU K1 开发板专门设计了街机样式的按键排列，很适合我们做这类游戏的开发。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01fFHJ801HyoDT19NYi_!!6000000000827-1-tps-1200-800.gif" style="zoom:50%;" />
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


# 游戏设定
    不同于规则简单的贪吃蛇，在飞机大战这类游戏中，往往需要对游戏中出现的每个对象进行数值、行为的设定。在开发游戏前期，梳理好这些设定也有助于我们更清晰地进行开发。有时，优秀的设定也是吸引玩家的重要因素。
## 角色设定

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01d8HY3L1eJnv3GDqni_!!6000000003851-2-tps-451-241.png" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN017l0kl91MtpzerUl1V_!!6000000001493-2-tps-451-206.png" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01Bh1oSu1MWTnP9XmpP_!!6000000001442-2-tps-450-206.png" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01vX0n0x20SQvdUNLJs_!!6000000006848-2-tps-450-205.png" />
</div>


## 行为设定

    - 在本游戏中，玩家将控制阿克琉斯级战舰，在持续不断的敌机中通过闪避或攻击开辟出自己的路。
    - 玩家可以通过 HaaS EDU K1 的四个按键控制 阿克琉斯级战舰 进行前后左右运动。
    - 在游戏进行过程中，玩家的战舰会不断发射炮弹。被炮弹攻击的敌方战舰会损失响应的装甲。
    - 若玩家战舰被敌方战舰撞击，双方均会损失装甲。
    - 玩家有三次紧急修复战舰的机会。
# 游戏实现
## 游戏流程
    在开始之前，我们先使用一个简单的流程，帮助大家理解本游戏的刷新机制。这个大循化即游戏刷新所需要的所有流程。
```c
// 游戏中所有对象的更新判定由大循环维护
void aircraftBattle_task()
{
    while (1)
    {
        OLED_Clear();		// 清理屏幕数据
        global_update();	// 刷新全局对象 如更新对象的贴图状态 发射子弹 撞击判断 等
        global_draw();		// 绘制刷新完后的所有对象
        OLED_Refresh_GRAM();// 将绘制结果显示在屏幕上
        aos_msleep(40);		// 40ms 为一个游戏周期
    }
}
```

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN018hWRfi1QiAMoH4hVJ_!!6000000002009-2-tps-579-394.png" />
</div>


## 贴图实现
    对于每个对象，我们希望能够将其定位到游戏地图上的每一点，而不是单纯使用贴图函数。因此，每个对象有一个“控制坐标”，而我们相对这个“控制坐标”计算出贴图坐标。这样，如果一个对象需要变换不同尺寸的贴图，我们可以更方便地计算出它的贴图坐标。
    如图，红色为该对象的控制坐标，蓝色为该贴图的贴图坐标。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01tnIUT71Jy0l22oM7d_!!6000000001096-2-tps-384-384.png" style="zoom:50%;" />
</div>
                

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01Zm97E01G9DzcNmDtv_!!6000000000579-2-tps-384-324.png" style="zoom:50%;" />
</div>

```c
typedef struct
{
    map_t *map;				// 贴图
    int cur_x; 
    int cur_y;				// 飞行物对象的控制坐标
} dfo_t;  					// 飞行物对象
```
```c
/*
    -> x
    ____________________
    |         |    icon|
 |  |       of_y       |
 \/ |         |        |
  y |--of_x--cp        |
    |__________________|
*/

typedef struct
{
    icon_t *icon;	// 贴图对象
    int offset_x;
    int offset_y;	// 相对于控制坐标的偏移
} map_t; // 贴图
```
    注意⚠️，在开发过程中，我们使用的是竖屏模式，坐标系是以竖屏做处理。因此，在绘图时，我们需要做坐标系的转换。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01GOOOKe25dwxtslX50_!!6000000007550-2-tps-953-491.png"  />
</div>


```c
void draw_dfo(dfo_t *dfo)
{
    map_t *cur_map = get_cur_map(dfo);	// 获取当前对象的贴图
    
    // 计算对象边界
    int top = dfo->cur_y + cur_map->offset_y;	
    int bottom = dfo->cur_y + cur_map->offset_y + cur_map->icon->width;
    int left = dfo->cur_x + cur_map->offset_x;
    int right = dfo->cur_x + cur_map->offset_x + cur_map->icon->height;
    
    // 若对象超出屏幕 则不绘制
    if (top > 132 || bottom < 0 || left > 64 || right < 0)
        return;

    // 绘制坐标转换后的贴图对象
    OLED_Icon_Draw(
        dfo->cur_y + cur_map->offset_y,
        64 - (dfo->cur_x + cur_map->offset_x + cur_map->icon->height),
        cur_map->icon,
        2);
}
```
    这样，就可以实现在OLED上绘制我们设定的战舰图片了。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01EejcyI1DtzaUfeMGg_!!6000000000275-2-tps-1200-800.png" style="zoom:50%;" />
</div>


## 移动战舰
    接下来，我们要实现的是根据用户的按键输入来移动战舰的贴图。在此之前，我们需要对 dfo_t 结构体进行更多的补充。我们额外定义一个 speed 属性，用于定义在用户每次操作时移动一定的距离。
    注意，这里的前后左右均是在游戏坐标系中。
```c
typedef struct
{
    // 舰船坐标
    int cur_x; // 运动
    int cur_y;
	// 舰船速度
    uint8_t speed;      // 绝对固定
    // 舰船贴图
    map_t *map;
} dfo_t;                   // Dentified Flying Object
```
```c
typedef enum
{
    UP,
    LEFT,
    RIGHT,
    DOWN
} my_craft_dir_e_t;

void move_MyCraft(dfo_t *my_craft, my_craft_dir_e_t dir)
{
    // 获取舰船当前的贴图对象
    map_t *cur_map = get_cur_map(my_craft);
    // 计算贴图边界
    int top = my_craft->cur_y + cur_map->offset_y;
    int bottom = my_craft->cur_y + cur_map->offset_y + cur_map->icon->width;
    int left = my_craft->cur_x + cur_map->offset_x;
    int right = my_craft->cur_x + cur_map->offset_x + cur_map->icon->height;
	// 判断方向
    switch (dir)
    {
    case UP:
        // 如果这次移动不会超过地图边界 则移动
        if (!(top - my_craft->speed < 0))
            my_craft->cur_y -= my_craft->speed;
        break;
    case DOWN:
        if (!(bottom + my_craft->speed > 132))
            my_craft->cur_y += my_craft->speed;
        break;
    case LEFT:
        if (!(left - my_craft->speed < 0))
            my_craft->cur_x -= my_craft->speed;
        break;
    case RIGHT:
        if (!(right + my_craft->speed > 64))
            my_craft->cur_x += my_craft->speed;
        break;
    default:
        break;
    }
}
```
    将按键回调函数关联至移动舰船函数。注意，这里的前后左右均是在游戏坐标系中。
```c
void aircraftBattle_key_handel(key_code_t key_code)
{
    switch (key_code)
    {
    case EDK_KEY_4:
        move_MyCraft(my_craft, LEFT);
        break;
    case EDK_KEY_1:
        move_MyCraft(my_craft, UP);
        break;
    case EDK_KEY_3:
        move_MyCraft(my_craft, DOWN);
        break;
    case EDK_KEY_2:
        move_MyCraft(my_craft, RIGHT);
        break;
    default:
        break;
    }
}
```


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01PZPCi622CynOOUGGa_!!6000000007085-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


## 加一点特效
    作为一个注重细节，精益求精的开发者，我们希望给我们的舰船加上一些特效。而这需要舰船对象不断改变重绘自己的贴图。为了这个功能，我们额外创建了一个新的结构体用于管理“动画”。
```c
typedef struct
{
    map_t **act_seq_maps;     		// 贴图指针数组 该动画的所有贴图（例如爆炸动作包含3帧）
    uint8_t act_seq_len;      		// 贴图指针数组长度
    uint8_t act_seq_index;        	// 用于索引帧
    
    uint8_t act_seq_interval; 		// 帧间延迟
    uint8_t act_seq_interval_cnt; 	// 用于延迟计数
    
    uint8_t act_is_destory;			// 用于标记该动画是否是毁灭动画 若是则不再重复
} act_seq_t;
```
    同时，每个舰船对象新增了一系列属性 act_seq_type， 用于显示当前的贴图状态。例如，当 act_seq_type = 0 时，表示舰船处于正常状态，每隔 act_seq_interval 个周期切换显示一次贴图，即第一行的三帧贴图。当 act_seq_type = 1 时，表示舰船处于爆炸状态，每隔 act_seq_interval 个周期切换显示一次贴图，即第二行的三帧贴图。
    目前 act_seq_type 的含义由每个舰船对象自己定义和维护。也可以归纳成统一的枚举量，这一步读者可以自行完成。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01lFBMge1IN5CwY4VTI_!!6000000000880-2-tps-509-296.png" />
</div>


```c
typedef struct
{
    int cur_x;
    int cur_y;
    uint8_t speed;

    act_seq_t **act_seq_list; // 动画数组 包含了多个动作序列
    uint8_t act_seq_list_len; // 动画数组长度
    uint8_t act_seq_type;
} dfo_t; 
```
```c
// 正常动作序列    
act_seq_t *achilles_normal_act = (act_seq_t *)malloc(sizeof(act_seq_t));
achilles_normal_act->act_seq_maps = achilles_normal_maplist;
achilles_normal_act->act_seq_len = 3;		// 该动作序列包含3帧图片
achilles_normal_act->act_seq_interval = 10;	// 该动画帧间延迟10周期
achilles_normal_act->act_is_destory = 0;	// 该动画不是毁灭动画 即一直重复
// 毁灭动作序列    
act_seq_t *achilles_destory_act = (act_seq_t *)malloc(sizeof(act_seq_t));
achilles_destory_act->act_seq_maps = achilles_destory_maplist;
achilles_destory_act->act_seq_len = 3;
achilles_destory_act->act_seq_interval = 4;	// 该动画帧间延迟4周期
achilles_destory_act->act_is_destory = 1;
// 动作序列数组
act_seq_t **achilles_act_seq_list = (act_seq_t **)malloc(sizeof(act_seq_t *) * achilles->act_seq_list_len);
achilles_act_seq_list[0] = achilles_normal_act;
achilles_act_seq_list[1] = achilles_destory_act;
// 将舰船对象属性指向该动作序列数组
achilles->act_seq_list = achilles_act_seq_list;
achilles->act_seq_type = 0;
```
    定义完成后，我们需要在游戏的每一次循环中，更新战舰状态和贴图。
```c
void craft_update_act(dfo_t *craft)
{
    act_seq_t *cur_act_seq = craft->act_seq_list[craft->act_seq_type];
    if (cur_act_seq->act_seq_interval == 0)
        return;	// 若当前战舰无动作序列，则不进行更新
    ++(cur_act_seq->act_seq_interval_cnt);
    if (cur_act_seq->act_seq_interval_cnt >= cur_act_seq->act_seq_interval)
    {
        cur_act_seq->act_seq_interval_cnt = 0;
        ++(cur_act_seq->act_seq_index);	// 切换贴图
        if (cur_act_seq->act_seq_index >= cur_act_seq->act_seq_len)
        {
            cur_act_seq->act_seq_index = 0;
            if (cur_act_seq->act_is_destory == 1)
            {
				// 在这里处理毁灭的舰船
            }
        }
    }
}
```
    这样，我们就为战舰添加了喷气的特效。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01xPmTtB28sYRWTrbj6_!!6000000007988-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


## 移动敌机
    移动敌机的方式更简单。只需要将其向下移动即可。实现方式如下。
```c
void move_enemy(dfo_t *craft)
{
    map_t *cur_map = get_cur_map(craft);
    craft->cur_y += craft->speed;
    int top = craft->cur_y + cur_map->offset_y;
    if (top > 132)										// 当敌机飞过屏幕下方
        reload_dfo(craft, AUTO_RELOAD, AUTO_RELOAD); 	// 重载敌机
}
```
## 重载敌机
    在飞机大战中，会有持续不断的敌机生成，并且敌机的出现顺序和位置都随机。为了实现这种效果，我们采用的方式是维护一个敌机数组，当敌机飞过屏幕下方或是被击落后，我们会回收敌机并重新加载，将其重新显示在屏幕上。
```c
void reload_dfo(dfo_t *craft, int pos_x, int pos_y)
{
    craft->cur_x = craft->pos_x;
    craft->cur_y = craft->pos_y;
    
    if (pos_x == AUTO_RELOAD)	// 如果指定重载坐标为自动重载
    {
        uint16_t height = get_cur_map(craft)->icon->width;
        craft->cur_x = random() % (64 - height) + height / 2;	// 则随机生成一个坐标，且保证对象显示在地图内
    }
    if (pos_y == AUTO_RELOAD)
    {
        uint16_t width = get_cur_map(craft)->icon->height;
        craft->cur_y = -(random() % 1000) - width / 2;
    }
}
```
    这样，就能够实现源源不断的敌机了。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN013ttk521FYaJ1UKtgR_!!6000000000499-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


## 发射子弹
    对于子弹而言，它和战舰的属性非常相似，因此我们在现有的舰船对象 dfo_t 上稍加改动即可。
```c
typedef enum
{
    Achilles, 	// 阿克琉斯级
    Venture,  	// 冲锋者级
    Ares,     	// 阿瑞斯级 战神级
    TiTan,    	// 泰坦级
    Bullet,		// 子弹
} dfo_model_e_t;	// 飞行物型号

typedef struct
{
    int offset_x;
    int offset_y;	// 炮台的相对位置
} arms_t;			// 武装结构体

typedef struct
{
    dfo_model_e_t model;	// 型号
    
    // 运动相关
    int start_x; 			// 飞行物的起始位置 用于计算飞行距离
    int start_y;

    int cur_x; 				// 飞行物的当前位置
    int cur_y;

    uint8_t speed;      	// 飞行物的运动速度
    unsigned int range; 	// 射程

    // 显示相关
    act_seq_t **act_seq_list; 	// 动画数组
    uint8_t act_seq_list_len; 	// 动画数组长度
    uint8_t act_seq_type;	 	// 动画状态

    // 攻击相关
    arms_t **arms_list;    // 武器装备数组
    uint8_t arms_list_len; // 武器数组长度
} dfo_t;
```
    那么，目前 dfo_t 结构体不仅仅可以用于舰船，也可以用于定义子弹。接下来，我们为舰船定义炮台和子弹。
```c
dfo_t *create_achilles()	// 定义阿克琉斯级战舰
{
	// 贴图等其他定义

    achilles->damage = 8;		// 定义撞击伤害
    achilles->full_life = 10;	// 定义完整装甲值
    achilles->cur_life = 10;	// 初始化装甲值

    achilles->arms_list_len = 2;	// 设定炮台数为2
    achilles->arms_list = achilles_arms_list;	// 定义炮台数组

    return achilles;
}

dfo_t *create_bullet()
{
	// 贴图等其他定义	

    bullet->damage = 1;			// 定义射击伤害
    bullet->full_life = 1;		// 定义完整装甲值
    bullet->cur_life = 0;		// 初始化子弹时 默认不激活

    bullet->start_x = -100;		// 初始化子弹时 将其移出屏幕外不做处理
    bullet->start_y = -100;
    bullet->cur_x = -100;
    bullet->cur_y = -100;

    return bullet;
}
```
    为了生成持续不断的子弹，我们也采用重载的方式去生成子弹。
```c
// 检索未被激活的子弹
dfo_t *get_deactived_bullet()
{
    for (int i = 0; i < MAX_BULLET; i++)
    {
        if (bullet_group[i]->cur_life <= 0)
            return bullet_group[i];
    }
    return NULL;
}

// 触发舰船射击子弹
void shut_craft(dfo_t *craft)
{
    if (craft->arms_list == NULL || craft->arms_list_len == 0)
        return;

    // 从每个炮台重载子弹
    for (int i = 0; i < craft->arms_list_len; i++)
    {
        dfo_t *bullet = get_deactived_bullet();
        if (bullet == NULL)
            return;
        reload_dfo(bullet, craft->cur_x + craft->arms_list[i]->offset_x, craft->cur_y + craft->arms_list[i]->offset_y);
    }
}

// 在每一次刷新时移动所有子弹
void move_bullet(dfo_t *bullet)
{
    if (bullet->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(bullet);
    bullet->cur_y -= bullet->speed;
    int bottom = bullet->cur_y + cur_map->offset_y + cur_map->icon->width;
    if (bottom < 0 || (bullet->start_y - bullet->cur_y) > bullet->range)
    {
        bullet->cur_life = 0;	// 对超出射程的子弹 取消激活
        bullet->cur_x = -100;
    }
}
```


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01zhXJZ11rzZ2IWhd9I_!!6000000005702-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


## 撞击判定
    在这一步，我们将会实现对于所有对象的撞击判定，并对对象的属性做出对应的处理。简单而言，撞击判定只需要检查两个对象是否有像素点的重叠即可。
```c
// 判断两个dfo对象 bullet craft 是否发生撞击
int hit_check(dfo_t *bullet, dfo_t *craft)
{
    if (craft->cur_y <= 0 || craft->cur_x <= 0)
        return 0;
    if (craft->cur_life <= 0)
        return 0;
    if (bullet->cur_life <= 0)
        return 0;
    act_seq_t *cur_act_seq = bullet->act_seq_list[bullet->act_seq_type];
    map_t *cur_map = cur_act_seq->act_seq_maps[cur_act_seq->act_seq_index];

    for (int bullet_bit_x = 0; bullet_bit_x < (cur_map->icon->height); bullet_bit_x++)
    {
        for (int bullet_bit_y = 0; bullet_bit_y < (cur_map->icon->width); bullet_bit_y++)
        {
            uint8_t bit = (cur_map->icon->p_icon_mask == NULL) ? cur_map->icon->p_icon_data[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8) : cur_map->icon->p_icon_mask[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8);
            if (bit == 0)
                continue;

            int bit_cur_x = bullet->cur_x + cur_map->offset_x + cur_map->icon->height - bullet_bit_x;
            int bit_cur_y = bullet->cur_y + cur_map->offset_y + bullet_bit_y;

            act_seq_t *cur_craft_act_seq = craft->act_seq_list[craft->act_seq_type];
            map_t *cur_craft_map = cur_craft_act_seq->act_seq_maps[cur_craft_act_seq->act_seq_index];

            for (int craft_bit_x = 0; craft_bit_x < (cur_craft_map->icon->height); craft_bit_x++)
            {
                for (int craft_bit_y = 0; craft_bit_y < (cur_craft_map->icon->width); craft_bit_y++)
                {
                    uint8_t craft_bit = (cur_craft_map->icon->p_icon_mask == NULL) ? cur_craft_map->icon->p_icon_data[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8) : cur_craft_map->icon->p_icon_mask[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8);
                    if (craft_bit == 0)
                        continue;
                    // 找到有效点对应的绝对坐标
                    int craft_bit_cur_x = craft->cur_x + cur_craft_map->offset_x + cur_craft_map->icon->height - craft_bit_x;
                    int craft_bit_cur_y = craft->cur_y + cur_craft_map->offset_y + craft_bit_y;
                    // 开始遍历所有可撞击对象
                    if (craft_bit_cur_x == bit_cur_x && craft_bit_cur_y == bit_cur_y)
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
```
    全局撞击判定，判断地图上所有存活对象的撞击情况。
```c
void global_hit_check(void)
{
    // 子弹撞击检测
    for (int j = 0; j < MAX_BULLET; j++)
    {
        dfo_t *bullet = bullet_group[j];
        if (bullet->cur_life <= 0)
            continue;

        for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++)
        {
            dfo_t *craft = enemy_crafts[i];
            if (craft->cur_life <= 0)
                continue;

            if (hit_check(bullet, craft))
            {
                craft->cur_life -= bullet->damage;
                bullet->cur_life = 0;
                bullet->cur_x = -100;
                if (craft->cur_life <= 0)
                {
                    destory(craft);
                }
                continue;
            }
        }
    }

    // 我方飞机撞击检测
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++)
    {
        dfo_t *craft = enemy_crafts[i];
        if (craft->cur_life <= 0)
            continue;

        if (hit_check(my_craft, craft))
        {
            craft->cur_life -= my_craft->damage;
            my_craft->cur_life -= craft->damage;
            // 如果舰船装甲损毁 则摧毁舰船 将其动画状态置为毁灭动画
            if (craft->cur_life <= 0)
            {
                craft->act_seq_type = 1;
    			craft->cur_life = 0;
            }
            if (my_craft->cur_life <= 0)
            {
                my_craft->act_seq_type = 1;
    			my_craft->cur_life = 0;
                g_chance--;
            }
            continue;
        }
    }
}
```
## 全局刷新
```c
void global_update(void)
{
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++)
    {
        craft_update_act(enemy_crafts[i]);	// 更新所有敌机贴图状态
        move_enemy(enemy_crafts[i]);		// 自动移动所有敌机
    }
    for (int i = 0; i < MAX_BULLET; i++)
    {
        move_bullet(bullet_group[i]);		// 自动移动所有激活的子弹
    }
    craft_update_act(my_craft);				// 更新玩家舰船状态
    shut_craft(my_craft);					// 触发玩家舰船射击
    global_hit_check();						// 全局撞击判定
}
```
# 实现效果
    接下来请欣赏笔者的操作。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01PuExAK1BsV3zJrGs5_!!6000000000001-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

