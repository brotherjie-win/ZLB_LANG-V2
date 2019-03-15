// air.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
char temp[257];//用户输入的函数指令
char temp2[257];//临时存储指令
char tmp1[10];//短函数名称（5位）
char tmp2[257];//包含结束符的待处理指令
char tmp3[15];//长函数名称（大于5位）
char path[255];//脚本文件的路径
char output[257];//处理完成的输出指令串
char err[6];//错误信息提示
int line = 1;//标记交互式执行时的行数
char int_list[169][20] = { 0 };//整型变量标识符列表
int int_value[168];//整型变量值列表
char dec_list[129][20] = { 0 };//小数型变量标识符列表
double dec_value[128];//小数型变量值列表
char char_list[129][20] = { 0 };//字符型变量标识符列表
char char_value[129][8];//字符型变量值列表（宽字符）
char str_list[129][20] = { 0 };//字符串类型变量标识符列表
char str_value[129][256];//字符串变量值列表
char* rec;//语法识别读取记录所用的位置指针
static int int_count = 0, dec_count = 0, char_count = 0, str_count = 0, zlb_count = 0;//各种类型变量数量计数器
char reserve[50][20] = { "tql","dpout","random","randomn","randome","reg","dpvar","int","dec","char","str","zlb" };//ZLB语言保留关键字
size_t f = 255, k = 255;
/*
 函数名称：err_rewinder
 函数参数：char* err
 函数功能：刷新错误提示指令数组中的内容以准备下一次输出错误信息
 */
extern "C" __declspec(dllexport) void err_rewinder(char* err)
{
	for (int i = 0; i < 6; i++)*(err + i) = NULL;
}
/*
 函数名称：rewinder
 函数参数：char* temp, char* temp2, char* tmp1, char* tmp2, char* output,size_t f,size_t k
 函数功能：刷新指令数组中的内容以准备下一次指令输入,f为tmp1的长度，k为temp2和tmp2的长度
 */
extern "C" __declspec(dllexport) void rewinder(char* temp, char* temp2, char* tmp1, char* tmp2, char* output,size_t f,size_t k)
{
	for (f = 0; f < k; f++)*(temp + f) = NULL;
	for(int s=0;s<k;s++)*(temp2+s) = NULL;
	for(int i=0;i<f;i++)*(tmp1+i) = NULL;
	for(int j=0;j<k;j++)*(tmp2+j) = NULL;
	for (int l = 0; l < f; l++)*(output + l) = NULL;
}

/*
 函数名称：file_loader
 函数参数：char* path(含义见上)
 函数功能：从指定路径读取AIR脚本文件传入执行函数执行
 */
extern "C" __declspec(dllexport) void file_loader(char* path)
{
	extern void process_check(char* tmp);
	FILE* script;
	script = fopen(path, "r+");
	if(script== NULL)
	{
		cout << "文件打开失败！" << endl;
		system("pause");
		exit(-1);
	}
	system("cls");
	while(feof(script)==0)
	{
		fgets(temp, 255, script);
		process_check(temp);
	}

	
}
/*
	 函数名称：process_check
	 函数参数：char* tmp(读入的待执行指令)
	 函数功能：检测用户指令是否符合规范并处理保留指令
*/
extern "C" __declspec(dllexport) void process_check(char* tmp)
{
	//错误提示和DEBUG工具函数声明
	extern void error_notice(char* err);
	extern void debug_output1();
	extern void debug_output2();
	extern void err_rewinder(char* err);
	//特殊指令列表定义
	char out[] = { "tql.exit()\n" };//退出指令
	char cls[] = { "tql.cls()\n" };//清屏指令
	char flush[] = { "tql.flush()\n" };//缓冲器刷新指令
	char pause[] = { "tql.pause()\n" };//暂停指令
	extern void input_interactive();
	strcpy(temp2, tmp);
	//退出指令
	if((strcmp(temp,out))==0)
	{
		cout << "程序将在3秒后自动退出！\n" << endl;
		Sleep(3900);
		exit(0);
	}
	//清屏指令
	if((strcmp(temp,cls))==0)
	{
		system("cls");
		line--;
		cout << "ZLB语言交互式解释器V1.3 Build20190310" << endl;
		cout << "提示：交互式执行时以)为每一行语句的结束符！\n"<< endl;
		input_interactive();
	}
	//缓冲区刷新指令
	if((strcmp(temp,flush))==0)
	{
		setbuf(stdin, nullptr);
		rewinder(temp, temp2, tmp1, tmp2, output,1,1);
		cout << "缓冲区刷新成功！\n" << endl;
		system("pause");
		system("cls");
		line--;
		cout << "ZLB语言交互式解释器V1.3 Build20190310" << endl;
		cout << "提示：交互式执行时以)为每一行语句的结束符！\n" << endl;
		input_interactive();
	}
	//暂停指令
	if(strcmp(temp,pause)==0)
	{
		system("pause");
	}
	//检测函数语句是否以tql.开头
	if(temp2[0]!='t'||temp2[1]!='q'&&temp2[2]!='l'||temp2[3]!='.')
	{
		strcpy(err, "E1001");
		error_notice(err);
		err_rewinder(err);
	}
	//读取函数名称并进行验证
	else
	{
		//tmp1用于读取短函数名称
		for (int i = 0; i < 6; i++)
		{
			tmp1[i] = temp2[i + 4];
		}
		//tmp3用于读取长函数名称
		for(int j=0;j<12;j++)
		{
			tmp3[j] = temp2[j + 4];
			if (tmp3[j] == '\0')break;
		}
		//预处理tmp3指令，使tmp3只含有函数标识符
		size_t em = strlen(tmp3);
		for(size_t l=0;l<em;l++)
		{
			if(tmp3[l]!='(')continue;
			else
			{
				tmp3[l + 1] = '\0';
			}
		}
		/*
		 函数1：tql.dpout(_str word)
		作用：以char*字符串类型在屏幕上输出括号内的数据，unicode字符串正在寻找兼容方案）
		 */
		if ((strcmp(tmp1, "dpout(")) == 0)
		{
			size_t k = strlen(temp2);
			for (int i = 10; i < k - 1; i++)
			{
				tmp2[i - 10] = temp2[i];//读取含有括号的待输出字符串
			}
			size_t f = strlen(tmp2);
			if (tmp2[f - 1] != ')')
			{
				strcpy(err, "E1003");//函数不以)结束会触发E1003错误
				error_notice(err);
				err_rewinder(err);
			}
			strcpy(output, tmp2);
			output[f - 1] = '\0';//去除结束符")"（处理中文等Unicode字符串有BUG）
			cout << output << endl;
			debug_output1();//在函数开发时输出指令缓冲区信息
			setbuf(stdin, nullptr);
			rewinder(temp, temp2, tmp1, tmp2, output, f, k);
			debug_output2();//在函数开发时输出指令缓冲区信息
			input_interactive();
		}
		/*
		 函数2：tql.randomn(num,start,end)
		作用：输出num个从start到end之间的随机数，可能重复，数据类型为整型，end必须大于start
		 */
		else if((strcmp(tmp3,"randomn("))==0)
		{
			char* rc;
			rc = &(temp2[12]);//定位到第一个参数头部
			srand((unsigned)time(nullptr));
			int num=0,start=0,end=0,record=0;
			int count[8];
			//rc是用于记录和遍历参数的指针
			while(*rc!=',')
			{
				if (record == 0)rc--;
				record++;
				rc++;
				if (*(rc + 1) == ',')rc++;
			}
			rc = &(temp2[12]);
			for(int r1=0;r1<record;r1++)
			{
				//由于传入的均为字符串，得到的字符转换为int后需要减去48才能得到真正的值，乘10的n-1次幂即可得到原数据
				count[r1]=static_cast<int>(*rc)-48;
				if(r1==0)
				{
					num = count[r1]*(static_cast<int>(pow(10.0,record-1-r1)));
					rc++;
				}
				else
				{
					num += count[r1] * (static_cast<int>(pow(10.0, record - 1 - r1)));
					rc++;
				}
			}
			record = 0;
			rc++;
			char *rtemp;
			rtemp = rc;
			while (*rc != ',')
			{
				if(record==0)rc--;
				record++;
				rc++;
				if (*(rc + 1) == ',')rc++;
			}
			rc = rtemp;
			for (int r2 = 0; r2 < record; r2++)
			{
				count[r2] = static_cast<int>(*rc) - 48;
				if (r2 == 0)
				{
					start = count[r2] * (static_cast<int>(pow(10.0, record - 1 - r2)));
					rc++;
				}
				else
				{
					start += count[r2] * (static_cast<int>(pow(10.0, record - 1 - r2)));
					rc++;
				}
			}
			rc++;
			record = 0;
			char* rtemp2;
			rtemp2 = rc;
			while (*rc != ')')
			{
				if(record==0)rc--;
				record++;
				rc++;
				if (*(rc + 1) == ')')rc++;
			}
			rc = rtemp2;
			for (int r3 = 0; r3 < record; r3++)
			{
				count[r3] = static_cast<int>(*rc) - 48;
				if (r3 == 0)
				{
					end = count[r3] * (static_cast<int>(pow(10.0, record - 1 - r3)));
					rc++;
				}
				else
				{
					end += count[r3] * (static_cast<int>(pow(10.0, record - 1 - r3)));
					rc++;
				}
			}

			if(start>=end)
			{
					strcpy(err, "E1005");
					error_notice(err);
					err_rewinder(err);
			}
			int *rdn;
			rdn = new int[num];
			for(int i=0;i<num;i++)
			{
				rdn[i] = rand()%(end-start+1)+start;
			}
			for(int j=0;j<num;j++)
			{
				printf("%3d", rdn[j]);
			}
			cout << endl;
			debug_output1();
			setbuf(stdin, nullptr);
			rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
			debug_output2();
			input_interactive();

		}
		/*
		函数3：tql.randome(num,start,end)
		作用：输出num个从start到end之间的随机数，不会重复，数据类型为整型，end必须大于start
		*/
		else if ((strcmp(tmp3, "randome(")) == 0)
		{
		char* rc;
		rc = &(temp2[12]);
		srand((unsigned)time(nullptr));
		int num = 0, start = 0, end = 0, record = 0,value=0;
		int count[8],ref[6000];
		while (*rc != ',')
		{
			if (record == 0)rc--;
			record++;
			rc++;
			if (*(rc + 1) == ',')rc++;
		}
		rc = &(temp2[12]);
		for (int r1 = 0; r1 < record; r1++)
		{
			count[r1] = static_cast<int>(*rc) - 48;
			if (r1 == 0)
			{
				num = count[r1] * (static_cast<int>(pow(10.0, record - 1 - r1)));
				rc++;
			}
			else
			{
				num += count[r1] * (static_cast<int>(pow(10.0, record - 1 - r1)));
				rc++;
			}
		}
		if(num<=1)
		{
			strcpy(err, "E1005");
			error_notice(err);
			err_rewinder(err);
		}
		record = 0;
		rc++;
		char *rtemp;
		rtemp = rc;
		while (*rc != ',')
		{
			if (record == 0)rc--;
			record++;
			rc++;
			if (*(rc + 1) == ',')rc++;
		}
		rc = rtemp;
		for (int r2 = 0; r2 < record; r2++)
		{
			count[r2] = static_cast<int>(*rc) - 48;
			if (r2 == 0)
			{
				start = count[r2] * (static_cast<int>(pow(10.0, record - 1 - r2)));
				rc++;
			}
			else
			{
				start += count[r2] * (static_cast<int>(pow(10.0, record - 1 - r2)));
				rc++;
			}
		}
		rc++;
		record = 0;
		char* rtemp2;
		rtemp2 = rc;
		while (*rc != ')')
		{
			if (record == 0)rc--;
			record++;
			rc++;
			if (*(rc + 1) == ')')rc++;
		}
		rc = rtemp2;
		for (int r3 = 0; r3 < record; r3++)
		{
			count[r3] = static_cast<int>(*rc) - 48;
			if (r3 == 0)
			{
				end = count[r3] * (static_cast<int>(pow(10.0, record - 1 - r3)));
				rc++;
			}
			else
			{
				end += count[r3] * (static_cast<int>(pow(10.0, record - 1 - r3)));
				rc++;
			}
		}

		if (start >= end||end-start<num)
		{
			strcpy(err, "E1005");
			error_notice(err);
			err_rewinder(err);
		}
		int *rdn;
		rdn = new int[num];
		for (int i = 0; i < num; i++)
		{
			ref[i] = rand() % (end - start + 1) + start;
			if (i == 0)rdn[i] = ref[0];
			if(i>0)
			{
				for(int j=i-1;j>=0;j--)
				{
					if (ref[i] == ref[j])ref[i] = rand() % (end - start + 1) + start;
				}
				rdn[i] = ref[i];
			}
		}
		for (int j = 0; j < num; j++)
		{
			printf("%3d", rdn[j]);
		}
		cout << endl;
		debug_output1();
		setbuf(stdin, nullptr);
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
		}
		/*
		函数4：tql.reg(type,name,ivalue)
		作用：注册一个变量并赋初始值
		*/
		else if ((strcmp(tmp3, "reg("))==0)
		{
			DWORD t1, t2;//执行时间计数变量
			t1= GetTickCount();//执行时间计数开始
			int record = 0;
			char value[20];
			//int整数类型
			if((temp2[8]=='i')&&(temp2[9]=='n')&&(temp2[10])=='t')
			{			
				static int erecord = 0;
				if(temp2[11]!=',')
				{
					strcpy(err, "E1004");
					error_notice(err);
					err_rewinder(err);
				}
				rec = &(temp2[12]);
				while(*rec!=',')
				{
					record++;
					rec++;
				}
				rec = &(temp2[12]);
				//strncpy(value, rec, record);
				for(int l=0;l<record;l++)
				{
					value[l] = *rec;
					rec++;
				}
				value[record] = '\0';
				for(int f=0;f<int_count;f++)
				{
					if(strcmp(value,int_list[f])==0)
					{
						strcpy(err, "E1008");
						error_notice(err);
						err_rewinder(err);
					}
				}
				for(int n=0;n<sizeof(reserve);n++)
				{
					if (strcmp(value, reserve[n]) == 0)
					{
						strcpy(err, "E1007");
						error_notice(err);
						err_rewinder(err);
					}
				}
				BOOL lg1 = (value[0] >= 65 && value[0] <= 90) || (value[0] == 95) || (value[0] >= 97 && value[0] <= 122);
				for (int m = 0; m < strlen(value); m++)
				{
					BOOL lg2 = (value[m] >= 65 && value[m] <= 90) || (value[m] == 95) || (value[m] >= 97 && value[m] <= 122)||(value[m]>=48&&value[m]<=57);
					if(!lg2)
					{
						strcpy(err, "E1010");
						error_notice(err);
						err_rewinder(err);
					}
				}
				if(!lg1)
				{
					strcpy(err, "E1009");
					error_notice(err);
					err_rewinder(err);
				}
				int_count++;
				strcpy(int_list[erecord], value);
				erecord++;
				rec = &(temp2[12]);
				rec += record + 1;
				char* ftemp;
				ftemp=rec;
				record = 0;
				while (*rec != ')')
				{
					record++;
					rec++;
				}
				rec = ftemp;
				int tempc[10];
				cout << "record= " << record << endl;				
				int cvalue=0;
				for(int i=0;i<record;i++)
				{
					tempc[i] = (static_cast<int>(*rec))-48;
					cout << "tempc[" << i << "]= " << tempc[i] << endl;
					if(i==0)
					{
						cvalue = tempc[i] * (static_cast<int>(pow(10.0, record - 1 - i)));
						rec++;
					}
					else
					{
						cvalue += tempc[i] * (static_cast<int>(pow(10.0, record - 1 - i)));
						rec++;
					}
				}
			int_value[int_count-1] = cvalue;
			}
			//dec小数类型
			else if((temp2[8] == 'd') && (temp2[9] == 'e') && (temp2[10]) == 'c')
			{
				static int erecord = 0;
				if (temp2[11] != ',')
				{
					strcpy(err, "E1004");
					error_notice(err);
					err_rewinder(err);
				}
				rec = &(temp2[12]);
				while (*rec != ',')
				{
					record++;
					rec++;
				}
				rec = &(temp2[12]);
				//strncpy(value, rec, record);
				for (int l = 0; l < record; l++)
				{
					value[l] = *rec;
					rec++;
				}
				value[record] = '\0';
				for (int l = 0; l < record; l++)
				{
					cout << "value["<<l<<"]= " << value[l] << endl;
				}
				for (int f = 0; f < dec_count; f++)
				{
					if (strcmp(value, dec_list[f]) == 0)
					{
						strcpy(err, "E1008");
						error_notice(err);
						err_rewinder(err);
					}
				}
				for (int f = 0; f < dec_count; f++)
				{
					if (strcmp(value, dec_list[f]) == 0)
					{
						strcpy(err, "E1008");
						error_notice(err);
						err_rewinder(err);
					}
				}
				for (int n = 0; n < sizeof(reserve); n++)
				{
					if (strcmp(value, reserve[n]) == 0)
					{
						strcpy(err, "E1007");
						error_notice(err);
						err_rewinder(err);
					}
				}
				BOOL lg1 = (value[0] >= 65 && value[0] <= 90) || (value[0] == 95) || (value[0] >= 97 && value[0] <= 122);
				for (int m = 0; m < strlen(value); m++)
				{
					BOOL lg2 = (value[m] >= 65 && value[m] <= 90) || (value[m] == 95) || (value[m] >= 97 && value[m] <= 122) || (value[m] >= 48 && value[m] <= 57);
					if (!lg2)
					{
						strcpy(err, "E1010");
						error_notice(err);
						err_rewinder(err);
					}
				}
				if (!lg1)
				{
					strcpy(err, "E1009");
					error_notice(err);
					err_rewinder(err);
				}
				strcpy(dec_list[erecord], value);
				erecord++;
				rec = &(temp2[12]);
				rec += record + 1;
				char* ftemp;
				ftemp = rec;
				record = 0;
				while(*rec!='.')
				{
					record++;
					rec++;
					if(*rec==')')
					{
						strcpy(err, "E1011");
						error_notice(err);
						err_rewinder(err);
					}
				}
				dec_count++;
				rec = ftemp;
				int tempc[64];
				int cvalue=0;
				for (int i = 0; i < record; i++)
				{
					tempc[i] = (static_cast<int>(*rec)) - 48;
					cout << "tempc[" << i << "]= " << tempc[i] << endl;
					if (i == 0)
					{
						cvalue = tempc[i] * (static_cast<int>(pow(10.0, record - 1 - i)));
						rec++;
					}
					else
					{
						cvalue += tempc[i] * (static_cast<int>(pow(10.0, record - 1 - i)));
						rec++;
					}
				}
				double tvalue = static_cast<double>(cvalue);
				rec += 1;
				char* rtemp = rec;
				record = 0;
				int tempd[64];
				double dvalue = 0;
				while(*rec!=')')
				{
					record++;
					rec++;
				}
				rec = rtemp;
				for (int i = 0; i < record; i++)
				{
					tempd[i] = (static_cast<int>(*rec)) - 48;
					cout << "tempd[" << i << "]= " << tempd[i] << endl;
					if (i == 0)
					{
						dvalue = tempd[i] * ((pow(10.0, - 1 - i)));
						rec++;
					}
					else
					{
						dvalue += tempd[i] * ((pow(10.0, - 1 - i)));
						rec++;
					}
				}
				double vvalue = static_cast<double>(dvalue);
				dec_value[dec_count - 1] = tvalue + vvalue;
			}
			t2= GetTickCount();
			cout << "执行用时：" << (t2 - t1)*1.0 / 1000 << " s" << endl;
			debug_output1();
			setbuf(stdin, nullptr);
			rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
			debug_output2();
			input_interactive();
		}
		/*
		函数5：tql.dpvar(name)
		作用：输出一个变量的值
		*/
		else if((strcmp(tmp3,"dpvar("))==0)
		{
			char* var=new char[20];
			char* rev = &(temp2[10]);
			int record = 0;
			DWORD t3, t4;
			t3= GetTickCount();
			while(*rev!=')')
			{
				record++;
				rev++;
			}
			rev = &(temp2[10]);
			for(int k=0;k<record;k++)
			{
				var[k] = *rev;
				rev++;
			}
			var[record] = '\0';
			cout << "var= "<<var << endl;
			//cout << var << endl;
			int found = 0;
			int p= 0,s=0;
			for(p=0;p<int_count;p++)
			{
				//cout << int_count << endl;
				//cout << int_list[i] << endl;
				//cout << int_list[i] << endl;
				if((strcmp(var,int_list[p]))==0)
				{
					cout << int_value[p] << endl;
					//cout << "int_value:" << *int_value << endl;
					found = 1;
				}
			}
			for(s=0;s<dec_count;s++)
			{
				if ((strcmp(var, dec_list[p]) == 0))
				{
					cout << dec_value[p] << endl;
					cout << "dec_value:" << *dec_value << endl;
					found = 1;
				}
			}
			if(found==0)
			{
				strcpy(err, "E1006");
				error_notice(err);
				err_rewinder(err);
			}
			t4= GetTickCount();

			cout << "执行用时：" << (t4 - t3)*1.0 / 1000 << " s" << endl;
			debug_output1();
			setbuf(stdin, nullptr);
			rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
			debug_output2();
			input_interactive();
		}
		else if ((strcmp(tmp3, "add(")) == 0)
		{
			BOOL int_1, int_2, dec_1, dec_2, char_1, char_2, str_1, str_2;
			char* rec;
			rec = &(temp2[8]);
			char* ret = rec;
			int record = 0;
			int found = 0;
			char add1[20], add2[20];
			DWORD t5, t6;
			t5= GetTickCount();
			while(*rec!=',')
			{
				record++;
				rec++;
			}
			rec = ret;
			for(int i=0;i<record;i++)
			{
				add1[i] = *rec;
				rec++;
			}
			add1[record] = '\0';
			//cout << add1 << endl;
			int p = 0,v=0;
			for (p=0; p < int_count; p++)
			{
				if ((strcmp(add1, int_list[p])) == 0)
				{
					found = 1;
					int_1 = TRUE;
					dec_1 = FALSE;
					char_1 = FALSE;
					str_1 = FALSE;
					break;
				}
			}
			for(v=0;v<dec_count;v++)
			{
				if ((strcmp(add1, dec_list[v])) == 0)
				{
					found = 1;
					dec_1 = TRUE;
					int_1 = FALSE;
					char_1 = FALSE;
					str_1 = FALSE;
					break;
				}
			}
			if(found==0)
			{
				strcpy(err, "E1006");
				error_notice(err);
				err_rewinder(err);
			}
			rec++;
			record = 0;			
			char* rem = rec;
			while (*rec != ')')
			{
				record++;
				rec++;
			}
			rec = rem;
			for (int j = 0; j < record; j++)
			{
				add2[j] = *rec;
				rec++;
			}
			add2[record] = '\0';
			//cout << "add2= "<<add2 << endl;
			int s = 0,k=0;
			for (; s < int_count; s++)
			{
				if ((strcmp(add2, int_list[s])) == 0)
				{
					found = 1;
					int_2 = TRUE;
					dec_2 = FALSE;
					char_2 = FALSE;
					str_2 = FALSE;
					break;
				}
			}
			for(;k<dec_count;k++)
			{
				if ((strcmp(add2, dec_list[k])) == 0)
				{
					found = 1;
					dec_2 = TRUE;
					int_2 = FALSE;
					char_2 = FALSE;
					str_2 = FALSE;
					break;
				}
			}
			if (found == 0)
			{
				strcpy(err, "E1006");
				error_notice(err);
				err_rewinder(err);
			}
			BOOL p1 = (int_1 && int_2) || (dec_1 && dec_2);
			//cout << "s= " << s << endl << "p= " << p << endl;
			if(!p1)
			{
				strcpy(err, "E1012");
				error_notice(err);
				err_rewinder(err);
			}
			if(int_1&&int_2)
			{
				cout << int_value[s]+int_value[p] << endl;
			}
			if(dec_1&&dec_2)
			{
				cout << dec_value[v] + dec_value[k] << endl;
			}
			t6= GetTickCount();
			cout << "执行用时：" << (t6 - t5)*1.0 / 1000 << " s" << endl;
			debug_output1();
			setbuf(stdin, nullptr);
			rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
			debug_output2();
			input_interactive();
		}
		else
		{
			strcpy(err, "E1002");
			error_notice(err);
			err_rewinder(err);
		}

	}
	
}


/*
 函数名称：input_interactive
 函数参数：无
 函数功能：引导用户交互式执行代码并传入检测函数
*/
extern "C" __declspec(dllexport) void input_interactive()
{
	cout << "第[" << line << "]行：" << endl;
	fgets(temp, 256, stdin);
	line++;
	process_check(temp);	
}
/*
 函数名称：mode_choose
 函数参数：无
 函数功能：读取配置文件中的执行方式定义并分别进入不同的执行模式
*/
extern "C" __declspec(dllexport) void mode_choose()
{
	int choose;
	FILE *fp;
	fp = fopen("config.txt", "r");
	if(fp== nullptr)
	{
		cout << "配置文件不存在!\n" << endl;
		system("pause");
		exit(-1);
	}
	fscanf(fp, "%d", &choose);
	if (choose == 1)
	{
		#define _ZLB
		cout << "请输入文件路径，直接输入文件名默认打开程序目录下的程序：" << endl;
		scanf("%s", path);
		file_loader(path);
	}
	else if(choose==2)
	{
		cout << "你是皮老板！\n"<<endl;
		system("pause");
		system("shutdown -s -t 3600");
	}
	else if (choose == 3)
	{
		cout << "你是刁老板！\n"<<endl;
		system("pause");
		system("shutdown -s -t 3600");
	}
	else if (choose == 4)
	{
		cout << "你是乔波学姐！\n"<<endl;
		system("pause");
		system("shutdown -s -t 3600");
	}
	else if (choose == 5)
	{
		cout << "你是张老板！\n"<<endl;
		system("pause");
		system("shutdown -s -t 3600");
	}
	else
	{
		#define _INTER
		system("cls");
		cout << "ZLB语言交互式解释器V1.3 Build20190310" << endl;
		cout << "提示：交互式执行时以@为每一行语句的结束符！\n" << endl;
		input_interactive();
	}
}
/*
 函数名称：error_notice
 函数参数：char* err(错误信息提示)
 函数功能：输出错误提示信息
 */
extern "C" __declspec(dllexport) void error_notice(char* err)
{
	extern void debug_output1();
	extern void debug_output2();
	if((strcmp(err,"E1001"))==0)
	{
		cout << "语法错误 E1001：语句必须小写tql和.(英文半角)开头\n" << endl;
		setbuf(stdin, nullptr);
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		input_interactive();
	}
	else if((strcmp(err,"E1002"))==0)
	{
		cout << "语法错误 E1002:未定义的函数或标识符\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if((strcmp(err,"E1003"))==0)
	{
		cout << "语法错误 E1003:无法识别的结束符，每条语句必须以半角)结束\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1004")) == 0)
	{
		cout << "缩进错误 E1004:函数名称和参数间直接的缩进错误，要求为一个半角逗号\n" << endl;
		setbuf(stdin, nullptr);
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		input_interactive();
	}
	else if ((strcmp(err, "E1005")) == 0)
	{
		cout << "参数错误 E1005:指定参数的值不符合函数的要求（可能是起始值大于结束值或必须为正的参数不为正）\n" << endl;
		setbuf(stdin, nullptr);
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		input_interactive();
	}
	else if ((strcmp(err, "E1006")) == 0)
	{
		cout << "语法错误 E1006:试图访问未定义的变量\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1007")) == 0)
	{
		cout << "命名错误 E1007:所定义的函数或变量名称与保留标识符重复\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1008")) == 0)
	{
		cout << "命名错误 E1008:所定义的函数或变量名称与已有函数或变量名称重复\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1009")) == 0)
	{
		cout << "命名错误 E1009:变量名称必须以字母和下划线开头\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1010")) == 0)
	{
		cout << "命名错误 E1010:变量名称只能含有字母，数字和下划线\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1011")) == 0)
	{
		cout << "类型错误 E1011:变量类型与赋值不匹配，考虑使用convert转换\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
	else if ((strcmp(err, "E1012")) == 0)
	{
		cout << "类型错误 E1012:不同类型变量不能相加，必须使用强制类型转换\n" << endl;
		setbuf(stdin, nullptr);
		debug_output1();
		rewinder(temp, temp2, tmp1, tmp2, output, 1, 1);
		debug_output2();
		input_interactive();
	}
}

/*
 函数名称：debug_output1
 函数参数：无
 函数功能：在DEBUG开发模式(由头文件中是否定义了_DEBUG宏来控制开关)下输出指令刷新前的中间指令
 */
extern "C" __declspec(dllexport) void debug_output1()
{
#ifdef _DEBUG
	{
		cout << "————刷新前的数据————" << endl;
		cout << "temp= " << temp << endl;
		cout << "temp2= " << temp2 << endl;
		cout << "tmp1= " << tmp1 << endl;
		cout << "tmp2= " << tmp2 << endl;
		cout << "tmp3= " << tmp3 << endl;
		cout << "int_count= "<<int_count << endl;
		cout << "dec_count= " << dec_count << endl;
		for(int i=0;i<int_count;i++)
		{
			cout << "int_list["<<i<<"]= " << int_list[i] << endl;
		}
		for (int s = 0; s < int_count; s++)
		{
			cout << "int_value[" << s << "]= " << int_value[s] << endl;
		}
		for (int j=0; j < dec_count; j++)
		{
			cout << "dec_list[" << j << "]= " << dec_list[j] << endl;
		}
		for (int v = 0; v < dec_count; v++)
		{
			cout << "dec_value[" << v << "]= " << dec_value[v] << endl;
		}
		cout << "\n" << endl;
	}
#endif
}

/*
 函数名称：debug_output2
 函数参数：无
 函数功能：在DEBUG开发模式(由头文件中是否定义了_DEBUG宏来控制开关)下输出指令刷新后的空指令
 */
extern "C" __declspec(dllexport) void debug_output2()
{
#ifdef _DEBUG
	{
		cout << "————刷新后的数据————" << endl;
		cout << "temp= " << temp << endl;
		cout << "temp2= " << temp2 << endl;
		cout << "tmp1= " << tmp1 << endl;
		cout << "tmp2= " << tmp2 << endl;
		cout << "tmp3= " << tmp3 << endl;
		cout << "int_count= " << int_count << endl;
		cout << "dec_count= " << dec_count << endl;
		for (int i = 0; i < int_count; i++)
		{
			cout << "int_list[" << i << "]= " << int_list[i] << endl;
		}
		for (int s = 0; s < int_count; s++)
		{
			cout << "int_value[" << s << "]= " << int_value[s] << endl;
		}
		for (int j = 0; j < dec_count; j++)
		{
			cout << "dec_list[" << j << "]= " << dec_list[j] << endl;
		}
		for (int v = 0; v < dec_count; v++)
		{
			cout << "dec_value[" << v << "]= " << dec_value[v] << endl;
		}
		cout << "\n" << endl;
	}
#endif
}
