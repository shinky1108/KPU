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
	char p[500];// 문제
	char r[500];// 답 
}question;


void readfile();
void add(char *);
void handler();
void clear();
void quizStart();
void result(char *,int,char *);

char reply[50] = {0}; 
int sig = 0; //시간 초과를 확인하기 위한 변수
int fin = 0; // 한문제가 종료되었음을 확인하는 변수
int readcnt=0;//읽은 횟수
int indexs=0;
int qsec = 0;///전역변수
question questions[300];// 문제들을 저장하기 위한 question 구조체 배열

void readfile(char *filename)
{
	FILE *fp = fopen(filename,"r");
	char str[200];
	while(fgets(str, 200, fp) != NULL) {//한줄씩 끊어서 저장하기위해 fgets를 사용
		add(str);//add함수를 이용해 구조체에 저장
		readcnt++;//읽은 횟수 추가, 밑에 add에서 사용하기 위해 씀 
	}
}

void add(char *str)
{
	if(readcnt%2==0) //만약 짝수면 당연히 문제가 들어갈테고 
	{
		strcpy(questions[indexs].p,str);
	}
	else //만약 홀수면 당연히 답이 들어가겠지
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

	it.it_value.tv_sec = 10;// 시그널 10초 지정 
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
		printf("%d번 문제 : %s\n",cnt+1,questions[cnt].p);
		printf("답 : %s",questions[cnt].r);
		scanf("%s",reply);
		printf("***************************************\n");
		if(strncmp(questions[cnt].r,reply,strlen(questions[cnt].r)-1)==0) //만약 답과 답이 같다면 정답
		{ 
			printf("정답 입니다.\n");
			if(cnt!=indexs-1)// 만약 마지막 문제가 아니라면 출력한다.
			{
				printf("다음 문제로 넘어갑니다.\n");
					
			}
			else
			{
				printf("시험이 끝났습니다.\n");// 마지막 문제라면 출력
			}

			correct++;// 맞은 갯수 증가
			sleep(1);
			clear();
		}
		else if(sig==1)//위에 핸들러 함수보면, signal 발생시 sig = 1 하도록 구현
		{
			printf("시간 초과입니다.\n");
			if(cnt!=indexs-1)
			{
			printf("다음 문제로 넘어갑니다.\n");
			}
			sleep(1);
			clear();
			
		}

		else 
		{
			printf("오답 입니다.\n");
			if(cnt!=indexs-1)
			{
			printf("다음 문제로 넘어갑니다.\n");
			}
			sleep(1);
			clear();

		}

	}

	result(name,correct,filename);



}

char *output[] = {
	"%G년 %m월 %d일 %H:%M" };

void result(char *name, int correct, char *filename)
{
	struct tm *tm;
	int n;

	time_t tt;
	time(&tt);
	char buf[257];
	tm = localtime(&tt);
	strftime(buf,sizeof(buf),output[0],tm);
	printf("%s 이름 : %s  정답 갯수 : %d/%d\n",buf,name,correct,indexs);

	FILE *fp;
	if((fp= fopen("result.txt","a+")) == NULL) {
		perror(name);
		exit(1);
	}

	fprintf(fp,"%s 문제 종류 : %s 이름 : %s 정답 갯수 : %d/%d\n",buf,filename,name,correct,indexs);
	fclose(fp);

	printf("결과가 기록되었습니다.\n");

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
	printf("문제를 입력하세요(종료하려면 q 입력) :");
	scanf("%s",problem);

	if(problem[0]=='q')
		exit(1);
	fflush(stdin);

	printf("정답을 입력하세요 :");
	scanf("%s",reply);
	fflush(stdin);

	fprintf(fp,"%s\n%s\n",problem,reply);


	printf("문제가 추가 되었습니다.\n");
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
