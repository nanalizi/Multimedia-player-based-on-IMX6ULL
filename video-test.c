#include "videodemo.h"

int GetChoose(void);
void SwitchChoose(int tmp);


int main()
{	
    mkfifo("fifo_mp3",0777);
    GetVideoFile();//获取媒体文件
    while(1){
        int choose = GetChoose();
        SwitchChoose(choose);
    }
}

int GetChoose(void) // 接收选项
{
     char tmp[4];
     fgets(tmp, 4, stdin);
     return atoi(tmp);
}
void SwitchChoose(int tmp)
{
     switch (tmp)
     {
     case 1:
          LookList(); // 查看播放列表
          break;
     case 2:
          Start_Pause(); // 开始/暂停
          break;
     case 3:
          StopOff(); // 停止
          break;
     case 4:
          LastFile(); // 上一个
          break;
     case 5:
          NextFile(); // 下一个
          break;
     case 6:
          SpeedOn(); // 倍速播放
          break;
     case 7:
          Location(); // 定位
          break;
     case 8:
          VideoMode(); // 播放方式
          break;
     case 9:
          QuitApp(); // 退出
          break;
 
     default:
          printf("\33[31m\33[1m输入越界，请重新输入！！！\33[0m\n");
          sleep(2);
          break;
     }
}

