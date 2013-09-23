#include <stdio.h> 
#include <math.h>  
#include "image.h"

#define COLORS 256



// ######  ######       #
// #       # 		#
// #       #		#
// ######  ######	#
// #       #		#
// #	   #		#
// ######  #		#
int effect_1(ImageData *img,ImageData *outimg,int param1,int param2)
{
	//線性處裡 
	int x,y;
	int i;
	int val;
	Pixel pix;

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			val = getPixel(img,x,y,&pix);//取的圖片中的每個PIXEL
			//線性函數
			// (( param2 - param1 )/255) * pi + param1 
			//將每個PIXEL的RGB帶入函數中做轉換
			pix.r = ((param2-param1)*pix.r)/255+param1;
			pix.g = ((param2-param1)*pix.g)/255+param1;
			pix.b = ((param2-param1)*pix.b)/255+param1;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}


// ######  ######       ######
// #       # 		     #
// #       #		     #
// ######  ######	######
// #       #		#
// #	   #		#
// ######  #		######

int effect_2(ImageData *img,ImageData *outimg)
{
	//參數自動補正(直方圖伸長)
	int x,y;
	int i;
	int val;
	int pval;
	Pixel pix;
	int param1,param2;

    param1=300;
    param2=-300;
    	//找出最暗 跟最亮的 PIXEL
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
			//P0 =   255*( (Pi-param1)/(param2-param1) )
			val = getPixel(img,x,y,&pix);	
			pix.r = (255*(pix.r-param1))/(param2-param1);
			pix.g = (255*(pix.g-param1))/(param2-param1);
			pix.b = (255*(pix.b-param1))/(param2-param1);
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 0;
}

// ######  ######       ######
// #       # 		     #
// #       #		     #
// ######  ######	######
// #       #		     #
// #	   #		     #
// ######  #		######

int effect_3(ImageData *img,ImageData *outimg,int gammaint)
{
	//伽瑪補正
	int x,y;
	int i;
	int val;
	double gamma;
	double a;
	int color_tbl[256];//用來存補正後的亮度
	Pixel pix;
	
	//轉換成正確的伽瑪值
	gamma=(double)gammaint/100.0;
	
	//建立起色彩的亮度的陣列 提高效率減少負荷
	for(i=0;i<256;i++) {
		//P0 = M((Pi/M)^r),
		// M = 255, r = gamma, pi = a
		a=(double)i/255.0;
		color_tbl[i]=(int)(pow(a,gamma)*255.0);
	}
	//開始亮度轉,換根據現在亮度去找陣列中的值
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
// ######  ######       #    #
// #       # 		#    #
// #       #		#    #
// ######  ######	######
// #       #		     #
// #	   #		     #
// ######  #		     #
int effect_4(ImageData *img,ImageData *outimg,int gammaint)
{
	//伽瑪補正
	int x,y;
	int i;
	int val;
	double gamma;
	double a;
	int color_tbl[256];//用來存補正後的亮度
	Pixel pix;
	
	//轉換成正確的伽瑪值
	gamma=(double)gammaint/100.0;

	//建立起色彩的亮度的陣列 提高效率減少負荷
	for(i=0;i<256;i++) {
		//sigmoid 函數的應用
		//f(x) = 1 /( 1 + e^(-x) )
		//x = a 
		//最後在乘上255當作亮度
		a=(double)(i-128)/(255.0);
		a*=gamma;
		color_tbl[i]=(int) (255.0/(1.0+exp(-a)) );
	}
	//開始亮度轉,換根據現在亮度去找陣列中的值
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
// ######  ######       ######	
// #       # 		#	 
// #       #		#	 
// ######  ######	######
// #       #		     #
// #	   #		     #
// ######  #		######
int getMinMax(int *hist, int *min, int *max, int *pixs_min, int *pixs_max, int ave)
{
	//找最大亮度跟最小亮度並作分配
	int i;
	int rest;
	int now;
	int pixels;
	int a,b;

	rest=0;
	now=0;
	
	for(i=0;i<256;i++) {
		//取得PIXEL數量
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
	//最大值為255超過還是255
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
	//直方圖均等化處理
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

	//初始化陣列
	for(i=0;i<256;i++) {
		histR[i]=histG[i]=histB[i]=0;
	}
	//這張圖片每個亮度平均要有幾個PIXEL數
	ave= img->height * img->width /256;  

	//參考圖片中的每個PIXEL
	//並記錄每個色彩的亮度出現次數
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
	//找出R G B 分別的最大亮度跟最小亮度並作分配
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
// #       #            #	 
// #       #            #	 
// ######  ######       ######
// #       #            #    #
// #	   #            #    #
// ######  #            ######
void setMatrix(double *mat,int idx,double x)
{
	mat[idx  ]=x*x*x;
	mat[idx+1]=x*x;
	mat[idx+2]=x;
	mat[idx+3]=1.0;
}

void PFree(void *adr)
{
	if(adr==NULL) return;
	free(adr);
}

double lu(int n,double *a,int *ip)
{
	int i,j,k,ii,ik;
	double t,u,det;
	double *weight;

	weight=(double*)malloc(sizeof(double)*n);
	det=0;
	for(k=0;k<n;k++) {
		ip[k]=k;
		u=0;
		for(j=0;j<n;j++) {
			t=fabs(a[k*n+j]);
			if(t>u) u=t;
		}
		if(u==0) goto EXIT;
		weight[k]=1/u;
	}
	det=1;
	for(k=0;k<n;k++) {
		u=-1;
		for(i=k;i<n;i++) {
			ii=ip[i];
			t=fabs(a[ii*n+k])*weight[ii];
			if(t>u) {
				u=t;	j=i;
			}
		}
		ik=ip[j];
		if(j!=k) {
			ip[j]=ip[k];
			ip[k]=ik;
			det= -det;
		}
		u=a[ik*n+k];
		det*=u;
		if(u==0) goto EXIT;
		for(i=k+1;i<n;i++) {
			ii=ip[i];
			t=(a[ii*n+k]/=u);
			for(j=k+1;j<n;j++) {
				a[ii*n+j]-= t*a[ik*n+j];
			}
		}
	}
EXIT:
	PFree(weight);
	return det;
}

void solve(int n,double *a,double *b,int *ip,double *x)
{
	int i,j,ii;
	double t;

	for(i=0;i<n;i++) {
		ii=ip[i];	t=b[ii];
		for(j=0;j<i;j++) {
			t -= a[ii*n+j]*x[j];
		}
		x[i]=t;
	}
	for(i=n-1;i>=0;i--) {
		t=x[i];	ii=ip[i];
		for(j=i+1;j<n;j++) {
			t -= a[ii*n+j]*x[j];
		}
		x[i]=t/a[ii*n+i];
	}
}

int solveSystemOfLinearEquations(int n,double *mat,double *vec,double *ans)
{
	double det;
	double *mat2;
	int *ip;
	int i,j,nn;
	char bbb[256],sbuf[256];

	ip=(int*)malloc(sizeof(int)*n);
	if(ip==NULL) return -1;
	nn=n*n;
	mat2=(double*)malloc(sizeof(double)*nn);
	if(mat2==NULL) {
		PFree(ip);
		return -1;
	}
	for(i=0;i<nn;i++) mat2[i]=mat[i];
	det=lu(n,mat2,ip);
	if(det==0) return -2;
	solve(n,mat2,vec,ip,ans);
	PFree(ip);
	PFree(mat2);
	return 1;
}
int effect_6(ImageData *img,ImageData *outimg,int i1,int o1,int i2,int o2)
{	//依多項式進行色調變換處理的程式
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
// #       #            #    #
// #       #            #    #	 
// ######  ######           #
// #       #               #
// #	   #              #
// ######  #             #
int effect_7(ImageData *img,ImageData *out)
{	
	//負片化處理
	int x,y;
	Pixel pix,pix2;

	for(y=0;y<img->height;y++) {
		for(x=0;x<img->width;x++) {
			getPixel(img,x,y,&pix);
			//取亮度的相反值
			pix2.r = 255 - pix.r;
			pix2.g = 255 - pix.g;
			pix2.b = 255 - pix.b;
			setPixel(out,x,y,&pix2);	
		}
	}
	return 0;
}
// ######  ######           ######	
// #       #                #    #
// #       #                #    #
// ######  ######	    ######
// #       #                #    #
// #	   #                #    #
// ######  #                ######
void makeColorTable(int *tbl,int wave)
{
	//過度曝光
	int i,j;
	int val;
	int mts;
	int center;

	//權重分配
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
	//過度曝光
	int x,y;
	int val;
	int sora_tbl[COLORS];
	Pixel pix,pix2;
	//建立亮度陣列減低系統負擔 並加快執行速率
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

// ######  ######       ######	
// #       #            #    #
// #       #            #    #
// ######  ######       ######
// #       #                 #
// #	   #                 #
// ######  #            ######

int effect_9(ImageData *img,ImageData *outimg,int ef)
{
	//單純平滑化
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
				//計算每種顏色的各種亮度出現次數
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

// ######  ######        #	######	
// #       # 	         #	#    #
// #       #             #	#    #
// ######  ######        #	#    #
// #       #		 #	#    #
// #	   #		 #	#    #
// ######  #		 #	######

int effect_10(ImageData *img,ImageData *outimg)
{
	//加權平均平滑化
	//平滑矩陣
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
					//對應陣列乘上值
	    			val = getPixel(img,x+xx,y+yy,&pix);	
					rr += pix.r * fil[ff];
					gg += pix.g * fil[ff];
					bb += pix.b * fil[ff];
					ff++;
				}
			}
			// 乘完矩陣在除以平均值
			pix.r=rr/12;
			pix.g=gg/12;
			pix.b=bb/12;
			setPixel(outimg,x,y,&pix);	
		}
	}
	return 1;
}
// ######  ######        #      #	
// #       # 	         #	#
// #       #             #	#
// ######  ######        #	#
// #       #             #	#
// #	   #             #	#
// ######  #             #	#
int effect_11(ImageData *img,ImageData *outimg)
{
	//邊緣抽出
	//此陣列有邊緣方向性
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
					//對應陣列乘上值
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
// ######  ######       #     ######	
// #       # 	        # 	   #
// #       #		#	   #
// ######  ######       #     ######
// #       #		#     #
// #	   #		#     #
// ######  #		#     ######

int effect_12(ImageData *img,ImageData *outimg)
{

	//尖銳化
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
					//對應陣列乘上值
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

// ######  ######       #   ######	
// #       # 	        #        #
// #       #		#        #
// ######  ######       #   ######
// #       #		#        #
// #	   #		#        #
// ######  #		#   ######

int effect_13(ImageData *img,ImageData *outimg)
{
	//浮雕處理
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
					//對應陣列乘上值
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
int main(int ac,char *av[])
{
	
	char inputFile[100];
	char outputFile[100];
	ImageData *img,*outimg;
	int i,k,res,p1,p2,p3,p4,which;
	which = 0;	
	while(1){
		for( i = 0; i < 15; i++){
			for (k = 0; k < 15; ++k)
			{
				if(k%14 == 0|| i%14==0){
					printf("+");
				}
				else if(k%14 == 1){
					printf("NO.%2d  EF%2d ",i,i);
				}
				else if(k%14>12)
					printf(" ");
			}
			printf("\n");
		}
		printf("\n");

		while(!which)
			scanf("%d",&which);
		
		printf("enter inputFile:\t");
		scanf("%s",inputFile);
		
		res=readBMPfile(inputFile,&img);
		if(res<0) {
			printf("	can't read this file\n");
			return -1;
		}

		printf("enter outputFile:\t");
		scanf("%s",outputFile);
		outimg=createImage(img->width,img->height,24);
		
		switch (which){
			case 1: 
				printf("enter 2 param\n");
				scanf("%d,%d",&p1,&p2);
				effect_1(img,outimg,p1,p2);
				break;
			case 2:
				effect_2(img,outimg);
				break;
			case 3:
				printf("enter gamma val\n");
				scanf("%d",&p1);
				effect_3(img,outimg,p1);
				break;
			case 4:
				printf("enter gamma val\n");
				scanf("%d",&p1);
				effect_4(img,outimg,p1);			
				break;
			case 5:
				effect_5(img,outimg);
				break;
			case 6:
				printf("enter i1,o1,i2,o2\n");
				scanf("%d%d%d%d",&p1,&p2,&p3,&p4);
				effect_6(img,outimg,p1,p2,p3,p4);
				break;			
			case 7:
				effect_7(img,outimg);
				break;
			case 8:
				printf("enter wave \n");
				scanf("%d",&p1);
				effect_8(img,outimg,p1);
				break;
			case 9:
				printf("enter ef\n");
				scanf("%d",&p1);
				effect_9(img,outimg,p1);
				break;
			case 10:
				effect_10(img,outimg);
				break;
			case 11:
				effect_11(img,outimg);
				break;
			case 12:
				effect_12(img,outimg);
				break;
			case 13:
				effect_13(img,outimg);
				break;																		
		}

		writeBMPfile(outputFile,outimg);
		disposeImage(img);
		disposeImage(outimg);
		which=0;
	}	
	return 0;
}