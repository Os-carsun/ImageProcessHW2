#include "image.h"
#include <stdlib.h>
#include <math.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<4) {
		printf("	");
		return;
	}


	res=readBMPfile(av[1],&img);
	if(res<0) {
		printf("‰æ?‚ª“Ç‚ß‚Ü‚¹‚ñ(%d)\n",res);
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
		a=(double)(i-128)/(255.0);
		a*=gamma;
		color_tbl[i]=(int) (255.0/(1.0+exp(-a)) );
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
