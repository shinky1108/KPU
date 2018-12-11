#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#define BUF_SIZE 100000 

typedef struct question {
	char p[500];// ����
	char r[500];// �� 
}question;

void readfile(); // ���� �о ����ü�� ���� 
void add(char *); 
void handler();
void clear(); //ȭ�� ����� �Լ�
void quizStart(); // ���� ���� �Լ�
void questionAdd(); // ���� �߰� �Լ�
void result(char *,int,char *,int); //��� �����Լ�
void study(); //���� ��� �Լ�
void modify(); // ���� �����Լ�
void gradecard(); //����ǥ ����Լ�
void note();

char reply[50] = {0}; 
int sig = 0; //�ð� �ʰ��� Ȯ���ϱ� ���� ����
int fin = 0; // �ѹ����� ����Ǿ����� Ȯ���ϴ� ����
int readcnt=0;//���� Ƚ��
int indexs=0;
int qsec = 0;///��������
int xnum =0; //�����Ʈ ���� 
question questions[500];// �������� �����ϱ� ���� question ����ü �迭


void readfile(char *filename)//���ڷ� ���� ������ �������� �о�´�.
{	
	int fd;
	char str[200];
	if(strncmp(filename,"note.txt",strlen("note.txt")-1)==0)
	{
		FILE *fp2 = fopen(filename,"a+");
		fclose(fp2);
		fp2 = fopen(filename,"r");
		fgets(str,200,fp2);
		if(strlen(str)<3)
		{
			printf("********************************************************\n");
			printf("����� ������ �����ϴ�.\n");
			printf("�켱 ��� �������ּ���.\n");
			printf("********************************************************\n");
			exit(1);
		}
		fclose(fp2);
	}
	FILE *fp = fopen(filename,"r");

	while(fgets(str, 200, fp) != NULL) {//���پ� ��� �����ϱ����� fgets�� ���
		add(str);//add�Լ��� �̿��� ����ü�� ����
		readcnt++;//���� Ƚ�� �߰�, �ؿ� add���� ����ϱ� ���� �� 
	}

	fclose(fp);
}

void add(char *str)// ������ ����ü�� �����ϱ� ���� �Լ�
{
	if(readcnt%2==0) //���� ¦���� �翬�� ������ ���װ� 
	{
		strcpy(questions[indexs].p,str);
	}
	else //���� Ȧ���� ���� ��
	{
		strcpy(questions[indexs].r,str);
		indexs++;
	}
}

void handler()
{
	sig = 1;
	printf("\a");
}

void clear() // clear ��ɾ ����ϱ� ���� �Լ�
{
	pid_t pid;
	int status;
	switch( pid = fork() ) {
		case -1 :
			perror("fork");
			exit(1);
			break;
		case 0 :
			if(execlp("clear","clear",(char *)NULL) == -1) {
				perror("execlp");
				exit(1);
				break;
				default :
				while (wait(&status) != pid)
					continue;
			}
	}
}

void quizStart(char *filename)//���� ���� �Լ�
{
	int ran;
	int overlap = 0;
	srand(time(0));

	sigset_t st;
	sigfillset(&st);
	sigprocmask(SIG_UNBLOCK, &st, (sigset_t*)NULL);
	struct itimerval it;
	sigset(SIGALRM, handler);
	it.it_value.tv_sec = 10;// �ñ׳� 10�� ���� , �� ������ 10�� 
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	char *name;
	FILE *rfp, *wfp;
	char buf1[BUF_SIZE];
	int n; 
	name = getlogin();// userid 
	int correct = 0;// ���� ���� Ȯ��
	int cnt = 0;
	int pran[500]; // ���� �Լ� �ߺ� Ȯ���� ���� �迭 
	memset(pran,11,sizeof(pran));
	clear();//ȭ�� �����
	int i = 0;

	for(cnt; cnt<10; cnt++)
	{
		if(cnt==indexs)
			break;
		ran = rand();
		ran = ran%indexs;// �������� index�� �޾ƿ´�.
		for(i=0; i<500; i++)
		{
		if(pran[i]==ran)// �ߺ��� �߰��� �� �ߺ� Ȯ�� ���� 1�� ����
		{
			overlap = 1;
			break;
		}
		}
		if(overlap==1)// �ߺ��� �߻��ϸ� �ٽ� for�� ó������ ���ư�
		{
			cnt--;
			overlap = 0;
			continue;
		}
		pran[cnt]=ran;

		sig = 0;
		if (setitimer(ITIMER_REAL, &it, (struct itimerval *)NULL) == -1) {
			perror("setitimer");
			exit(1);
		}
		if (getitimer(ITIMER_REAL, &it) == -1) {
			perror("getitimer");
			exit(1);
		}
		printf("******************************************************************\n");
		printf("%d�� ���� : %s\n",cnt+1,questions[ran].p);
		printf("�� : ");
		gets(reply);
		printf("******************************************************************\n");
		if(strncmp(questions[ran].r,reply,strlen(questions[ran].r)-1)==0) //���� ��� ���� ���ٸ� ����
		{ 
			printf("���� �Դϴ�.\n");
			if(cnt!=indexs-1)// ���� ������ ������ �ƴ϶�� ����Ѵ�.
			{
				printf("���� ������ �Ѿ�ϴ�.\n");

			}
			else
			{
				printf("������ �������ϴ�.\n");// ������ ������� ���
			}
			correct++;// ���� ���� ����
			sleep(1);
			clear();
		}
		else if(sig==1)//���� �ڵ鷯 �Լ�����, signal �߻��� sig = 1 �ϵ��� ����
		{
			printf("�ð� �ʰ��Դϴ�.\n");

			if((wfp = fopen("note.txt", "a+")) == NULL){
				perror("fopen: Write Error");
				exit(1);
			}

			fprintf(wfp, "%s%s",questions[cnt].p,questions[cnt].r);
			xnum++;
			if(cnt!=indexs-1)
			{
				printf("���� ������ �Ѿ�ϴ�.\n");
			}
			sleep(1);
			clear();

		}
		else 
		{
			printf("���� �Դϴ�.\n");


			if((wfp = fopen("note.txt", "a+")) == NULL){
				perror("fopen: Write Error");
				exit(1);
			}

			fprintf(wfp, "%s%s",questions[cnt].p,questions[cnt].r);
			xnum++;

			if(cnt!=indexs-1)
			{
				printf("���� ������ �Ѿ�ϴ�.\n");
			}
			sleep(1);
			clear();
		}
	}
	result(name,correct,filename,cnt);
}

char *output[] = {
	"%G�� %m�� %d�� %H:%M"
};

void result(char *name, int correct, char *filename, int pnum)// ����� ����ϱ� ���� �Լ�
{
	struct tm *tm;
	int n;
	time_t tt;
	time(&tt);
	char buf[257];
	tm = localtime(&tt);//���� �ð� ����
	FILE *fp;
	strftime(buf,sizeof(buf),output[0],tm);

	printf("************************************************\n\n");
	printf("%s �̸� : %s  ���� ���� : %d/%d\n",buf,name,correct,pnum);
	printf("************************************************\n\n");
	if((fp= fopen("result.txt","a+")) == NULL) {
		perror(name);
		exit(1);
	}
	fprintf(fp,"%s ���� ���� : %s �̸� : %s ���� ���� : %d/%d\n",buf,filename,name,correct,pnum);
	fclose(fp);
	printf("����� ��ϵǾ����ϴ�.\n");

	return;
}

void questionAdd(char *file)// ���ϴ� ���� ���Ͽ� ������ �߰��� �� �ִ�.
{
	FILE *fp;
	char problem[500];
	char reply[500];
	int cnt = 0;
	if ((fp = fopen(file,"a+")) == NULL) {
		perror(file);
		exit(1);
	}
	while(1)
	{
		clear();
		printf("������ �Է��ϼ���(�����Ϸ��� q �Է�) :");
		gets(problem);
		if(problem[0]=='q')
			exit(1);
		fflush(stdin);
		printf("������ �Է��ϼ��� :");
		gets(reply);
		fflush(stdin);
		fprintf(fp,"%s\n%s\n",problem,reply);
		printf("************************************************\n\n");
		printf("������ �߰� �Ǿ����ϴ�.\n");
		printf("************************************************\n\n");
		sleep(1);
	}

	fclose(fp);

	return ;
}

void gradecard()/// ����ǥ ����ϱ�
{
	FILE *fp;
	int cnt=1;
	char buf[BUF_SIZE];
	int n; 
	char *name;
	name = getlogin();

	clear();
	printf("*********************************************************************************************\n\n");
	printf("                                  %s���� ����ǥ\n",name);

	if((fp = fopen("result.txt", "r")) == NULL){
		perror("fopen: Gradecard Open Error\n");
		exit(1);
	}

	while(fgets(buf, 200, fp) != NULL) {
		printf("%s",buf);
	}

	printf("********************************************************************************************\n\n");

	fclose(fp);

	return ;
}


void study(char *filename)
{
	int cnt=0;
	clear();
	if(indexs==0)
	{
		printf("������ �������� �ʽ��ϴ�.\n");
		printf("������ �߰��ϰ� �ʹٸ� -a �ɼ��� �̿����ּ���.\n");
		exit(1);
	}
	printf("****************%s ���� �Դϴ�.****************\n\n",filename);

	for(cnt; cnt<indexs; cnt++){
		printf("%d�� ���� : %s",cnt+1,questions[cnt].p);
		printf("�� : %s\n",questions[cnt].r);
	}
	printf("***********************************************\n");
}

void modify(char *filename)// ���ϴ� ������ ������ �� �ִ�.
{
	int cnt=0;
	int pnum=0; //���� ��ȣ  �ޱ�a
	char newp[200]; //���ο� ���� �ޱ� 
	char newr[200]; // ���ο� �� �ޱ�
	FILE *rfp, *wfp;
	char buf[100];


	if(indexs==0)
	{
		clear();
		printf("***************************************\n\n");
		printf("������ �������� �ʽ��ϴ�.\n");
		printf("�ٸ� ���� ������ �Է����ּ���.\n\n");
		printf("���� �Է��� ���� ���� : %s�Դϴ�.\n",filename);
		printf("***************************************\n\n");
	}


	clear();

	printf("************************************************\n\n");
	for(cnt; cnt<indexs; cnt++)
	{
		printf("%d�� ���� : %s\n", cnt+1, questions[cnt].p);
		printf("�� : %s\n", questions[cnt].r);

	}
	printf("\n\n************************************************\n");


	printf("�����ϰ� ���� ������ ��ȣ�� �Է��ϼ���(�����Ϸ��� 0 �Է�) :");
	scanf("%d", &pnum);
	if(pnum>indexs)
	{
		printf("\n\n************************************************\n\n");
		printf("�������� �ʴ�  �����Դϴ�. �ٽ� �Է����ּ���.\n");
		printf("%d�� �̳��� �Է����ּ���.\n",pnum);
		printf("************************************************\n\n");
		cnt=0;
		sleep(1);
		clear();
		exit(1);
	}

	fflush(stdin);

	if(pnum == 0){
		exit(1);
	}
	clear();
	sleep(1);
	printf("************************************************\n\n");
	printf("%d�� ���� : %s\n", pnum, questions[pnum-1].p);//������ ������ �ٽ� ������ش�.
	printf("�� : %s\n", questions[pnum-1].r);
	printf("************************************************\n\n");
	printf("������ �Է��ϼ��� :");
	gets(newp);
	fflush(stdin);

	printf("���� �Է��ϼ��� :");
	gets(newr);
	fflush(stdin);

	strcpy(questions[pnum-1].p, newp);// �ٲ� ���� ����ü�� ���� 
	strcpy(questions[pnum-1].r, newr);
	printf("\n\n************************************************\n\n");
	printf("������ �����Ǿ����ϴ�.*^^*\n");
	printf("************************************************\n");
	sleep(1);


	cnt = 0 ;//���� �ٽ� �б� ���� 0���� �ʱ�ȭ 


	if((rfp = fopen(filename,"w")) ==NULL){
		perror("fopen: Read Error");
		exit(1);
	}

	if((wfp = fopen(filename, "w")) == NULL){
		perror("fopen: Write Error");
		exit(1);
	}

	for(cnt; cnt<indexs; cnt++){
		if(cnt==pnum-1)
		{
			fprintf(wfp, "%s\n%s\n",questions[cnt].p, questions[cnt].r);
		}
		else
		{
			fprintf(wfp, "%s%s",questions[cnt].p, questions[cnt].r);
		}
	}

}

void note()//�����Ʈ ���
{
	char *name;
	int c=0; 
	name = getlogin();

	clear();
	if(indexs==0)
	{
		printf("***********************************************************************************\n");
		printf("Ʋ�� ������ �������� �ʽ��ϴ�.\n");
		printf("������ ���� ���ؼ��� -q �ɼ��� �̿����ּ���.\n");
		printf("***********************************************************************************\n");
		exit(1);
	}

	printf("*****************************************************************************************\n");
	printf("                                  %s���� �����Ʈ\n",name);
	for(c; c<indexs; c++)
	{
		printf("���� : %s\n",questions[c].p);
		printf("�� : %s\n",questions[c].r);
	}
	printf("*****************************************************************************************\n");
}

int main(int argc,char *argv[])
{
	int n;
	extern char *optarg;
	extern int optind;
	sigset_t st;
	sigfillset(&st);// �������� ����
	sigprocmask(SIG_BLOCK, &st, (sigset_t *)NULL);


	while ((n = getopt(argc,argv,"a:q:rxs:m:p")) != -1) {
		switch (n) {
			case 'a': //a �ɼ�, ���� �߰��ϱ� 
				readfile(optarg);
				questionAdd(optarg);
				break;

			case 'q': //q �ɼ�, ���� ����
				readfile(optarg);
				quizStart(optarg);
				break;

			case 'r': //r �ɼ�, ���� ����
				gradecard();
				break;

			case 's': //s �ɼ�, ���� �ϱ�
				readfile(optarg);
				study(optarg);
				break; 

			case 'm': //m �ɼ�, ���� ���� �ϱ�
				readfile(optarg);
				modify(optarg);
				break;

			case 'x': //x �ɼ�, ���� ��Ʈ ����
				readfile("note.txt");
				note();
				break;

			case 'p': //p �ɼ�, ���� ��Ʈ�� ���躸��
				readfile("note.txt");
				quizStart("note.txt"); 
		}
	}

	if(argc<2)
	{
		clear();
		printf("********************************************************\n");
		printf("********************************************************\n\n");
		printf(" -a [���� �ؽ�Ʈ ����] \n");
		printf(" ������ ���� �߰���ų �� �ֽ��ϴ�.\n");
		printf(" ���� �Է� �� q�� �Է��ϸ� ���� �߰��� �����մϴ�.\n\n");
		printf(" -q [���� �ؽ�Ʈ ����] \n");
		printf(" �ڽ��� �Ƿ��� �׽�Ʈ �� �� �ֽ��ϴ�.\n");
		printf(" ����Ǿ��ִ� ������ �������� 15������ ���õ˴ϴ�.\n");
		printf(" �� ������ 10�ʷ� ������ �θ�, Ʋ�� �� �Է½� ����ó���˴ϴ�.\n");
		printf(" 15������ ��� Ǯ������ ����ǥ�� ������ �ۼ��˴ϴ�.\n\n");
		printf(" -s [���� �ؽ�Ʈ ����] \n");
		printf(" �׽�Ʈ ���� ������ ����� ���� ������ �� �ֽ��ϴ�.\n\n");
		printf(" -m [���� �ؽ�Ʈ ����] \n");
		printf(" �ڽ��� �Է��� ������ ���� ������ �� �ֽ��ϴ�.\n\n");
		printf(" -r \n");
		printf(" �ڽ��� ������ Ȯ���� �� �ֽ��ϴ�.\n");
		printf(" �ڽ��� ������ ����� �Ѵ��� Ȯ���� �� �ֽ��ϴ�.\n\n");
		printf(" -x \n");
		printf(" Ʋ�ȴ� ������ �Ѵ��� Ȯ���� �� �ֽ��ϴ�.\n");
		printf(" ��� ���� �ڵ����� �����Ʈ�� ���ڰ� �߰��˴ϴ�.\n\n");
		printf(" -p \n");
		printf(" Ʋ�ȴ� ������θ� ��� �� �� �ֽ��ϴ�.\n\n");
		printf("********************************************************\n");
		printf("********************************************************\n");
	}

	sigprocmask(SIG_UNBLOCK, &st, (sigset_t*)NULL);

	return 0;

}
