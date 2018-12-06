#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


typedef struct question {
	char p[500];// ����
	char r[500];// �� 
}question;


void readfile();
void add(char *);
void handler();
void clear();
void quizStart();
void result(char *,int,char *);

char reply[50] = {0}; 
int sig = 0; //�ð� �ʰ��� Ȯ���ϱ� ���� ����
int fin = 0; // �ѹ����� ����Ǿ����� Ȯ���ϴ� ����
int readcnt=0;//���� Ƚ��
int indexs=0;
int qsec = 0;///��������
question questions[300];// �������� �����ϱ� ���� question ����ü �迭

void readfile(char *filename)
{
	FILE *fp = fopen(filename,"r");
	char str[200];
	while(fgets(str, 200, fp) != NULL) {//���پ� ��� �����ϱ����� fgets�� ���
		add(str);//add�Լ��� �̿��� ����ü�� ����
		readcnt++;//���� Ƚ�� �߰�, �ؿ� add���� ����ϱ� ���� �� 
	}
}

void add(char *str)
{
	if(readcnt%2==0) //���� ¦���� �翬�� ������ ���װ� 
	{
		strcpy(questions[indexs].p,str);
	}
	else //���� Ȧ���� �翬�� ���� ������
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

void clear()
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

void quizStart(char *filename)
{  
	struct itimerval it;
	sigset(SIGALRM, handler);

	it.it_value.tv_sec = 10;// �ñ׳� 10�� ���� 
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec;
	it.it_interval.tv_usec = 0;


	char * name;
	name = getlogin();// userid 

	int correct = 0;
	int cnt = 0;
	clear();

	for(cnt; cnt<indexs; cnt++)
	{
		sig = 0;
		if (setitimer(ITIMER_REAL, &it, (struct itimerval *)NULL) == -1) {
			perror("setitimer");
			exit(1);
		}
		if (getitimer(ITIMER_REAL, &it) == -1) {
			perror("getitimer");
			exit(1);
		}
        printf("****************************************\n");
		printf("%d�� ���� : %s\n",cnt+1,questions[cnt].p);
		printf("�� : %s",questions[cnt].r);
		scanf("%s",reply);
		printf("***************************************\n");
		if(strncmp(questions[cnt].r,reply,strlen(questions[cnt].r)-1)==0) //���� ��� ���� ���ٸ� ����
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
			if(cnt!=indexs-1)
			{
			printf("���� ������ �Ѿ�ϴ�.\n");
			}
			sleep(1);
			clear();

		}

	}

	result(name,correct,filename);



}

char *output[] = {
	"%G�� %m�� %d�� %H:%M" };

void result(char *name, int correct, char *filename)
{
	struct tm *tm;
	int n;

	time_t tt;
	time(&tt);
	char buf[257];
	tm = localtime(&tt);
	strftime(buf,sizeof(buf),output[0],tm);
	printf("%s �̸� : %s  ���� ���� : %d/%d\n",buf,name,correct,indexs);

	FILE *fp;
	if((fp= fopen("result.txt","a+")) == NULL) {
		perror(name);
		exit(1);
	}

	fprintf(fp,"%s ���� ���� : %s �̸� : %s ���� ���� : %d/%d\n",buf,filename,name,correct,indexs);
	fclose(fp);

	printf("����� ��ϵǾ����ϴ�.\n");

	return;
}

void questionAdd(char *file)
{
	FILE *fp;
	char problem[500];
	char reply[500];


    if ((fp = fopen(file,"a+")) == NULL) {
		perror(file);
		exit(1);
	}
	while(1)
	{
	printf("������ �Է��ϼ���(�����Ϸ��� q �Է�) :");
	scanf("%s",problem);

	if(problem[0]=='q')
		exit(1);
	fflush(stdin);

	printf("������ �Է��ϼ��� :");
	scanf("%s",reply);
	fflush(stdin);

	fprintf(fp,"%s\n%s\n",problem,reply);


	printf("������ �߰� �Ǿ����ϴ�.\n");
	}

	fclose(fp);

	return ;



}


int main(int argc,char *argv[])
{
	int n;
	extern char *optarg;
	extern int optind;

	while ((n = getopt(argc,argv,"a:q:")) != -1) {
		switch (n) {
			case 'a':
				questionAdd(optarg);
				break;
			case 'q':
				readfile(optarg);
				quizStart(optarg);
				break;
			default :
				printf("Quiz Option : ");
		}
	}




	return 0;


}
