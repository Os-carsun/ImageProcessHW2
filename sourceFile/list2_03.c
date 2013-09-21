#include "image.h"
#include <stdlib.h>
#include <math.h>

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
	if(res<0) {
		printf("Вс(%d)\n",res);
		return;
	}


	outimg=createImage(img->width,img->height,24);
	
	effect(img,outimg,atoi(av[3]));

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}
int effect(ImageData *img,ImageData *outimg,int gammaint)
{
	int x,y;
	int i;
	int val;
	double gamma;
	double a;
	int color_tbl[256];
	Pixel pix;

	gamma=(double)gammaint/100.0;

	for(i=0;i<256;i++) {
		a=(double)i/255.0;
		color_tbl[i]=(int)(pow(a,gamma)*255.0);
	}
	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);
			pix.r = color_tbl[pix.r];
			pix.g = color_tbl[pix.g];
			pix.b = color_tbl[pix.b];
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}
