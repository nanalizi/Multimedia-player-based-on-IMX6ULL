out_file_name = "touch_bmp_app"

all:  touch.c touch.h 2-bmp-show.c bmp-show.h videodemo.c videodemo.h main.c 
	arm-linux-gnueabihf-gcc $^ -l pthread -o $(out_file_name)


.PHONY: clean
clean:
	rm $(out_file_name)

	
