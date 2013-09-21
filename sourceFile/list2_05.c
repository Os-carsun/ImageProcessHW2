#include "image.h"
#include <stdlib.h>
#include <math.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<2) {
		printf("	\n");
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


int getMinMax(int *hist,int *min,int *max,int *pixs_min,int *pixs_max,int ave)
{
	int i;
	int rest;
	int now;
	int pixels;
	int a,b;

	rest=0;
	now=0;
	for(i=0;i<256;i++) {
		pixels=rest+hist[i];
		min[i]=now;
		if(rest>0) pixs_min[i]=ave-rest;
		else pixs_min[i]=ave+100;
		a=pixels/ave;
		rest=pixels%ave;
		max[i]=now+a;
		if(rest>0) pixs_max[i]=rest;
		else pixs_max[i]=ave+100;
		now+=a;
	}
	for(i=0;i<255;i++) {
		if(min[i]>255) min[i]=255;
		if(max[i]>255) max[i]=255;
	}
	max[255]=255;
	pixs_max[255]=ave*10;
}


int getHistEqu(int x,int *now,int *min,int *max,int *pix_min,int *pix_max)
{
	int res;

	if(now[x]==max[x]) {
		if(pix_max[x]<=0) now[x]=min[x];
		else pix_max[x]--;
	}
	if(now[x]==min[x]) {
		if(pix_min[x]<=0) now[x]=min[x]+1;
		else pix_min[x]--;
	}
	if(now[x]>max[x]) now[x]=min[x];
	res=now[x];
	now[x]++;
	return res;
}

int effect(ImageData *img,ImageData *outimg)
{
	int x,y;
	int rr,gg,bb;
	int ro,go,bo;
	int i;
	int val;
	int ave;
	int histR[256],histG[256],histB[256];			
	int omaxR[256],omaxG[256],omaxB[256];			
	int ominR[256],ominG[256],ominB[256];			
	int pix_maxR[256],pix_maxG[256],pix_maxB[256];	
	int pix_minR[256],pix_minG[256],pix_minB[256];	
	int onowR[256],onowG[256],onowB[256];
	Pixel pix;

	for(i=0;i<256;i++) {
		histR[i]=histG[i]=histB[i]=0;
	}
	ave= img->height * img->width /256;  

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);	
			rr=pix.r;
			gg=pix.g;
			bb=pix.b;
			histR[rr]++;
			histG[gg]++;
			histB[bb]++;
		}
	}

	getMinMax(histR,ominR,omaxR,pix_minR,pix_maxR,ave);
	getMinMax(histG,ominG,omaxG,pix_minG,pix_maxG,ave);
	getMinMax(histB,ominB,omaxB,pix_minB,pix_maxB,ave);
	for(i=0;i<256;i++) {
		onowR[i]=ominR[i];
		onowG[i]=ominG[i];
		onowB[i]=ominB[i];
	}

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);
			ro=pix.r;
			go=pix.g;
			bo=pix.b;
			rr=getHistEqu(ro,onowR,ominR,omaxR,pix_minR,pix_maxR);
			gg=getHistEqu(go,onowG,ominG,omaxG,pix_minG,pix_maxG);
			bb=getHistEqu(bo,onowB,ominB,omaxB,pix_minB,pix_maxB);
			pix.r=rr;
			pix.g=gg;
			pix.b=bb;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}
