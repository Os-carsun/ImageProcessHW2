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

	effect(img,outimg,atoi(av[3]));

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}



int effect(ImageData *img,ImageData *outimg,int ef)
{
	int x,y;
	int i;
	int val;
	Pixel pix;

	int rate;
	int sumr[20],sumg[20],sumb[20];
	int smx;
	int sq;
	int rr,gg,bb;
	int x1,y1,x2,y2;
	int xx,yy;

	x1=0;
	y1=0;
	x2=img->width-1;
	y2=img->height-1;

	sq=(2*ef+1)*(2*ef+1);

	for(y=y1;y<=y2;y++) {
		x=x1;
		for(xx=-ef;xx<ef;xx++) {
			smx=xx+ef;
			sumr[smx]=sumg[smx]=sumb[smx]=0;
			for(yy=-ef;yy<=ef;yy++) {
				val = getPixel(img,x+xx,y+yy,&pix);	

				sumr[smx]+=pix.r;
				sumg[smx]+=pix.g;
				sumb[smx]+=pix.b;
			}
		}
		for(x=x1;x<=x2;x++) {
			rr=gg=bb=0;
			smx=ef+ef;
			sumr[smx]=sumg[smx]=sumb[smx]=0;
			for(yy=-ef;yy<=ef;yy++) {
				val = getPixel(img,x+ef,y+yy,&pix);	
				sumr[smx]+=pix.r;
				sumg[smx]+=pix.g;
				sumb[smx]+=pix.b;
			}
			for(xx=0;xx<=smx;xx++) {
				rr+=sumr[xx];
				gg+=sumg[xx];
				bb+=sumb[xx];
				sumr[xx]=sumr[xx+1];
				sumg[xx]=sumg[xx+1];
				sumb[xx]=sumb[xx+1];
			}
			pix.r = rr/sq;
			pix.g = gg/sq;
			pix.b = bb/sq;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}
