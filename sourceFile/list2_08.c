#include "image.h"
#include <stdlib.h>
#include <math.h>

#define COLORS 256

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<4) {
		printf("ƒp??[ƒ^‚ª‘«‚è‚Ü‚¹‚ñ\n");
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

void makeColorTable(int *tbl,int wave)
{
	int i,j;
	int val;
	int mts;
	int center;


	mts=COLORS/wave;

	center=COLORS/(wave*2);
	for(i=0;i<COLORS;i++) {
		//j=i/mts;
		j=i%mts;
		if(j<=center) {
			val=j*(COLORS)/(center);
		}
		else {
			val=-(j-center+1)*(COLORS)/center+COLORS;
		}
		if(val<0) val=0;
		if(val>=COLORS) val=COLORS-1;
		tbl[i]=val;
	}
}

int effect(ImageData *img,ImageData *out,int wave)
{
	int x,y;
	int val;
	int sora_tbl[COLORS];
	Pixel pix,pix2;

	makeColorTable(sora_tbl, wave);
	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);	
			pix2.r = sora_tbl[pix.r];
			pix2.g = sora_tbl[pix.g];
			pix2.b = sora_tbl[pix.b];
			setPixel(out,x,y,&pix2);	
		}
	}
	return 0;
}
