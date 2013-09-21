#include "image.h"
#include <stdlib.h>

#define COLORS 256

main(int ac,char *av[])
{
	ImageData *img,*outimg;
	int res;
	int x,y,mx,my;

	if(ac<4) {
		printf("		");
		return;
	}


	res=readBMPfile(av[1],&img);
	if(res<0) {
		printf("		");
		return;
	}


	outimg=createImage(img->width,img->height,24);
	
	effect(img,outimg,atoi(av[3]),atoi(av[4]));

	writeBMPfile(av[2],outimg);
	disposeImage(img);
	disposeImage(outimg);

}

// ######  ######    	#
// #               # 		#
// #               #			#
// ######  ######	#
// #               #			#
// #	         #			#
// ######  #			#
int effect_1(ImageData *img,ImageData *outimg,int param1,int param2)
{
	int x,y;
	int i;
	int val;
	Pixel pix;

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			val = getPixel(img,x,y,&pix);	
			pix.r = ((param2-param1)*pix.r)/255+param1;
			pix.g = ((param2-param1)*pix.g)/255+param1;
			pix.b = ((param2-param1)*pix.b)/255+param1;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}


// ######  ######    	######
// #               # 			 #
// #               #				 #
// ######  ######	######
// #               #			#
// #	         #			#
// ######  #			######

int effect_2(ImageData *img,ImageData *outimg)
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

// ######  ######    	######
// #               # 			 #
// #               #				 #
// ######  ######	######
// #               #				 #
// #	         #				 #
// ######  #			######

int effect_3(ImageData *img,ImageData *outimg,int gammaint)
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
// ######  ######    	#	 #
// #               # 		#	 #
// #               #			#	 #
// ######  ######	######
// #               #				 #
// #	         #				 #
// ######  #				 #
int effect_4(ImageData *img,ImageData *outimg,int gammaint)
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
// ######  ######    	######	
// #               # 		#	 
// #               #			#	 
// ######  ######	######
// #               #				 #
// #	         #				 #
// ######  #			######
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

int effect_5(ImageData *img,ImageData *outimg)
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
// ######  ######    	######	
// #               # 		#	 
// #               #			#	 
// ######  ######	######
// #               #			#	 #
// #	         #			#	 #
// ######  #			######
void setMatrix(double *mat,int idx,double x)
{
	mat[idx  ]=x*x*x;
	mat[idx+1]=x*x;
	mat[idx+2]=x;
	mat[idx+3]=1.0;
}

int effect_6(ImageData *img,ImageData *outimg,int i1,int o1,int i2,int o2)
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
// ######  ######    	######	
// #               # 		#	 #
// #               #			#	 #	 
// ######  ######	           #
// #               #			         #
// #	         #			        #
// ######  #			       #
int effect_7(ImageData *img,ImageData *out)
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
// ######  ######    	######	
// #               # 		#	 #
// #               #			#	 #
// ######  ######	######
// #               #			#	 #
// #	         #			#	 #
// ######  #			######
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

int effect_8(ImageData *img,ImageData *out,int wave)
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

// ######  ######    	######	
// #               # 		#	 #
// #               #			#	 #
// ######  ######	######
// #               #				 #
// #	         #				 #
// ######  #			######

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

// ######  ######     #	######	
// #               # 	     #	#	 #
// #               #		     #	#	 #
// ######  ######     #	#	 #
// #               #		     #	#	 #
// #	         #		     #	#	 #
// ######  #		     #	######

int effect_10(ImageData *img,ImageData *outimg)
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
// ######  ######        #	             #	
// #               # 	        #		 #
// #               #		        #		 #
// ######  ######        #		 #
// #               #		        #		 #
// #	         #		        #		 #
// ######  #		        #		 #
int effect_11(ImageData *img,ImageData *outimg)
{

    int fil[9]={
    	 0,-1, 0,
    	-1, 0, 1,
    	 0, 1, 0};
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
			pix.r=rr;
			pix.g=gg;
			pix.b=bb;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 1;
}
// ######  ######        #	             ######	
// #               # 	        #		 	  #
// #               #		        #		 	  #
// ######  ######        #		 ######
// #               #		        #		 #
// #	         #		        #		 #
// ######  #		        #		 ######

int effect_12(ImageData *img,ImageData *outimg)
{

    int fil[9]={
    	 0,-1, 0,
    	 -1, 5,-1,
    	 0,-1, 0};
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
			pix.r=rr;
			pix.g=gg;
			pix.b=bb;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 1;
}

// ######  ######        #	             ######	
// #               # 	        #		 	  #
// #               #		        #		 	  #
// ######  ######        #		 ######
// #               #		        #		 	  #
// #	         #		        #		 	  #
// ######  #		        #		 ######

int effect_13(ImageData *img,ImageData *outimg)
{

    int fil[9]={
    	 0, 0, 0,
    	-1, 0, 1,
    	 0, 0, 0};
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
			pix.r=rr/4+128;
			pix.g=gg/4+128;
			pix.b=bb/4+128;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 1;
}
