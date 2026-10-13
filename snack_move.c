#include <curses.h>         // 使用curses图形库头文件
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//表示蛇的位置结构体
typedef struct {
    int x;
    int y;
} Point;

void generate_food(Point *food,Point snake[],int length,int width,int height);//生成食物函数
void show_start_screen(int width, int height);// 显示开始界面函数

int main() {
    //// 游戏初始化变量////
    int width = 40;                // 游戏区域宽度
    int height = 20;               // 游戏区域高度     以左上为原点
    int score = 0;                 // 分数
    int game_over = 0;              // 游戏结束标志
    int ch;

    Point snake[100];              // 蛇的身体部分数组
    int length = 4;                 // 蛇的初始长度

    // 蛇的初始位置
    int snake_x = width / 2;
    int snake_y = height / 2;
    int dir_x = 1;// 初始移动方向向右
    int dir_y = 0;

    Point food;// 食物位置
    srand(time(NULL)); // 用当前时间作为随机数种子
    show_start_screen(width, height);// 显示开始界面
    generate_food(&food, snake, length, width, height);// 生成初始食物位置

    // 初始化蛇的位置
    snake[0].x = width / 2;// 蛇头位置
    snake[0].y = height / 2;
    snake[1].x = snake[0].x - 1;
    snake[1].y = snake[0].y;
    snake[2].x = snake[0].x - 2;
    snake[2].y = snake[0].y;
    snake[3].x = snake[0].x - 3;
    snake[3].y = snake[0].y;

    //// 初始化curses////
    initscr();          // 将终端切换为图形模式
    curs_set(0);        // 隐藏光标
    noecho();           // 不显示输入字符
    keypad(stdscr, TRUE);// 启用方向键输入
    timeout(100);       // 设置输入等待时间（100毫秒）

    // 主游戏循环
    while (!game_over) {
        clear();// 清屏
        
        // 画边框
        for (int i = 0; i < width; i++) {// 上下边框
            mvprintw(0, i, "#");
            mvprintw(height-1, i, "#");
        }
        for (int i = 0; i < height; i++) {// 左右边框
            mvprintw(i, 0, "#");
            mvprintw(i, width-1, "#");
        }
        // 画食物
        mvprintw(food.y, food.x, "*");

        // 画蛇头，蛇身
        mvprintw(snake[0].y, snake[0].x, "@");
        for (int i = 1; i < length; i++) {
            mvprintw(snake[i].y, snake[i].x, "o");
        }

        // 显示游戏信息
        mvprintw(height+1, 2, "Score: %d", score);// 显示分数
        mvprintw(height+2, 2, "Position: (%d, %d)", snake[0].x, snake[0].y);// 显示蛇位置
        mvprintw(height+3, 2, "Use Arrow Keys to Move | Press 'Q' to Quit");// 显示控制说明
        refresh();//刷新屏幕
        ch = getch();// 获取玩家输入
        
        // 处理方向键
        if (ch == 'q' || ch == 'Q') {
            game_over = 1;
        } else if (ch == KEY_UP) {
            dir_x = 0;
            dir_y = -1;
        } else if (ch == KEY_DOWN) {
            dir_x = 0;
            dir_y = 1;
        } else if (ch == KEY_LEFT) {
            dir_x = -1;
            dir_y = 0;
        } else if (ch == KEY_RIGHT) {
            dir_x = 1;
            dir_y = 0;
        }
        
        // 移动蛇身体
        for(int i = length - 1; i > 0; i--) {
            snake[i].x = snake[i-1].x;
            snake[i].y = snake[i-1].y;
        }
        // 移动蛇头
        snake[0].x += dir_x;
        snake[0].y += dir_y;

        // 食物被吃掉的逻辑
        if (snake[0].x == food.x && snake[0].y == food.y) {
            score += 10;
            snake[length].x = snake[length - 1].x; // 新增蛇的身体部分位置
            snake[length].y = snake[length - 1].y;
            length++;// 增加蛇的长度
            generate_food(&food, snake, length, width, height);
        }
        
        // 检查碰撞
        if (snake[0].x <= 0 || snake[0].x >= width - 1 
            || snake[0].y <= 0 || snake[0].y >= height - 1) {
            game_over = 1;// 碰到墙壁，游戏结束
        }
        for (int i = 1; i < length; i++) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                game_over = 1;// 碰到自己，游戏结束
                break;
            }
        }
        usleep(100000);// 控制游戏速度，每0.1秒更新一次游戏状态
    }
    
    endwin();// 结束curses模式
    printf("========游戏结束!========\n\n");
    printf("最终分数: %d\n蛇的长度: %d\n", score, length);
    if(score == 0) {
        printf("你需要多加练习!\n");
    } else if(score < 50) {
        printf("不错，但还有提升空间!\n");
    } else {
        printf("太棒了! 你是个高手!\n\n");
    }
    printf("========================\n");
    return 0;
}

void generate_food(Point *food, Point snake[], int length, int width, int height) {
    int valid_position;
    int max_attempts = 1000; // 最大尝试次数，防止死循环
    do {
        valid_position = 1;
        food->x = rand() % (width - 2) + 1; // 确保食物不在边框上
        food->y = rand() % (height - 2) + 1;
        // 检查食物位置是否与蛇身重叠
        for (int i = 0; i < length; i++) {
            if (food->x == snake[i].x && food->y == snake[i].y) {
                valid_position = 0; // 重叠，重新生成
                break;
            }
        }
        max_attempts--;
        if (max_attempts <= 0) {
            food->x = 5;
            food->y = 5;
            break; // 防止死循环
        }
    } while (!valid_position);// 直到找到有效位置
}

void show_start_screen(int width, int height) {
    //临时初始化curses用于开始界面
    initscr();
    curs_set(0);
    noecho();
    clear();
    //绘制标题边框
    for(int i=0;i<width;i++){
        mvprintw(0,i,"#");
        mvprintw(height-1,i,"#");
    }
    for(int i=0;i<height;i++){
        mvprintw(i,0,"#");
        mvprintw(i,width-1,"#");
    }
    //显示游戏标题
    int center_x=width/2;
    mvprintw(5,center_x-6,"================");
    mvprintw(6,center_x-6,"===Snake Game===");
    mvprintw(7,center_x-6,"================");
    mvprintw(10,center_x-2,"@ooo");//蛇的示例
    mvprintw(11,center_x-2,"This is the snake");
    mvprintw(13,center_x-2,"*");//食物示例
    mvprintw(14,center_x-2,"This is the food");
    mvprintw(16,10,"Control:");
    mvprintw(17,10,"Arrow keys:Move the snake");
    mvprintw(18,10,"Q     :Quit game");
    mvprintw(height-3,center_x-10,"Press any key to start...");
    refresh();//刷新显示
    getch(); // 等待按键
    endwin();//结束开始界面的curses
}