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

void getLine(); //��ʼ��������

void MainStart(FILE*fw, FILE*few);  //����
void Start(FILE*fw, FILE*few);   //�ֳ���
void Statement_List(FILE*fw, FILE*few); //˵������
void Statement_List_dot(FILE*fw, FILE*few);  //˵������
void Statement(FILE*fw, FILE*few); //˵�����
void Variable(FILE*fw, FILE*few, char*procs, int vkinds); //����
void Action_List(FILE*fw, FILE*few); //ִ������
void Action_List_dot(FILE*fw, FILE*few); //ִ������
void Action(FILE*fw, FILE*few);  //ִ�����
void Arithmeticexpression(FILE*fw, FILE*few); //�������ʽ
void Arithmeticexpression_dot(FILE*fw, FILE*few); //�������ʽ��
void Function(FILE*fw, FILE*few); //������
void Function_Statement(FILE*fw, FILE*few); //����˵��
void Item(FILE*fw, FILE*few);  //��
void Item_dot(FILE*fw, FILE*few); //�
void Element(FILE*fw, FILE*few);  //����
void Relation(FILE*fw, FILE*few); //��ϵ�����
void Ifexpression(FILE*fw, FILE*few); //�������ʽ
void Parameter(FILE*fw, FILE*few, char*procs); //����

int NumCast(char* chlist); //���ַ����ֱ�ת��������
void ErrorPrintAndInput(char* p, FILE* f); //�����ӡ������Ϣ
void PrintAndInput(char* p, FILE*f); //�����ͬʱ��ӡ
void EndprocessInput(FILE*fw, FILE*few); //��ӡ��β��Ϣ
bool StringCmp(char*s1, char*s2);  //�ж��ַ����Ƿ�ƥ��
bool isConst(int le);  //�ж��ǲ��ǳ���

bool FindVar(char* chlist);  //�ұ�����
void AddVar(char* chlist, char*vprocs, char* vtypes, int vkinds, int vlevs);  //����װ��
bool FindFunVar(char* chlist);   //�ҵ�������
void AddFunVar(char* chlist, char*ptypes, int plevs, int fadrs, int ladrs);   //����װ��

char* TempProcname = "Main\0";
int level = 0;
int tempfadrs = 0;
int templadrs = 0;

char Letter[MAX_L][3] = { "\0" };  //�����ֱ�
char NameL[MAX_L][40] = { "\0" };  //���ʷ���
int NumL[MAX_L] = { 0 };   //�����Ĵ�����������
int *Queue = NumL;  


int NowPointer = 0;  //��ǰָ��
int is_Newdefined_var = 0;
int is_functionvar = 0;
int Is_in_subfunction = 0;
char* Formal_parameter = "\0";

int E_Flag = 0;   //����λ
int line = 1; //��λԴ��������



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

struct FVarS FVarList[100];  //�����б�
int CountFVar = -1;


bool StringCmp(char*s1, char*s2)  //�ж��Ƿ�ƥ��
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

bool isConst(int le)  //�ж��ǲ��ǳ���
{
	if (le == 11) { return true; }
	return false;
}

void ErrorPrintAndInput(char* p, FILE* f)
{
	E_Flag = 1;
	fprintf(f, "***����:%d  %s\n", line, p, Queue[NowPointer]);
	printf("***����:%d  %s: %d\n", line, p, Queue[NowPointer]);
}

void PrintAndInput(char* p, FILE*f) //�����ͬʱ��ӡ
{
	fprintf(f, "%s", p);
	printf(p);
}

void EndprocessInput(FILE*fw, FILE*few)
{
	if (E_Flag == 0) { PrintAndInput("���գ�\n", fw); }
	else { PrintAndInput("�д������ܣ�\n", fw); }

}

void getLine()
{
	errno_t err;
	FILE* f;
	err = fopen_s(&f, Input_File, "r");
	char p[100] = "\0"; //��ʱ����
	if (f)
	{
		int i = 0;
		int j = 0;
		for (i = 0; i <= MAX_L; i++)
		{
			fgets(p, 1000, f); //��һ��
			int l = strlen(p);
			if (!feof(f)) {
				Letter[i][0] = p[l - 3], Letter[i][1] = p[l - 2];  //�����ֱ�
				int s = 0;
				for (j = 0; j<16; j++)
				{
					if (p[j] != 32) {
						NameL[i][s] = p[j];   //�������
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
	else { printf("�ļ���ʧ�ܣ�"); }
}

char* NumList[] = { "0","1","2","3","4","5","6","7","8","9" };
int NumCast(char* chlist) //���ַ����ֱ�ת��������,�Ա��ڶ�λ
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

bool FindVar(char* chlist)  //�ұ�����
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

void AddVar(char* chlist, char*vprocs,char* vtypes, int vkinds, int vlevs)  //����װ��
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

bool FindFunVar(char* chlist)   //�ҵ�������
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

void AddFunVar(char* chlist, char*ptypes, int plevs, int fadrs, int ladrs)   //����װ��
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
	while (Queue[NowPointer] == 24 || Queue[NowPointer] == 25)  //�������β
	{
		if (Queue[NowPointer] == 24) { Advance(); line++; PrintAndInput("����------------------------ \n", fw); }
		if (Queue[NowPointer] == 25) { Advance(); }
	}
}

void MainStart(FILE*fw, FILE*few)  //����
{
	Start(fw, few);
	LAdvance(fw, few);
	LAdvance(fw, few);
	PrintAndInput("����\n", fw);
	EndprocessInput(fw, few);
}

void Start(FILE*fw, FILE*few) //�ֳ���
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
			PrintAndInput("�ֳ���\n", fw);
		}
		else {
			ErrorPrintAndInput("�ֳ���: end ƥ��ʧ�ܣ�", few);
			LAdvance(fw, few);
			PrintAndInput("�ֳ���\n", fw);
		}
	}
//		else {
//			ErrorPrintAndInput("�ֳ���: No letter to match.", few);
//			LAdvance(fw, few);
//			goto chcplus1;
//		}
//	}
	else { ErrorPrintAndInput("�ֳ���: begin ƥ��ʧ��", few); 
			LAdvance(fw, few); 
			goto chc1; }
}

void Statement_List(FILE*fw, FILE*few)  //˵������
{
	Statement(fw, few); //˵�����
	Statement_List_dot(fw, few);  //˵������
	PrintAndInput("˵������\n", fw);
}   

void Statement_List_dot(FILE*fw, FILE*few)  //˵������
{
	if (Queue[NowPointer] == 23) {    //;
		LAdvance(fw, few);
		if (Queue[NowPointer] == 3) {  //integer
			Statement(fw, few);
			Statement_List_dot(fw, few);
		}
		//LAdvance(fw, few);
		PrintAndInput("˵������\n", fw);
	}
	else {
		ErrorPrintAndInput("˵������: ��ƥ��ʧ�ܣ�", few);
		PrintAndInput("˵������\n", fw);
	}
}

void Statement(FILE*fw, FILE*few)  //˵�����
{
	level++;
	if (Queue[NowPointer] == 3) {   //integer
		is_Newdefined_var = 1;
		is_functionvar = 0;
		LAdvance(fw, few);
	chc2:
		if (Queue[NowPointer] == 7) {   // function
			is_functionvar = 1; //���庯��
			LAdvance(fw, few);
			Function_Statement(fw, few);  //���뺯��˵��
		}
		else {   //�������
			if (Is_in_subfunction)  //����������Ӻ��������˵�����
			{
				if (StringCmp(Formal_parameter, NameL[NowPointer])) //�Ҷ�Ӧ���β��ַ�
				{
					Parameter(fw, few, TempProcname);
					templadrs++;
					//Is_in_subfunction = 0;
				}
				else { 
					ErrorPrintAndInput("˵�����: �β�δ������", few); 
					LAdvance(fw, few); 
					//Is_in_subfunction = 0; 
				}
			}
			else
			{
				Variable(fw, few, TempProcname, 0);  //�����Ķ�����ͨ����
			}

		}
		PrintAndInput("˵�����\n", fw);

	}
	else {
		ErrorPrintAndInput("˵�����: ����ƥ��ʧ�ܣ�", few);   //˵���ⲻ����˵�������ʽ
		LAdvance(fw, few);        
		is_Newdefined_var = 1;
		is_functionvar = 0; 
		goto chc2;
	}
	level--;
}

void Variable(FILE*fw, FILE*few, char*procs, int vkinds)  //����
{
	if (Queue[NowPointer] == 10) {   //��ʶ��
		if (is_Newdefined_var == 1) {   //���¶���ı���
			if (is_functionvar == 0) {
				if (FindVar(NameL[NowPointer])) {   //�ҵ���
					ErrorPrintAndInput("����: �����Ѿ������壡", few);   //�ظ����壡
					LAdvance(fw, few); 
					is_Newdefined_var = 0; 
				}
				else {   //û�ҵ�
					AddVar(NameL[NowPointer], procs, "ints", vkinds, level);   //��¼�ñ�ʶ��
					is_Newdefined_var = 0;
					LAdvance(fw, few);
					PrintAndInput("����\n", fw);
				}
				}
			else {   //�Ǻ�����
				AddFunVar(NameL[NowPointer],"ints", level, tempfadrs, templadrs);  //��¼�ú�����
				is_functionvar = 0;
				is_Newdefined_var = 0;
				LAdvance(fw, few);
				PrintAndInput("������\n", fw);
				}
			}
		else {   //���ù��ı�ʶ��
				if (FindVar(NameL[NowPointer])) { 
					PrintAndInput("����\n", fw); 
				}
				else if (FindFunVar(NameL[NowPointer])) { 
					PrintAndInput("������\n", fw); 
				}
				else {
					ErrorPrintAndInput("����: ����δ���壡", few);
				}
				LAdvance(fw, few);
			}
		}
	else {
		ErrorPrintAndInput("�������Ƿ�������", few); 
		LAdvance(fw, few); 
	}
}

void Function_Statement(FILE*fw, FILE*few)  //����˵��
{
	is_Newdefined_var = 1;  //���¶����ʶ��
	TempProcname = NameL[NowPointer];
	Variable(fw, few, TempProcname, 0);
	if (Queue[NowPointer] == 21) {   //��
		LAdvance(fw, few);
	chc5:
		is_Newdefined_var = 0;
		if (FindVar(NameL[NowPointer]))  //��ʽ����
		{
			ErrorPrintAndInput("����˵�����β��Ѿ������壡", few); 
			Formal_parameter = NameL[NowPointer];   //��¼������β�
			LAdvance(fw, few);
		}
		else { 
			Formal_parameter = NameL[NowPointer];   //��¼������β�
			LAdvance(fw, few); 
		}

		if (Queue[NowPointer] == 22) {  //  ��
			LAdvance(fw, few);
		chc4:
			if (Queue[NowPointer] == 23) {   //��
				LAdvance(fw, few);
			chc3:

				Function(fw, few);
				PrintAndInput("����˵��\n", fw);

			}
			else { 
				ErrorPrintAndInput("����˵��: ��ƥ��ʧ�ܣ�", few); 
				LAdvance(fw, few); 
				goto chc3; 
			}

		}
		else { 
			ErrorPrintAndInput("����˵��: �� ƥ��ʧ�ܣ�", few); 
			LAdvance(fw, few); 
			goto chc4; 
		}

	}
	else { 
		ErrorPrintAndInput("����˵��: �� ƥ��ʧ�ܣ�", few); 
		LAdvance(fw, few); 
		goto chc5; 
	}
}

void Parameter(FILE*fw, FILE*few, char*procs) //����
{
	//is_Newdefined_var = 0;
	Variable(fw, few, procs, 1);
	PrintAndInput("����\n", fw);
}

void Function(FILE*fw, FILE*few)   //������
{ 
	if (Queue[NowPointer] == 1) {   //begin
		LAdvance(fw, few);
	chc6:
		Is_in_subfunction = 1;  //�ӹ����ڲ�����ʽ����
		tempfadrs = CountVar+1;
		Statement_List(fw, few);   //˵������
		Is_in_subfunction = 0;  //�ӹ����ڲ�����ʽ����
		Action_List(fw, few);   //ִ������
		if (Queue[NowPointer] == 2) {  //end
			LAdvance(fw, few);
		}
		else {
			ErrorPrintAndInput("������: end ƥ��ʧ�ܣ�", few); 
			LAdvance(fw, few);
		}
		PrintAndInput("������\n", fw);
	}
	else { 
		ErrorPrintAndInput("������: begin ƥ��ʧ�ܣ�", few); 
		LAdvance(fw, few); 
		goto chc6; 
	}
	FVarList[CountFVar].fadr = tempfadrs;
	FVarList[CountFVar].ladr = templadrs;
	tempfadrs = 0;
	templadrs = 0;
	TempProcname = "Main\0";
}

void Action_List(FILE*fw, FILE*few)  //ִ������
{
	Action(fw, few);   //ִ�����
	Action_List_dot(fw, few);  //ִ������
	PrintAndInput("ִ������\n", fw);
}

void Action_List_dot(FILE*fw, FILE*few)   //ִ������
{
	if (Queue[NowPointer] == 23) {  //��
		LAdvance(fw, few);
		Action(fw, few);   //ִ�����
		Action_List_dot(fw, few);  //ִ������
	}
	PrintAndInput("ִ������\n", fw);
}

void Action(FILE*fw, FILE*few)  //ִ�����
{
	if (Queue[NowPointer] == 8) {   //read��������
		LAdvance(fw, few);
		if (Queue[NowPointer] == 21) {  //��
			LAdvance(fw, few);
		chc7:
			is_Newdefined_var = 0;
			Variable(fw, few, TempProcname,0);
			if (Queue[NowPointer] == 22) {  // )
				LAdvance(fw, few); 
				PrintAndInput("������\n", fw); 
			}
			else { 
				ErrorPrintAndInput("������: �� ƥ��ʧ�ܣ�", few); 
				LAdvance(fw, few); 
				PrintAndInput("������\n", fw); 
			}
		}
		else { 
			ErrorPrintAndInput("������: �� ƥ��ʧ�ܣ�", few); 
			LAdvance(fw, few); 
			goto chc7; 
		}
	}

	else if (Queue[NowPointer] == 9) {   //write��������
		LAdvance(fw, few);
		if (Queue[NowPointer] == 21) {
			LAdvance(fw, few);
		chc8:
			is_Newdefined_var = 0;
			Variable(fw, few, TempProcname,0);
			if (Queue[NowPointer] == 22) { 
				LAdvance(fw, few); 
				PrintAndInput("д����\n", fw); 
			}
			else { 
				ErrorPrintAndInput("д����: �� ƥ��ʧ��!", few); 
				LAdvance(fw, few); 
				PrintAndInput("д����\n", fw); }
		}
		else { 
			ErrorPrintAndInput("д����: ( ƥ��ʧ�ܣ�", few); 
			LAdvance(fw, few); 
			goto chc8; 
		}
	}

	else if (Queue[NowPointer] == 4) {   //������䣬if��then��else
		LAdvance(fw, few);
		Ifexpression(fw, few);
		if (Queue[NowPointer] == 5) {
			LAdvance(fw, few);
		chc10:
			Action(fw, few); //ƥ�������֮��û��ƥ�䵽else
			if (Queue[NowPointer] == 6) {
				LAdvance(fw, few);
			chc9:
				Action(fw, few);
				PrintAndInput("�������\n", fw);
			}
			else { 
				ErrorPrintAndInput("�������: else ƥ��ʧ�ܣ�", few);
				LAdvance(fw, few); 
				goto chc9; 
			}

		}
		else { 
			ErrorPrintAndInput("�������: then ƥ��ʧ�ܣ�", few); 
			LAdvance(fw, few); 
			goto chc10; 
		}
	}

	else {    //��ֵ���
		is_Newdefined_var = 0;
		Variable(fw, few, TempProcname, 0);
		if (Queue[NowPointer] == 20) {   //:=
			LAdvance(fw, few);
		chc11:
			Arithmeticexpression(fw, few);
			PrintAndInput("��ֵ���\n", fw);
		}
		else { 
			ErrorPrintAndInput("��ֵ���: := ƥ��ʧ�ܣ�", few);
			LAdvance(fw, few); 
			goto chc11; 
		}
	}
}

void Arithmeticexpression(FILE*fw, FILE*few)   //�������ʽ
{
	Item(fw, few);    //��
	Arithmeticexpression_dot(fw, few);   //�������ʽ��
	PrintAndInput("�������ʽ\n", fw);
}

void Arithmeticexpression_dot(FILE*fw, FILE*few)    //�������ʽ��
{
	if (Queue[NowPointer] == 18) {    // -
		LAdvance(fw, few); 
		Item(fw, few); 
		Arithmeticexpression_dot(fw, few);
	}
	PrintAndInput("�������ʽ��\n", fw);
}

void Item(FILE*fw, FILE*few)   //��
{
	Element(fw, few);  //����
	Item_dot(fw, few);    //�
	PrintAndInput("��\n", fw);
}

void Item_dot(FILE*fw, FILE*few)    //�
{
	if (Queue[NowPointer] == 19) {    // *
		LAdvance(fw, few); 
		Element(fw, few); 
		Item_dot(fw, few); 
	}
	PrintAndInput("�\n", fw);
}

void Element(FILE*fw, FILE*few)   //����
{
	if (isConst(Queue[NowPointer]) != true)
	{
		Variable(fw, few, TempProcname, 0);    //������ ���������ʽ��
		if (Queue[NowPointer] == 21) {
			LAdvance(fw, few);
			Arithmeticexpression(fw, few);
			if (Queue[NowPointer] == 22) { 
				LAdvance(fw, few); 
			}
			else { 
				ErrorPrintAndInput("����: �� ƥ��ʧ�ܣ�", few); 
				LAdvance(fw, few); 
			}
		}

	}
	else {
		LAdvance(fw, few);
	}
	PrintAndInput("����\n", fw);
}

void Ifexpression(FILE*fw, FILE*few) //�������ʽ
{
	Arithmeticexpression(fw, few);
	Relation(fw, few);
	Arithmeticexpression(fw, few);
	PrintAndInput("�������ʽ\n", fw);
}

void Relation(FILE*fw, FILE*few)  //��ϵ�����
{
	if (Queue[NowPointer] == 12) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else if (Queue[NowPointer] == 13) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else if (Queue[NowPointer] == 14) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else if (Queue[NowPointer] == 15) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else if (Queue[NowPointer] == 16) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else if (Queue[NowPointer] == 17) { LAdvance(fw, few); PrintAndInput("��ϵ�����\n", fw); }
	else { 
		ErrorPrintAndInput("��ϵ�����: ��ϵ�����ƥ��ʧ�ܣ�", few);
		LAdvance(fw, few); 
		PrintAndInput("��ϵ�����\n", fw); }
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
		printf("û����д�����ļ���");
		exit(0);
	}
	else { printf("�ɹ���д�����ļ���\n"); }

	if (FiletowriteError == NULL) {
		printf("û����д�����ļ���.");
		exit(0);
	}
	else { printf("�ɹ���д�����ļ���\n"); }

	if (FiletosaveVar == NULL) {
		printf("û����д�������ļ���");
		exit(0);
	}
	else { printf("�ɹ���д�������ļ���\n"); }

	if (FiletosaveFunVar == NULL) {
		printf("û����д���̱��ļ���");
		exit(0);
	}
	else { printf("�ɹ���д���̱��ļ���\n"); }

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