#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#include"bmp-show.h"
#include"touch.h"
#include "videodemo.h"

char * mainbmp[]={"/mnt/linux-project/picture/2.bmp"};
char * musicbmp[]={"/mnt/linux-project/picture/music.bmp"};
FB_INFO fb_info;
IMG_INFO img_info;
struct input_event ev;
void signal_handler(int sig){
	if(sig==SIGINT){
		printf("the touch had SIGINT\n");
		close(fb_info.fbfd);
		exit(1);
	}
}
int GetChoose(void);
void SwitchChoose(int tmp);
int main()
{	
    int layer=1;
	int p_res;
	int x,y;
	int touch_fd = open("/dev/input/event2",O_RDONLY);//只读打开

    if(touch_fd<0)
    {
        perror("touch_fd open fail");
        return 0;
    }
	
	fb_info.fbfd = open("/dev/fb0", O_RDWR);
	if (!fb_info.fbfd) {
		printf("Error: cannot open framebuffer device(/dev/fb0).\n");
		return -1;
	}

    mkfifo("fifo_mp3",0777);
    GetVideoFile();//获取媒体文件
    show_picture(mainbmp[0]); 

    while(1){
        // if(layer==0){
        //    show_picture(mainbmp[0]); 
        // }
        
        // int choose = GetChoose();
        // SwitchChoose(choose);
        
        read(touch_fd, &ev, sizeof(struct input_event));
        if(ev.type==EV_ABS){
            if(ev.code==ABS_X){
                usleep(100000);
                x=ev.value;
            }
                
            else if(ev.code==ABS_Y){
                usleep(100000);
                y=ev.value;
            }
                
        }
        //player address
        if(x>130 && x<263 && y>213 && y<362 && layer==1){
            show_picture(musicbmp[0]);
            layer=2;
            
        }
        //播放事件
        if(x>315 && x<471 && y>175 && y<300 && layer==2){
            sleep(1);
            Start_Pause();
        }
        //退出事件
        if(x>265 && x<516 && y>373 && y<470 && layer==2){
            
            printf("\n\nback main picture\n\n");
            layer=1;
            show_picture(mainbmp[0]);
        }
        //下一首
        if(x>74 && x<175 && y>211 && y<295 && layer==2){
            sleep(1);
            NextFile();
        }
        //上一首
        if(x>605 && x<703 && y>210 && y<295 && layer==2){
            sleep(1);
            LastFile();
        }
        //picture
        if(x>588 && x<706 && y>213 && y<335 && layer==1){
            layer=2;
            pid_t p_process;
        	p_process=fork();
            if(p_process==0){
                execl("/mnt/linux-project/picture-show","./",NULL);
                

                // // pthread_create(&p_thread,NULL,photo_thread,NULL);
                // struct sigaction act;
                // act.sa_handler=signal_handler;
                // sigemptyset(&act.sa_mask);
                // act.sa_flags=SA_RESETHAND;
                // if (sigaction(SIGINT, &act, NULL) == -1) {
                //     perror("sigaction");
                //     exit(1);
                // }
                // photo_thread();
               
	        }
           
        }
       
        }


	


	


	close(fb_info.fbfd);
	return 0;
}

// int GetChoose(void) // 接收选项
// {
//      char tmp[4];
//      fgets(tmp, 4, stdin);
//      return atoi(tmp);
// }
// void SwitchChoose(int tmp)
// {
//      switch (tmp)
//      {
//      case 1:
//           LookList(); // 查看播放列表
//           break;
//      case 2:
//           Start_Pause(); // 开始/暂停
//           break;
//      case 3:
//           StopOff(); // 停止
//           break;
//      case 4:
//           LastFile(); // 上一个
//           break;
//      case 5:
//           NextFile(); // 下一个
//           break;
//      case 6:
//           SpeedOn(); // 倍速播放
//           break;
//      case 7:
//           Location(); // 定位
//           break;
//      case 8:
//           VideoMode(); // 播放方式
//           break;
//      case 9:
//           QuitApp(); // 退出
//           break;
 
//      default:
//           printf("\33[31m\33[1m输入越界，请重新输入！！！\33[0m\n");
//           sleep(2);
//           break;
//      }
// }

