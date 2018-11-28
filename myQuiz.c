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
void result(char *,int);

char reply[50] = {0}; 
int readcnt=0;//���� Ƚ��
int indexs=0;
int qsec = 0;///��������
question questions[300];// �������� �����ϱ� ���� question ����ü �迭

void readfile()
{
	FILE *fp = fopen("ls.txt","r");
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
	printf(" ");
}

void clear()
{
	if(execlp("clear","clear",(char *)NULL) == -1) {
		perror("execlp");
		exit(1);
	}
}

void quizStart()
{  
	struct itimerval it;
	sigset(SIGALRM, handler);

	it.it_value.tv_sec = 15;// �ñ׳� 15�� ���� 
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec;
	it.it_interval.tv_usec = 0;

	pid_t pid;
	char * name;
	name = getlogin();// userid �a������ �

    int correct = 0;
	int cnt = 0;
	for(cnt; cnt<indexs; cnt++)
	{
	if (setitimer(ITIMER_REAL, &it, (struct itimerval *)NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
		if (getitimer(ITIMER_REAL, &it) == -1) {
			perror("getitimer");
			exit(1);
		}

		printf("%d�� ���� : %s\n",cnt+1,questions[cnt].p);
		printf("�� : %s",questions[cnt].r);
		scanf("%s",reply);
		if(strncmp(questions[cnt].r,reply,strlen(questions[cnt].r)-1)==0) //���� ��� ���� ���ٸ� ����
		{ 
			printf("���� �Դϴ�.\n");
			if(cnt!=indexs-1)
			{
			printf("���� ������ �Ѿ�ϴ�.\n");
			}
			else
			{
				printf("������ �������ϴ�.\n");// ������ ������� ���
			}

			correct++;
			fflush(stdin);
			continue;
		}
		else if(strcmp(reply,"z")==0)
		{
			printf("�ð� �ʰ��Դϴ�.\n");
		    fflush(stdin);
		}

		else if(strlen(reply)>1)
		{
			printf("���� �Դϴ�.\n");
			printf("���� ������ �Ѿ�ϴ�.\n");
			fflush(stdin);
			continue;
		}
	
		else
		{
			printf("�ð� �ʰ��Դϴ�.\n");
		    fflush(stdin);
		}
	}

	result(name,correct);

	

}

char *output[] = {
	"%G�� %m�� %d�� %H:%M" };

void result(char *name, int correct)
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
	int fd; 
	mode_t mode;

	mode = S_IRWXU;

	fd = open("result.txt",O_CREAT|O_APPEND,mode);
	if (fd == -1) {
		perror("result.txt");
		exit(1);
	}
	fp = fdopen(fd,"a+");
	fprintf(fp,"�̸� : %d ���� ���� : %d/%d\n",name,correct,indexs);
	fclose(fp);


	printf("����� ��ϵǾ����ϴ�.\n");


    return ;
}

void questionAdd(char *file)
{
    int fd;
	char problem[50];
	char reply[50];
		
	mode_t mode;

	mode = S_IRWXU;

	fd = open(file,O_CREAT|O_APPEND,mode);
	if (fd == -1) {
		perror(file);
		exit(1);
	}
	printf("������ �Է��ϼ��� :");
	scanf("%s",problem);
	printf("������ �Է��ϼ��� :");
	scanf("%s",reply);

    if(write(fd, problem, 50) < 0 ) 
	{
		perror("Write");
	}

	if(write(fd, reply, 50) < 0 )
	{
		perror("Write");
	}

		printf("������ �߰� �Ǿ����ϴ�.\n");

	close(fd);
	

}
		

int main(int argc,char *argv[])
{
	if(*argv[1] == '-' && *(argv[1] + 1) == 'q' ) {
		readfile();
		quizStart();
    }
	else if(*argv[1] == '-' && *(argv[1] + 1) == 'a') {
		questionAdd(argv[2]);
	}

	return 0;
		

}
