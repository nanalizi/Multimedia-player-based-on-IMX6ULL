#ifndef __VIDEO_DEMO_H__
#define __VIDEO_DEMO_H__
 
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <dirent.h>
 
typedef enum
{
     STAT_FREE,//空闲
     STAT_ON,//播放
     STAT_PAUSE,//暂停
}STATING;
 
typedef enum
{
     CYCLE_ORDER,//顺序播放
     CYCLE_SINGLE,//单曲循环
     CYCLE_RANDOM,//随机播放
}VIDEO_MOD;

 
extern int GetChoose(void);//接收选项
extern void Start_Pause(void);//开始、暂停
extern void StopOff(void); // 停止
extern void LastFile(void); // 上一个
extern void NextFile(void); // 下一个
extern void SpeedOn(void); // 倍速播放
extern void Location(void); // 定位
extern void QuitApp(void); // 退出
extern void VideoMode(void); // 播放方式
extern void GetVideoFile(void);
extern void DayBook(char *CmdSentence);
extern void Video_List(void); //音视频界面逻辑
extern void LookList(void); // 查看播放列表


#endif