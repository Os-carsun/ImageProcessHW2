#include "image.h"
#include <stdlib.h>
#include <math.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<2) {
		printf("ƒp??[ƒ^‚ª‘«‚è‚Ü‚¹‚ñ\n");
		return;
	}


	res=readBMPfile(av[1],&img);
	if(res<0) {
		printf("‰æ?‚ª“Ç‚ß‚Ü‚¹‚ñ(%d)\n",res);
		return;
	}


	outimg=createImage(img->width,img->height,24);
	
	effect(img,outimg);

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}
int effect(ImageData *img,ImageData *out)
{
	int x,y;
	Pixel pix,pix2;

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);

			pix2.r = 255 - pix.r;
			pix2.g = 255 - pix.g;
			pix2.b = 255 - pix.b;
			setPixel(out,x,y,&pix2);	
		}
	}
	return 0;
}
