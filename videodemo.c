#include "videodemo.h"

/**
 * 应用程序---业务逻辑
 * 用户态与内核态是相互隔离的
 * 
 * 系统调用（C接口）：用户空间进入内核空间的入口
 * 
 * C/C++库函数：对系统调用的封装
 * 
 * 不局限编程语言
*/
char Mplayerbuf[4096] = {0};//缓冲区
int FileStat = 0; // 切换媒体文件字符串数组
STATING stat_get = STAT_FREE;//播放状态
VIDEO_MOD MODING = CYCLE_ORDER;//播放模式
char *filename[1024] = {0};//指针数组，文件名字
int filenum = 0;//文件数

void GetVideoFile(void)//获取媒体文件
{
     DIR *fdir;
     fdir = opendir("/home/video");
     if (NULL == fdir)
     {
          perror("fail to open");
          exit(0);
     }
     struct dirent *pdir;
     while (1)
     {
          pdir = readdir(fdir);
          if (NULL == pdir)
               break;
          char *tmpbuf = pdir->d_name;
          while ('\0' != *tmpbuf) // 找末尾
          {
               tmpbuf++;
          }
          while ('.' != *tmpbuf) // 找 .
          {
               tmpbuf--;
          }
          if (!strcmp(tmpbuf, ".mp3") || !strcmp(tmpbuf, ".mp4") || !strcmp(tmpbuf, ".avi") || !strcmp(tmpbuf, ".wma") || !strcmp(tmpbuf, ".rmvb") || !strcmp(tmpbuf, ".flv") || !strcmp(tmpbuf, ".rm"))
          {
               filename[filenum++] = pdir->d_name;
          }
     }
}
void LookList(void) // 查看播放列表
{
     system("clear");
     char tmp[32];
     printf("\33[43m\33[1m+=================================+\33[0m\n");
     printf("\33[43m\33[1m|         ***\33[31m视频播放器\33[0m\33[43m\33[1m***        |\33[0m\n");
     printf("\33[43m\33[1m+=================================+\33[0m\n");
     for (int i = 0; i < filenum; i++)
     {
          printf("\33[31m\33[1m%d. %s\33[0m\n", i + 1, filename[i]);
     }
     printf("\33[1m\33[35m~~输入“last”返回上一步\33[0m\n\n");
     printf("\33[1m\33[35m~~请输入：\33[0m");
     fgets(tmp, 32, stdin);
     if (!strcmp("last\n", tmp))
     {
          return;
     }
     int num = atoi(tmp);
     if (num >= 1 && num <= filenum)
     {
          FileStat = num - 1;
          StopOff();
          Start_Pause();
 
          return;
     }
     else
     {
          printf("\33[31m\33[1m输入不正确，请重新输入！！！\33[0m\n");
          sleep(2);
          LookList();
     }
}


void Start_Pause(void) // 开始、暂停
{
     if (stat_get == STAT_FREE) //==========开始=========
     {
          char tmpbuf[1024] = {0};
          sprintf(tmpbuf, "/home/video/%s", filename[FileStat]);
          stat_get = STAT_ON;
          pid_t pid = fork();
          if (pid > 0)
          {
               // 父进程
          }
          else if (pid == 0)
          {
 
            //    close(1);
            //    close(2);
               execlp("mplayer", "mplayer", "-slave", "-input", "file=./fifo_mp3", tmpbuf, NULL);
               perror("fail to mplayer");
          }
          else
          {
               perror("fail to fork");
               return;
          }
     }
     else //=============暂停pause============
     {
          int fd = open("./fifo_mp3", O_WRONLY);
          if (-1 == fd)
          {
               perror("fail to open");
               return;
          }
          write(fd, "pause\n", 6);
          if (stat_get == STAT_ON)
          {
               stat_get = STAT_PAUSE;
          }
          else if (stat_get == STAT_PAUSE)
          {
               stat_get = STAT_ON;
          }
 
          close(fd);
     }
}
void StopOff(void) // 停止
{
     int fd = open("./fifo_mp3", O_WRONLY | O_NONBLOCK);
     if (-1 == fd)
     {
          perror("fail to open");
          return;
     }
 
     write(fd, "stop\n", 5);
     close(fd);
     stat_get = STAT_FREE;
}
void LastFile(void) // 上一个
{
     if (MODING == CYCLE_ORDER) // 顺序循环
     {
 
          if (0 == FileStat)
          {
               printf("已是第一个文件无法切换上一个\n");
               sleep(2);
               return;
          }
          StopOff(); // 关闭当前文件
          FileStat--;
          Start_Pause(); // 播放
     }
     else if (MODING == CYCLE_SINGLE) // 单曲循环
     {
          StopOff();     // 关闭当前文件
          Start_Pause(); // 播放
     }
     else if (MODING == CYCLE_RANDOM) // 随机循环
     {
          StopOff(); // 关闭当前文件
          FileStat = rand() % filenum;
          Start_Pause(); // 播放
     }
}
void NextFile(void) // 下一个
{
     if (MODING == CYCLE_ORDER)
     {
          if (filenum == (FileStat + 1))
          {
 
               printf("最后一个文件无法切换下一个\n");
               sleep(2);
               return;
          }
          StopOff(); // 关闭当前文件
          FileStat++;
          Start_Pause(); // 播放
     }
     else if (MODING == CYCLE_SINGLE) // 单曲循环
     {
          StopOff();     // 关闭当前文件
          Start_Pause(); // 播放
     }
     else if (MODING == CYCLE_RANDOM) // 随机循环
     {
          StopOff(); // 关闭当前文件
          FileStat = rand() % filenum;
          Start_Pause(); // 播放
     }
}
void SpeedOn(void) // 倍速播放
{
     char tmp[12];
     fgets(tmp, sizeof(tmp), stdin);
     if (!strcmp("last\n", tmp))
     {
          return;
     }
     int fd = open("./fifo_mp3", O_WRONLY | O_NONBLOCK); // 管道会阻塞
     if (-1 == fd)
     {
          perror("fail to open");
          sleep(2);
          return;
     }
     int data = atoi(tmp);
     switch (data)
     {
     case 1:
          write(fd, "speed_set 1\n", 12);
          printf("\33[1m当前为 1 倍速\33[0m\n");
          sleep(2);
          break;
     case 2:
          write(fd, "speed_set 2\n", 12);
          printf("\33[1m当前为 2 倍速\33[0m\n");
          sleep(2);
          break;
     case 3:
          write(fd, "speed_set 4\n", 12);
          printf("\33[1m当前为 4 倍速\33[0m\n");
          sleep(2);
          break;
 
     default:
          printf("\33[31m\33[1m输入不正确，请重新输入！！！\33[0m\n");
          sleep(2);
          SpeedOn();
          break;
     }
     close(fd);
}
void Location(void) // 定位
{
     char tmp[12];
     char Tmpbuf[1024] = {0};
     fgets(tmp, sizeof(tmp), stdin);
     if (!strcmp("last\n", tmp))
     {
          return;
     }
 
     int fd = open("./fifo_mp3", O_WRONLY | O_NONBLOCK); // 管道会阻塞
     if (-1 == fd)
     {
          perror("fail to open");
          sleep(2);
          return;
     }
     int data = atoi(tmp);
     char tmpbuf[1024] = {0};
     if (data >= 0 && data <= 100)
     {
          sprintf(tmpbuf, "seek %d 1\n", data);
          write(fd, tmpbuf, strlen(tmpbuf));
          sprintf(Tmpbuf, "定位到%d%%", data);
          close(fd);
 
          return;
     }
     printf("\33[31m\33[1m输入不正确，请重新输入！！！\33[0m\n");
     sleep(2);
     Location();
}
void VideoMode(void) // 播放方式
{
     char tmp[12];
     fgets(tmp, sizeof(tmp), stdin);
     if (!strcmp("last\n", tmp))
     {
          return;
     }
     int fd = open("./fifo_mp3", O_WRONLY | O_NONBLOCK); // 管道会阻塞
     if (-1 == fd)
     {
          perror("fail to open");
          sleep(2);
          return;
     }
     int data = atoi(tmp);
     switch (data)
     {
     case 1:
          MODING = CYCLE_ORDER;
          printf("\33[1m当前为循环播放\33[0m\n");
          sleep(2);
          break;
     case 2:
          MODING = CYCLE_SINGLE;
          printf("\33[1m当前为单曲循环\33[0m\n");
          sleep(2);
          break;
     case 3:
          MODING = CYCLE_RANDOM;
          printf("\33[1m当前为随机播放\33[0m\n");
          sleep(2);
          break;
 
     default:
          printf("\33[31m\33[1m输入不正确，请重新输入！！！\33[0m\n");
          sleep(2);
          VideoMode();
          break;
     }
     close(fd);
}
void QuitApp(void) // 退出
{
     StopOff();
     system("clear");
     printf("\33[42m\33[1m\33[31m         期待下次再见!            \33[0m\n");
     printf("\33[1m        Mplayer已退出!!!\33[0m\n");
     exit(0);
}
void Video_List(void){
     Start_Pause();
     //界面/按钮/逻辑
}
