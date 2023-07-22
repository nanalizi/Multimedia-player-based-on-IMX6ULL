#include"bmp-show.h"
#include"touch.h"
extern FB_INFO fb_info;
extern IMG_INFO img_info;
char * bmpname[] = {"/mnt/linux-project/picture/6.bmp","/mnt/linux-project/picture/3.bmp","/mnt/linux-project/picture/4.bmp"};
void *photo_thread(void){
	fb_info.fbfd = open("/dev/fb0", O_RDWR);
	if (!fb_info.fbfd) {
		printf("Error: cannot open framebuffer device(/dev/fb0).\n");
		return -1;
	}
	int rs = 0;
    int i = 0;
	
	while(1){
		
		usleep(50000);
		rs = GetDirection();
        printf("%d\n",rs);
        if (1 == rs || 3 == rs)
        {
            if (3 == i)
            {
                i = 0;
            }
            else
                ++i;
            show_picture(bmpname[i]);
			
            
        }
      
        else if (2 == rs || 4 == rs)
        {
            if (0 == i)
            {
                i = 3;
            }
            else
                --i;
            // bmp_player(i);
            show_picture(bmpname[i]);
        }




	 }
}
int main(){
    printf("\nSTART show picture\n");
    photo_thread();
}