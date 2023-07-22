#include "touch.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>

 #include <linux/input.h>
 #include <linux/input-event-codes.h>

 #include <unistd.h>
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4


//宏定义
//设备文件
#define TOUCH_PATH "/dev/input/event2"

int GetDirection()
{
    
    
    int fd = open(TOUCH_PATH,O_RDONLY);//只读打开
    if(fd<0)
    {
        perror("open fail");
        return 0;
    }

    int x_start=-1,y_start=-1;	//坐标的初值
    int x_end = -1,y_end = -1;    //坐标终点
    
    struct input_event ev;
    while(1)
    {
        //2.不停地从文件中读取数据
        read(fd, &ev, sizeof(struct input_event));
        
        //3.解析数据
        if(ev.type == EV_ABS)   //触摸事件 
        {
            if(ev.code == ABS_X)
            {
                if (-1 == x_start)        //x轴
                {
                    x_start = ev.value;	//起点
                }
                x_end = ev.value;      //终点
                
            }
            if(ev.code == ABS_Y)		//y轴
            {
                if (-1 == y_start)
                {
                    y_start = ev.value;
                }
                y_end = ev.value;      //终点
                
            }
            if(ev.code ==ABS_PRESSURE && ev.value == 0)
            {
                if(x_start != -1 && y_start != -1)
                {
                    break;
                }
            }
        }
        
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)   //按键事件
        {
            if(x_start != -1 && y_start != -1)
            {
                break;
            }
        }

        if (abs(x_end - x_start) > (y_end - y_start))
        {
            if (x_end - x_start > 0)
            {
                return 4;
            }
            else
            {
                return 3;
            }
            
        }
        if (abs(x_end - x_start) < (y_end - y_start))
        {
            if (y_end - y_start > 0)
            {
                return 2;
            }
            else
            {
                return 1;
            }
            
        }
        

    }
    
    
   


}
