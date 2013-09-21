#include "image.h"
#include <stdlib.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<3) {
		printf("	");
		return;
	}


	res=readBMPfile(av[1],&img);
	//printf("res=%d\n",res);
	if(res<0) {
		printf("	");
		return;
	}


	outimg=createImage(img->width,img->height,24);

	effect(img,outimg);

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}


int effect(ImageData *img,ImageData *outimg)
{
	int x,y;
	int i;
	int val;
	int pval;
	Pixel pix;
	int param1,param2;

    param1=300;
    param2=-300;
	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			val = getPixel(img,x,y,&pix);	

            pval=(pix.r+pix.g+pix.b)/3;
            if(param1>pval) param1=pval;
            if(param2<pval) param2=pval;
		}
	}
	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			val = getPixel(img,x,y,&pix);	
			pix.r = (255*(pix.r-param1))/(param2-param1);
			pix.g = (255*(pix.g-param1))/(param2-param1);
			pix.b = (255*(pix.b-param1))/(param2-param1);
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}
