#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_L 2048

const char* Input_File =  "./Input/Input_File.txt" ;
const char* Filewrite_Process = "./Output/Filewrite_Process.txt";
const char* Filewrite_Err = "./Output/Filewrite_Err.txt";
const char* Filewrite_Varlist = "./Output/Filewrite_Varlist.txt";
const char* Filewrite_FunVarList = "./Output/Filewrite_FunVarList.txt";

void getLine(); //初始化处理函数

void MainStart(FILE*fw, FILE*few);  //程序
void Start(FILE*fw, FILE*few);   //分程序
void Statement_List(FILE*fw, FILE*few); //说明语句表
void Statement_List_dot(FILE*fw, FILE*few);  //说明语句表’
void Statement(FILE*fw, FILE*few); //说明语句
void Variable(FILE*fw, FILE*few, char*procs, int vkinds); //变量
void Action_List(FILE*fw, FILE*few); //执行语句表
void Action_List_dot(FILE*fw, FILE*few); //执行语句表’
void Action(FILE*fw, FILE*few);  //执行语句
void Arithmeticexpression(FILE*fw, FILE*few); //算数表达式
void Arithmeticexpression_dot(FILE*fw, FILE*few); //算数表达式’
void Function(FILE*fw, FILE*few); //函数体
void Function_Statement(FILE*fw, FILE*few); //函数说明
void Item(FILE*fw, FILE*few);  //项
void Item_dot(FILE*fw, FILE*few); //项’
void Element(FILE*fw, FILE*few);  //因子
void Relation(FILE*fw, FILE*few); //关系运算符
void Ifexpression(FILE*fw, FILE*few); //条件表达式
void Parameter(FILE*fw, FILE*few, char*procs); //参数

int NumCast(char* chlist); //将字符的种别转换成数字
void ErrorPrintAndInput(char* p, FILE* f); //输出打印错误信息
void PrintAndInput(char* p, FILE*f); //输入的同时打印
void EndprocessInput(FILE*fw, FILE*few); //打印结尾信息
bool StringCmp(char*s1, char*s2);  //判断字符串是否匹配
bool isConst(int le);  //判断是不是常数

bool FindVar(char* chlist);  //找变量名
void AddVar(char* chlist, char*vprocs, char* vtypes, int vkinds, int vlevs);  //变量装入
bool FindFunVar(char* chlist);   //找到过程名
void AddFunVar(char* chlist, char*ptypes, int plevs, int fadrs, int ladrs);   //过程装入

char* TempProcname = "Main\0";
int level = 0;
int tempfadrs = 0;
int templadrs = 0;

char Letter[MAX_L][3] = { "\0" };  //读入种别
char NameL[MAX_L][40] = { "\0" };  //单词符号
int NumL[MAX_L] = { 0 };   //分析的串，以种类表别
int *Queue = NumL;  


int NowPointer = 0;  //当前指针
int is_Newdefined_var = 0;
int is_functionvar = 0;
int Is_in_subfunction = 0;
char* Formal_parameter = "\0";

int E_Flag = 0;   //错误位
int line = 1; //定位源代码行数



struct VarS {
	char vname[17] = { "\0" };
	char vproc[17] = { "\0" };
	bool vkind;
	char vtype[17] = { "\0" };
	int vlev;
	int vadr;
};

struct VarS VarList[100];
int CountVar = -1;


struct FVarS {
	char pname[17] = { "\0" };
	char ptype[17] = { "\0" };
	int plev;
	int fadr;
	int ladr;
};

struct FVarS FVarList[100];  //函数列表
int CountFVar = -1;


bool StringCmp(char*s1, char*s2)  //判断是否匹配
{
	int i = 0;
	for (i = 0; i<17; i++)
	{
		if (s1[i] != s2[i]) {
			return false;
		}
	}
	return true;
}

bool isConst(int le)  //判断是不是常数
{
	if (le == 11) { return true; }
	return false;
}

void ErrorPrintAndInput(char* p, FILE* f)
{
	E_Flag = 1;
	fprintf(f, "***行数:%d  %s\n", line, p, Queue[NowPointer]);
	printf("***行数:%d  %s: %d\n", line, p, Queue[NowPointer]);
}

void PrintAndInput(char* p, FILE*f) //输入的同时打印
{
	fprintf(f, "%s", p);
	printf(p);
}

void EndprocessInput(FILE*fw, FILE*few)
{
	if (E_Flag == 0) { PrintAndInput("接收！\n", fw); }
	else { PrintAndInput("有错，不接受！\n", fw); }

}

void getLine()
{
	errno_t err;
	FILE* f;
	err = fopen_s(&f, Input_File, "r");
	char p[100] = "\0"; //临时存行
	if (f)
	{
		int i = 0;
		int j = 0;
		for (i = 0; i <= MAX_L; i++)
		{
			fgets(p, 1000, f); //读一行
			int l = strlen(p);
			if (!feof(f)) {
				Letter[i][0] = p[l - 3], Letter[i][1] = p[l - 2];  //读入种别
				int s = 0;
				for (j = 0; j<16; j++)
				{
					if (p[j] != 32) {
						NameL[i][s] = p[j];   //读入符号
						s = s + 1;
					}
				}
				NameL[i][s + 1] = '\0';
			}
			else { break; }
		}
		fclose(f);
		for (i = 0; i <= MAX_L; i++)
		{
			NumL[i] = NumCast(Letter[i]);
		}
	}
	else { printf("文件打开失败！"); }
}

char* NumList[] = { "0","1","2","3","4","5","6","7","8","9" };
int NumCast(char* chlist) //将字符的种别转换成数字,以便于定位
{
	int show = 0;
	int len = strlen(chlist);
	int i, j;
	char c;

	for (j = len - 1; j >= 0; j--)
	{
		c = chlist[j];
		for (i = 0; i<10; i++)
		{
			if (c == *NumList[i])
			{
				show += i*(int)pow(10, len - j - 1);
			}
		}
	}
	return show;
}

bool FindVar(char* chlist)  //找变量名
{
	int i;
	for (i = 0; i<100; i++)
	{
		if (StringCmp(chlist, VarList[i].vname) == true)
		{
			return true;
		}
	}
	return false;
}

void AddVar(char* chlist, char*vprocs,char* vtypes, int vkinds, int vlevs)  //变量装入
{
	CountVar++;
	int len = strlen(chlist);
	int i = 0;
	for (i = 0; i<len; i++)
	{
		VarList[CountVar].vname[i]= chlist[i];
	}
	VarList[CountVar].vname[len] = '\0';
	len = strlen(vprocs);
	for (i = 0; i<len; i++)
	{
		VarList[CountVar].vproc[i] = vprocs[i];
	}
	VarList[CountVar].vproc[len] = '\0';
	VarList[CountVar].vkind = vkinds;
	len = strlen(vtypes);
	for (i = 0; i<len; i++)
	{
		VarList[CountVar].vtype[i] = vtypes[i];
	}
	VarList[CountVar].vtype[len] = '\0';
	VarList[CountVar].vlev = vlevs;
	VarList[CountVar].vadr = CountVar;
};

bool FindFunVar(char* chlist)   //找到过程名
{
	int i = 0;
	for (i = 0; i<100; i++)
	{
		if (StringCmp(chlist, FVarList[i].pname) == true)
		{
			return true;
		}
	}
	return false;
}

void AddFunVar(char* chlist, char*ptypes, int plevs, int fadrs, int ladrs)   //过程装入
{
	CountFVar++;
	int len = strlen(chlist);
	int i;
	for (i = 0; i<len; i++)
	{
		FVarList[CountFVar].pname[i] = chlist[i];
	}
	FVarList[CountFVar].pname[len] = '\0';
	len = strlen(ptypes);
	for (i = 0; i<len; i++)
	{
		FVarList[CountFVar].ptype[i] = ptypes[i];
	}
	FVarList[CountFVar].ptype[len] = '\0';
	FVarList[CountFVar].plev = plevs;
	FVarList[CountFVar].fadr = fadrs;
	FVarList[CountFVar].ladr = ladrs;
};

void Advance()
{
	NowPointer += 1;   
}

void LAdvance(FILE*fw, FILE*few)
{
	printf("%d\n", Queue[NowPointer]);
	Advance();
	while (Queue[NowPointer] == 24 || Queue[NowPointer] == 25)  //换行与结尾
	{
		if (Queue[NowPointer] == 24) { Advance(); line++; PrintAndInput("换行------------------------ \n", fw); }
		if (Queue[NowPointer] == 25) { Advance(); }
	}
}

void MainStart(FILE*fw, FILE*few)  //程序
{
	Start(fw, few);
	LAdvance(fw, few);
	LAdvance(fw, few);
	PrintAndInput("程序\n", fw);
	EndprocessInput(fw, few);
}

void Start(FILE*fw, FILE*few) //分程序
{
	if (Queue[NowPointer] == 1) {   //begin
		LAdvance(fw, few);
	chc1:
		Statement_List(fw, few);
//		if (Queue[NowPointer] == 23){
//			LAdvance(fw, few);
//	chcplus1:
		Action_List(fw, few);
		if (Queue[NowPointer] == 2) {   //end
			LAdvance(fw, few);
			PrintAndInput("分程序\n", fw);
		}
		else {
			ErrorPrintAndInput("分程序: end 匹配失败！", few);
			LAdvance(fw, few);
			PrintAndInput("分程序\n", fw);
		}
	}
//		else {
//			ErrorPrintAndInput("分程序: No letter to match.", few);
//			LAdvance(fw, few);
//			goto chcplus1;
//		}
//	}
	else { ErrorPrintAndInput("分程序: begin 匹配失败", few); 
			LAdvance(fw, few); 
			goto chc1; }
}

void Statement_List(FILE*fw, FILE*few)  //说明语句表
{
	Statement(fw, few); //说明语句
	Statement_List_dot(fw, few);  //说明语句表’
	PrintAndInput("说明语句表\n", fw);
}   

void Statement_List_dot(FILE*fw, FILE*few)  //说明语句表’
{
	if (Queue[NowPointer] == 23) {    //;
		LAdvance(fw, few);
		if (Queue[NowPointer] == 3) {  //integer
			Statement(fw, few);
			Statement_List_dot(fw, few);
		}
		//LAdvance(fw, few);
		PrintAndInput("说明语句表’\n", fw);
	}
	else {
		ErrorPrintAndInput("说明语句表: ；匹配失败！", few);
		PrintAndInput("说明语句表’\n", fw);
	}
}

void Statement(FILE*fw, FILE*few)  //说明语句
{
	level++;
	if (Queue[NowPointer] == 3) {   //integer
		is_Newdefined_var = 1;
		is_functionvar = 0;
		LAdvance(fw, few);
	chc2:
		if (Queue[NowPointer] == 7) {   // function
			is_functionvar = 1; //定义函数
			LAdvance(fw, few);
			Function_Statement(fw, few);  //进入函数说明
		}
		else {   //定义变量
			if (Is_in_subfunction)  //如果这是在子函数里面的说明语句
			{
				if (StringCmp(Formal_parameter, NameL[NowPointer])) //找对应的形参字符
				{
					Parameter(fw, few, TempProcname);
					templadrs++;
					//Is_in_subfunction = 0;
				}
				else { 
					ErrorPrintAndInput("说明语句: 形参未声明！", few); 
					LAdvance(fw, few); 
					//Is_in_subfunction = 0; 
				}
			}
			else
			{
				Variable(fw, few, TempProcname, 0);  //单纯的定义普通变量
			}

		}
		PrintAndInput("说明语句\n", fw);

	}
	else {
		ErrorPrintAndInput("说明语句: 类型匹配失败！", few);   //说明这不符合说明语句形式
		LAdvance(fw, few);        
		is_Newdefined_var = 1;
		is_functionvar = 0; 
		goto chc2;
	}
	level--;
}

void Variable(FILE*fw, FILE*few, char*procs, int vkinds)  //变量
{
	if (Queue[NowPointer] == 10) {   //标识符
		if (is_Newdefined_var == 1) {   //是新定义的变量
			if (is_functionvar == 0) {
				if (FindVar(NameL[NowPointer])) {   //找到了
					ErrorPrintAndInput("变量: 变量已经被定义！", few);   //重复定义！
					LAdvance(fw, few); 
					is_Newdefined_var = 0; 
				}
				else {   //没找到
					AddVar(NameL[NowPointer], procs, "ints", vkinds, level);   //记录该标识符
					is_Newdefined_var = 0;
					LAdvance(fw, few);
					PrintAndInput("变量\n", fw);
				}
				}
			else {   //是函数名
				AddFunVar(NameL[NowPointer],"ints", level, tempfadrs, templadrs);  //记录该函数名
				is_functionvar = 0;
				is_Newdefined_var = 0;
				LAdvance(fw, few);
				PrintAndInput("过程名\n", fw);
				}
			}
		else {   //是用过的标识符
				if (FindVar(NameL[NowPointer])) { 
					PrintAndInput("变量\n", fw); 
				}
				else if (FindFunVar(NameL[NowPointer])) { 
					PrintAndInput("过程名\n", fw); 
				}
				else {
					ErrorPrintAndInput("变量: 变量未定义！", few);
				}
				LAdvance(fw, few);
			}
		}
	else {
		ErrorPrintAndInput("变量：非法变量！", few); 
		LAdvance(fw, few); 
	}
}

void Function_Statement(FILE*fw, FILE*few)  //函数说明
{
	is_Newdefined_var = 1;  //是新定义标识符
	TempProcname = NameL[NowPointer];
	Variable(fw, few, TempProcname, 0);
	if (Queue[NowPointer] == 21) {   //（
		LAdvance(fw, few);
	chc5:
		is_Newdefined_var = 0;
		if (FindVar(NameL[NowPointer]))  //形式参数
		{
			ErrorPrintAndInput("函数说明：形参已经被定义！", few); 
			Formal_parameter = NameL[NowPointer];   //记录定义的形参
			LAdvance(fw, few);
		}
		else { 
			Formal_parameter = NameL[NowPointer];   //记录定义的形参
			LAdvance(fw, few); 
		}

		if (Queue[NowPointer] == 22) {  //  ）
			LAdvance(fw, few);
		chc4:
			if (Queue[NowPointer] == 23) {   //；
				LAdvance(fw, few);
			chc3:

				Function(fw, few);
				PrintAndInput("函数说明\n", fw);

			}
			else { 
				ErrorPrintAndInput("函数说明: ；匹配失败！", few); 
				LAdvance(fw, few); 
				goto chc3; 
			}

		}
		else { 
			ErrorPrintAndInput("函数说明: ） 匹配失败！", few); 
			LAdvance(fw, few); 
			goto chc4; 
		}

	}
	else { 
		ErrorPrintAndInput("函数说明: （ 匹配失败！", few); 
		LAdvance(fw, few); 
		goto chc5; 
	}
}

void Parameter(FILE*fw, FILE*few, char*procs) //参数
{
	//is_Newdefined_var = 0;
	Variable(fw, few, procs, 1);
	PrintAndInput("参数\n", fw);
}

void Function(FILE*fw, FILE*few)   //过程体
{ 
	if (Queue[NowPointer] == 1) {   //begin
		LAdvance(fw, few);
	chc6:
		Is_in_subfunction = 1;  //子过程内部，形式参数
		tempfadrs = CountVar+1;
		Statement_List(fw, few);   //说明语句表
		Is_in_subfunction = 0;  //子过程内部，形式参数
		Action_List(fw, few);   //执行语句表
		if (Queue[NowPointer] == 2) {  //end
			LAdvance(fw, few);
		}
		else {
			ErrorPrintAndInput("过程体: end 匹配失败！", few); 
			LAdvance(fw, few);
		}
		PrintAndInput("过程体\n", fw);
	}
	else { 
		ErrorPrintAndInput("过程体: begin 匹配失败！", few); 
		LAdvance(fw, few); 
		goto chc6; 
	}
	FVarList[CountFVar].fadr = tempfadrs;
	FVarList[CountFVar].ladr = templadrs;
	tempfadrs = 0;
	templadrs = 0;
	TempProcname = "Main\0";
}

void Action_List(FILE*fw, FILE*few)  //执行语句表
{
	Action(fw, few);   //执行语句
	Action_List_dot(fw, few);  //执行语句表’
	PrintAndInput("执行语句表\n", fw);
}

void Action_List_dot(FILE*fw, FILE*few)   //执行语句表’
{
	if (Queue[NowPointer] == 23) {  //；
		LAdvance(fw, few);
		Action(fw, few);   //执行语句
		Action_List_dot(fw, few);  //执行语句表’
	}
	PrintAndInput("执行语句表’\n", fw);
}

void Action(FILE*fw, FILE*few)  //执行语句
{
	if (Queue[NowPointer] == 8) {   //read（变量）
		LAdvance(fw, few);
		if (Queue[NowPointer] == 21) {  //（
			LAdvance(fw, few);
		chc7:
			is_Newdefined_var = 0;
			Variable(fw, few, TempProcname,0);
			if (Queue[NowPointer] == 22) {  // )
				LAdvance(fw, few); 
				PrintAndInput("读变量\n", fw); 
			}
			else { 
				ErrorPrintAndInput("读变量: ） 匹配失败！", few); 
				LAdvance(fw, few); 
				PrintAndInput("读变量\n", fw); 
			}
		}
		else { 
			ErrorPrintAndInput("读变量: （ 匹配失败！", few); 
			LAdvance(fw, few); 
			goto chc7; 
		}
	}

	else if (Queue[NowPointer] == 9) {   //write（变量）
		LAdvance(fw, few);
		if (Queue[NowPointer] == 21) {
			LAdvance(fw, few);
		chc8:
			is_Newdefined_var = 0;
			Variable(fw, few, TempProcname,0);
			if (Queue[NowPointer] == 22) { 
				LAdvance(fw, few); 
				PrintAndInput("写变量\n", fw); 
			}
			else { 
				ErrorPrintAndInput("写变量: ） 匹配失败!", few); 
				LAdvance(fw, few); 
				PrintAndInput("写变量\n", fw); }
		}
		else { 
			ErrorPrintAndInput("写变量: ( 匹配失败！", few); 
			LAdvance(fw, few); 
			goto chc8; 
		}
	}

	else if (Queue[NowPointer] == 4) {   //条件语句，if，then，else
		LAdvance(fw, few);
		Ifexpression(fw, few);
		if (Queue[NowPointer] == 5) {
			LAdvance(fw, few);
		chc10:
			Action(fw, few); //匹配完变量之后，没有匹配到else
			if (Queue[NowPointer] == 6) {
				LAdvance(fw, few);
			chc9:
				Action(fw, few);
				PrintAndInput("条件语句\n", fw);
			}
			else { 
				ErrorPrintAndInput("条件语句: else 匹配失败！", few);
				LAdvance(fw, few); 
				goto chc9; 
			}

		}
		else { 
			ErrorPrintAndInput("条件语句: then 匹配失败！", few); 
			LAdvance(fw, few); 
			goto chc10; 
		}
	}

	else {    //赋值语句
		is_Newdefined_var = 0;
		Variable(fw, few, TempProcname, 0);
		if (Queue[NowPointer] == 20) {   //:=
			LAdvance(fw, few);
		chc11:
			Arithmeticexpression(fw, few);
			PrintAndInput("赋值语句\n", fw);
		}
		else { 
			ErrorPrintAndInput("赋值语句: := 匹配失败！", few);
			LAdvance(fw, few); 
			goto chc11; 
		}
	}
}

void Arithmeticexpression(FILE*fw, FILE*few)   //算术表达式
{
	Item(fw, few);    //项
	Arithmeticexpression_dot(fw, few);   //算数表达式’
	PrintAndInput("算术表达式\n", fw);
}

void Arithmeticexpression_dot(FILE*fw, FILE*few)    //算术表达式’
{
	if (Queue[NowPointer] == 18) {    // -
		LAdvance(fw, few); 
		Item(fw, few); 
		Arithmeticexpression_dot(fw, few);
	}
	PrintAndInput("算术表达式’\n", fw);
}

void Item(FILE*fw, FILE*few)   //项
{
	Element(fw, few);  //因子
	Item_dot(fw, few);    //项’
	PrintAndInput("项\n", fw);
}

void Item_dot(FILE*fw, FILE*few)    //项’
{
	if (Queue[NowPointer] == 19) {    // *
		LAdvance(fw, few); 
		Element(fw, few); 
		Item_dot(fw, few); 
	}
	PrintAndInput("项’\n", fw);
}

void Element(FILE*fw, FILE*few)   //因子
{
	if (isConst(Queue[NowPointer]) != true)
	{
		Variable(fw, few, TempProcname, 0);    //函数体 （算术表达式）
		if (Queue[NowPointer] == 21) {
			LAdvance(fw, few);
			Arithmeticexpression(fw, few);
			if (Queue[NowPointer] == 22) { 
				LAdvance(fw, few); 
			}
			else { 
				ErrorPrintAndInput("因子: ） 匹配失败！", few); 
				LAdvance(fw, few); 
			}
		}

	}
	else {
		LAdvance(fw, few);
	}
	PrintAndInput("因子\n", fw);
}

void Ifexpression(FILE*fw, FILE*few) //条件表达式
{
	Arithmeticexpression(fw, few);
	Relation(fw, few);
	Arithmeticexpression(fw, few);
	PrintAndInput("条件表达式\n", fw);
}

void Relation(FILE*fw, FILE*few)  //关系运算符
{
	if (Queue[NowPointer] == 12) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else if (Queue[NowPointer] == 13) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else if (Queue[NowPointer] == 14) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else if (Queue[NowPointer] == 15) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else if (Queue[NowPointer] == 16) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else if (Queue[NowPointer] == 17) { LAdvance(fw, few); PrintAndInput("关系运算符\n", fw); }
	else { 
		ErrorPrintAndInput("关系运算符: 关系运算符匹配失败！", few);
		LAdvance(fw, few); 
		PrintAndInput("关系运算符\n", fw); }
}

int main()
{
	errno_t err1;
	errno_t err2;
	errno_t err3;
	errno_t err4;
	FILE* FiletowriteProcess;
	FILE* FiletowriteError;
	FILE* FiletosaveVar;
	FILE* FiletosaveFunVar;
	err1 = fopen_s(&FiletowriteProcess, Filewrite_Process, "w");
	err2 = fopen_s(&FiletowriteError,Filewrite_Err, "w");
	err3 = fopen_s(&FiletosaveVar,Filewrite_Varlist, "w");
	err4 = fopen_s(&FiletosaveFunVar,Filewrite_FunVarList, "w");

	if (FiletowriteProcess == NULL) {
		printf("没法打开写过程文件！");
		exit(0);
	}
	else { printf("成功打开写过程文件！\n"); }

	if (FiletowriteError == NULL) {
		printf("没法打开写错误文件！.");
		exit(0);
	}
	else { printf("成功打开写错误文件！\n"); }

	if (FiletosaveVar == NULL) {
		printf("没法打开写变量表文件！");
		exit(0);
	}
	else { printf("成功打开写变量表文件！\n"); }

	if (FiletosaveFunVar == NULL) {
		printf("没法打开写过程表文件！");
		exit(0);
	}
	else { printf("成功打开写过程表文件！\n"); }

	getLine();
	MainStart(FiletowriteProcess, FiletowriteError);

	int m = 0;
	for (m = 0; m<CountVar+1; m++)
	{
		fprintf(FiletosaveVar, "------------\n vname:%s\n vproc:%s\n vkind:%d\n vtype:%s\n vlev:%d\n vadr:%d\n", 
			VarList[m].vname, VarList[m].vproc, VarList[m].vkind, VarList[m].vtype, VarList[m].vlev, VarList[m].vadr);
	}

	for (m = 0; m<CountFVar+1; m++)
	{
		fprintf(FiletosaveFunVar, "-------------\n pname:%s\n ptype:%s\n plev:%d\n fadr:%d\n ladr:%d\n", 
			FVarList[m].pname, FVarList[m].ptype, FVarList[m].plev, FVarList[m].fadr, FVarList[m].ladr);
	}

	fclose(FiletowriteProcess);
	fclose(FiletowriteError);
	fclose(FiletosaveVar);
	fclose(FiletosaveFunVar);
	system("pause");
	return(0);
}