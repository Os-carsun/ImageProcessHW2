#include "image.h"
#include <stdlib.h>
#include <math.h>

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<7) {
		printf("ƒp??[ƒ^‚ª‘«‚è‚Ü‚¹‚ñ\n");
		return;
	}


	res=readBMPfile(av[1],&img);
	if(res<0) {
		printf("‰æ?‚ª“Ç‚ß‚Ü‚¹‚ñ(%d)\n",res);
		return;
	}


	outimg=createImage(img->width,img->height,24);
	
	effect(img,outimg,atoi(av[3]),atoi(av[4]),atoi(av[5]),atoi(av[6]));

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}

void setMatrix(double *mat,int idx,double x)
{
	mat[idx  ]=x*x*x;
	mat[idx+1]=x*x;
	mat[idx+2]=x;
	mat[idx+3]=1.0;
}

int effect(ImageData *img,ImageData *outimg,int i1,int o1,int i2,int o2)
{
	int x,y;
	int i;
	int val;
	double gamma;
	double xx;
	int color_tbl[256];
	Pixel pix;
	int res;
	double mat[4*4],vec[4],ans[4];

	vec[0]=0.0;
	setMatrix(mat , 0 , 0.0);
	vec[1]=(double)o1;
	setMatrix(mat , 4 , (double)i1);
	vec[2]=(double)o2;
	setMatrix(mat , 8 , (double)i2);
	vec[3]=255.0;
	setMatrix(mat ,12 , 255.0);

	res=solveSystemOfLinearEquations(4,mat,vec,ans);
	if(res<0) {
		printf("Illegal parameter\n");
		return 1;
	}

	for(i=0;i<256;i++) {
		xx=(double)i;
		color_tbl[i]=(int)( ans[0]*xx*xx*xx  + ans[1]*xx*xx + ans[2]*xx +ans[3] );
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
