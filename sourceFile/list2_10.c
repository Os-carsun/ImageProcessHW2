
#include "image.h"
#include <stdlib.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<3) {
		printf("ƒp??[ƒ^‚ª‘«‚è‚Ü‚¹‚ñ");
		return;
	}


	res=readBMPfile(av[1],&img);
	if(res<0) {
		printf("‰æ?‚ª“Ç‚ß‚Ü‚¹‚ñ");
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

    int fil[9]={
    	 1, 1, 1,
    	 1, 4, 1,
    	 1, 1, 1};
    int val;
    int x1,y1,x2,y2;
	int x,y;
	int xx,yy;
	int ff;
	int rr,gg,bb;
	Pixel pix;

	x1=0;
	y1=0;
	x2=img->width-1;
	y2=img->height-1;
	for(y=y1;y<=y2;y++) {
		for(x=x1;x<=x2;x++) {
			val=0;
			ff=0;
			rr=gg=bb=0;
			for(yy=-1;yy<=1;yy++) {			
				for(xx=-1;xx<=1;xx++) {
	    			val = getPixel(img,x+xx,y+yy,&pix);	
					rr += pix.r * fil[ff];
					gg += pix.g * fil[ff];
					bb += pix.b * fil[ff];
					ff++;
				}
			}
			pix.r=rr/12;
			pix.g=gg/12;
			pix.b=bb/12;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 1;
}
