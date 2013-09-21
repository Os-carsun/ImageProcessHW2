#include "image.h"

ImageData* createImage(int width,int height,int depth)
{
//建立一張新的圖片
  ImageData *newimg;//用來存取新圖片的結構指標
  int byte_per_pixel;//存每個pixel有多少BYTE

  if(width<0 || height<0) return NULL;//若長寬為負數則不做事(回傳NULL
  if(depth!=8 && depth!=24) return NULL;//只能是灰階或全彩
  
  newimg=(ImageData*)malloc(sizeof(ImageData));//分配一段空間來存圖片(指標的空間
  if(newimg==NULL) return NULL;//若分配空間失敗回傳NULL

  byte_per_pixel=depth/8;//根據depth來分析每個pixel的BYTE大小(全彩為3,灰階為2

  newimg->pixels=malloc(sizeof(BYTE)*byte_per_pixel*width*height);//給予圖片真正的空間(結構內指標必須單獨在malloc一次
																  //大小為 每個pixel的byte數 * 寬 * 長 的 byte 
  if(newimg->pixels==NULL) {//確定newimg->pixels有被分配空間
    free(newimg);
    return NULL;
  }

  newimg->width=width;  //存下圖片的寬度
  newimg->height=height;//存下圖片的長度
  newimg->depth=depth;  //存下depth
  return newimg;//把指向圖片的指標回傳
}


void disposeImage(ImageData *img)
{
  if(img->pixels!=NULL) //先把img->pixels 的記憶體位置還回去
	  free(img->pixels);
  free(img);//再把img的記憶體位置還回去
  //順序不可逆,不然會memory leakage
  return;
}


int getPixel(ImageData *img,int x,int y,Pixel *pix)
{
  //讀取圖片的一個Pixel(第(x,y)個
  int ret=1;
  int adr;  
  int dep,val;
  BYTE *pixels;

  if(img==NULL) return -1;//沒圖片就回傳-1
  if(img->pixels==NULL) return -1;//圖片內沒pixel回傳-1

  if(x<0) {//X小於0X等於0
    x=0;
    ret=0;
  }
  if(x >= img->width ) {//X 超過圖片的寬度X變為圖片寬度-1
    x=img->width -1;
    ret=0;
  }
  if(y<0) {//Y小於0Y等於0
    y=0;
    ret=0;
  }
  if(y >= img->height ) {//X 超過圖片的長度X變為圖長度-1
    y=img->height -1;
    ret=0;
  }
  dep=img->depth;//得到depth (全彩或灰階
  adr=x + y*img->width;//計算當前Pixel的memory address
  pixels=(BYTE*)img->pixels;//取得該Pixel
  if(dep==8) {//灰階情況  
    val=pixels[adr];
    pix->r=val;//紅色的值
    pix->g=val;//綠色的值
    pix->b=val;//藍色的值
  }
  else if(dep==24) {//全彩情況
    pixels+=(adr*3);
    pix->r=(*pixels);//紅色的值
    pixels++;
    pix->g=(*pixels);//綠色的值
    pixels++;
    pix->b=(*pixels);//藍色的值
  }
  else {
    return -1;
  }
  return ret;
}


int correctValue(int val,int max)
{
  if(val<0) return 0;
  if(val>max) return max;
  return val;
}


int setPixel(ImageData *img,int x,int y,Pixel *pix)
{//將Pixel放到圖片裡面
  int adr;  
  int dep;
  int val;
  BYTE *pixels;

  if(img==NULL) return -1;//沒圖片就回傳-1
  if(img->pixels==NULL) return -1;//圖片內沒pixel回傳-1

  if(x<0 || x >= img->width || y<0 || y >= img->height ) {
    return 0;
  }
  dep=img->depth;
  adr=x + y*img->width;
  pixels=(BYTE*)img->pixels;
  if(dep==8) {//灰階情況
    pixels[adr]=correctValue(pix->r,PIXELMAX);
  }
  else if(dep==24) {//全彩情況

    pixels+=(adr*3);
    (*pixels)=correctValue(pix->r,PIXELMAX);
    pixels++;
    (*pixels)=correctValue(pix->g,PIXELMAX);
    pixels++;
    (*pixels)=correctValue(pix->b,PIXELMAX);
  }
  else {
    return -1;
  }
  return 1;
}

int fwriteWORD(WORD val,FILE *fp)
{	//寫入一個 WORD（2BYTE）大小的值
	int i,c;

	c=val;
	for(i=0;i<2;i++) {
		fputc(c%256,fp);//256是因為1個BYTE = 8個BIT = 256
		c/=256;
	}
	return TRUE;
}


int fwriteDWORD(DWORD val,FILE *fp)
{	//寫入一個 DOUBLE WORD（4BYTE）大小的值
	int i,c;

	c=val;
	for(i=0;i<4;i++) {
		fputc(c%256,fp);//256是因為1個BYTE = 8個BIT = 256
		c/=256;
	}
	return TRUE;
}


int freadWORD(WORD *res,FILE *fp)
{	//讀入一個 WORD（2BYTE）大小的值
	int i,c;
	int val[2];

	for(i=0;i<2;i++) {
		c=fgetc(fp);
		if(c==EOF) return FALSE;
		val[i]=c;
	}
	*res=val[1]*256+val[0];//256是因為1個BYTE = 8個BIT = 256
	return TRUE;
}


int  freadDWORD(DWORD *res,FILE *fp)
{	//讀入一個 DOUBLE WORD（2BYTE）大小的值
	int i,c;
	int val[4];
	DWORD tmp=0;

	for(i=0;i<4;i++) {
		c=fgetc(fp);
		if(c==EOF) return FALSE;
		val[i]=c;
	}
	tmp=0;
	for(i=3;i>=0;i--) {
		tmp*=256;//256是因為1個BYTE = 8個BIT = 256
		tmp+=val[i];
	}
	*res=tmp;
	return TRUE;
}


static BOOL	IsWinDIB(BITMAPINFOHEADER* pBIH)
{	//確認這個Bmp(Bitmap檔)是不是DIB格式 是的話回傳TRUE 反之則為FALSE
	//一個完整的DIB由兩部分組成：一個BITMAPINFO結構和一個存儲像素陣列的數組
	if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER)) {
		return FALSE;
	}
	return TRUE;
}


int countOfDIBColorEntries(int iBitCount)
{
	int	iColors;
	//分辨顏色
	switch (iBitCount) {
	case 1://雙色 灰階
		iColors	= 2;
		break;
	case 4://彩色
		iColors	= 16;
		break;
	case 8://全彩
		iColors	= 256;
		break;
	default:
		iColors	= 0;
		break;
	}

	return iColors;
}


int getDIBxmax(int mx,int dep)
{
	switch(dep) {
	case 32:
		return mx*4;
	case 24:
		//return mx;
		return ((mx*3)+3)/4*4;
		break;
	case 16:
		return (mx+1)/2*2;
		break;
	case 8:
		return (mx+3)/4*4;
		break;
	case 4:
		return (((mx+1)/2)+3)/4*4;
		break;
	case 1:
		return (((mx+7)/8)+3)/4*4;
	}
	return mx;
}


int readBMPfile(char *fname,ImageData **img)
{	//讀進一張圖片存在img這指標裡
	int i,c;
	int errcode=0;
	BITMAPFILEHEADER BMPFile;
	int	fsize;
	BITMAPINFOHEADER BMPInfo;
	BITMAPCOREHEADER BMPCore;
	int	colors;
	int	colorTableSize;
	int	bitsSize;
	int	BISize;
	int x,y;
	int mx,my,depth;
	int pad;
	int mxb,myb;
	int isPM=FALSE;	
	FILE *fp;
	//DIB的結構
    WORD    HEAD_bfType;
    DWORD   HEAD_bfSize;
    WORD    HEAD_bfReserved1;
    WORD    HEAD_bfReserved2;
    DWORD   HEAD_bfOffBits;
    DWORD   INFO_bfSize;
    Pixel palet[MAXCOLORS];
    Pixel setcolor;

	if((fp=fopen(fname,"rb"))==NULL) {
		return -1;
	}


	if(!freadWORD(&HEAD_bfType,fp)) {//開始讀檔案類型如果沒讀到就到$ABORT
		errcode=-2;
		goto $ABORT;
	}
	if (HEAD_bfType != 0x4d42) {//0x4D42="BM" 是BITMAP的TYPE
		errcode=-10;				//用來確定是不是.bmp檔
		goto $ABORT;
	}

	if(!freadDWORD(&HEAD_bfSize,fp)) {//讀檔案大小
		errcode=-10;
		goto $ABORT;
	}

	if(!freadWORD(&HEAD_bfReserved1,fp)) {
		errcode=-10;
		goto $ABORT;
	}

	if(!freadWORD(&HEAD_bfReserved2,fp)) {
		errcode=-10;
		goto $ABORT;
	}

	if(!freadDWORD(&HEAD_bfOffBits,fp)) {
		errcode=-10;
		goto $ABORT;
	}

	if(!freadDWORD(&INFO_bfSize,fp)) {
		errcode=-10;
		goto $ABORT;
	}


	if (INFO_bfSize == 40/*sizeof(BITMAPINFOHEADER)*/ || INFO_bfSize == 12/*sizeof(BITMAPCOREHEADER)*/) {
		BMPInfo.biSize =	INFO_bfSize;

		if(INFO_bfSize == sizeof(BITMAPCOREHEADER)) {
			WORD tmp;
			isPM =	TRUE;

			if(!freadWORD(&tmp,fp)) {
				errcode=-10;
				goto $ABORT;
			}
			BMPInfo.biWidth=tmp;

			if(!freadWORD(&tmp,fp)) {
				errcode=-10;
				goto $ABORT;
			}
			BMPInfo.biHeight=tmp;

			if(!freadWORD(&(BMPInfo.biPlanes),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadWORD(&(BMPInfo.biBitCount),fp)) {
				errcode=-10;
				goto $ABORT;
			}
		}
		else {		

			if(!freadDWORD(&(BMPInfo.biWidth),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biHeight),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadWORD(&(BMPInfo.biPlanes),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadWORD(&(BMPInfo.biBitCount),fp)) {
				errcode=-10;
				goto $ABORT;
			}
		}

		if(!isPM) {

			if(!freadDWORD(&(BMPInfo.biCompression),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biSizeImage),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biXPelsPerMeter),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biYPelsPerMeter),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biClrUsed),fp)) {
				errcode=-10;
				goto $ABORT;
			}

			if(!freadDWORD(&(BMPInfo.biClrImportant),fp)) {
				errcode=-10;
				goto $ABORT;
			}
		}
	}
	else {
		errcode=-10;
		goto $ABORT;
	}
	mx=BMPInfo.biWidth;
	my=BMPInfo.biHeight;
	depth=BMPInfo.biBitCount;

	if(depth!=8 && depth!=24) {
		errcode=-3;
		goto $ABORT;
	}

	if(BMPInfo.biCompression!=BI_RGB) {
		errcode=-20;
		goto $ABORT;
	}

	if(BMPInfo.biClrUsed==0) {
		colors	= countOfDIBColorEntries(BMPInfo.biBitCount);
	}
	else {
		colors	= BMPInfo.biClrUsed;
	}


	if (!isPM)	{
		for(i=0;i<colors;i++) {

			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].b=c;

			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].g=c;

			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].r=c;

			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
		}
	} else {
		for(i=0;i<colors;i++) {
			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].b=c;
			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].g=c;
			c=fgetc(fp);
			if(c==EOF) {
				errcode=-10;
				goto $ABORT;
			}
			palet[i].r=c;
		}
	}

	*img=createImage(mx,my,24);
	mxb=getDIBxmax(mx,depth);
	pad=mxb-mx*depth/8;

	for(y=my-1;y>=0;y--) {
		for(x=0;x<mx;x++) {
			if(depth==8) {	
				c=fgetc(fp);
				if(c==EOF) {
					errcode=-20;
					goto $ABORT;
				}
				setcolor.r=palet[c].r;
				setcolor.g=palet[c].g;
				setcolor.b=palet[c].b;
			}
			else if(depth==24) {
				c=fgetc(fp);
				if(c==EOF) {
					errcode=-20;
					goto $ABORT;
				}
				setcolor.b=c;
				c=fgetc(fp);
				if(c==EOF) {
					errcode=-20;
					goto $ABORT;
				}
				setcolor.g=c;
				c=fgetc(fp);
				if(c==EOF) {
					errcode=-20;
					goto $ABORT;
				}
				setcolor.r=c;
			}
			setPixel(*img,x,y,&setcolor);
		}

		for(i=0;i<pad;i++) {
			c=fgetc(fp);
			if(c==EOF) {
				errcode=-20;
				goto $ABORT;
			}
		}
	}
$ABORT:	
	fclose(fp);
	return errcode;
}


int writeBMPfile(char *fname,ImageData *img)
{	//將一張已經把所以pixel存好的ImageData結構真的輸出成一張圖片
	FILE *fp;
	BITMAPFILEHEADER bfn;
	int w,h,rw;
	int mxb,pad;
	int depth;	
	int pbyte;	
	int palsize;	
	int x,y,i;
	int saveloop,saverest;
	int	iBytes;
	unsigned int wsize;
	Pixel pix;

	w=img->width;	//取得圖片寬
	h=img->height;	//取得圖片長
	depth=img->depth; //取的圖片的depeth(全彩灰階...


	if(depth!=24) {//
		//errcode=-3;
		goto $abort1;
	}

	if(depth==24) {
		pbyte=1;
	}
	else {
		pbyte=depth/8;
	}
	if(depth>=24) {
		palsize=0;
	}
	else {
		palsize=256;
	}
	rw=getDIBxmax(w,depth);

	bfn.bfType=0x4d42;	//'BM'
	bfn.bfSize=14+40+/*sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER) +*/
			   palsize*4/*sizeof(RGBQUAD)*/ +
			   rw*h*pbyte;
	bfn.bfReserved1=0;
	bfn.bfReserved2=0;
	bfn.bfOffBits=14+40/*sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER)*/ +
			      palsize*4/*sizeof(RGBQUAD)*/;

	if((fp=fopen(fname,"wb"))==NULL) {
		goto $abort1;
	}
	//寫入DIB的結構
	fwriteWORD(bfn.bfType,fp);
	fwriteDWORD(bfn.bfSize,fp);
	fwriteWORD(bfn.bfReserved1,fp);
	fwriteWORD(bfn.bfReserved2,fp);
	fwriteDWORD(bfn.bfOffBits,fp);
	fwriteDWORD(40/*sizeof(BITMAPINFOHEADER)*/,fp);	//biSize
	fwriteDWORD(w,fp);		// biWidth
	fwriteDWORD(h,fp);		// biHeight
	fwriteWORD(1,fp);		// biPlanes
	fwriteWORD(depth,fp);	// biBitCount
	fwriteDWORD(BI_RGB,fp);	// biCompression
	fwriteDWORD(0,fp);	// biSizeImage
	fwriteDWORD(300,fp);	// biXPelsPerMeter
	fwriteDWORD(300,fp);	// biYPelsPerMeter
	fwriteDWORD(0,fp);		// biClrUsed
	fwriteDWORD(0,fp);		// biClrImportant


	pad=rw-w*depth/8;

	for(y=h-1;y>=0;y--) {
		for(x=0;x<w;x++) {
			getPixel(img,x,y,&pix);
			fputc(pix.b,fp);
			fputc(pix.g,fp);
			fputc(pix.r,fp);
		}

		for(i=0;i<pad;i++) {
			fputc(0,fp);
		}
	}
	return 0;
$abort1:
	return 1;
$abort2:
	fclose(fp);
	return 1;
}
