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
void result(char *,int);

char reply[50] = {0}; 
int readcnt=0;//읽은 횟수
int indexs=0;
int qsec = 0;///전역변수
question questions[300];// 문제들을 저장하기 위한 question 구조체 배열

void readfile()
{
	FILE *fp = fopen("ls.txt","r");
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

	it.it_value.tv_sec = 15;// 시그널 15초 지정 
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec;
	it.it_interval.tv_usec = 0;

	pid_t pid;
	char * name;
	name = getlogin();// userid 캻져오기 �

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

		printf("%d번 문제 : %s\n",cnt+1,questions[cnt].p);
		printf("답 : %s",questions[cnt].r);
		scanf("%s",reply);
		if(strncmp(questions[cnt].r,reply,strlen(questions[cnt].r)-1)==0) //만약 답과 답이 같다면 정답
		{ 
			printf("정답 입니다.\n");
			if(cnt!=indexs-1)
			{
			printf("다음 문제로 넘어갑니다.\n");
			}
			else
			{
				printf("시험이 끝났습니다.\n");// 마지막 문제라면 출력
			}

			correct++;
			fflush(stdin);
			continue;
		}
		else if(strcmp(reply,"z")==0)
		{
			printf("시간 초과입니다.\n");
		    fflush(stdin);
		}

		else if(strlen(reply)>1)
		{
			printf("오답 입니다.\n");
			printf("다음 문제로 넘어갑니다.\n");
			fflush(stdin);
			continue;
		}
	
		else
		{
			printf("시간 초과입니다.\n");
		    fflush(stdin);
		}
	}

	result(name,correct);

	

}

char *output[] = {
	"%G년 %m월 %d일 %H:%M" };

void result(char *name, int correct)
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
	int fd; 
	mode_t mode;

	mode = S_IRWXU;

	fd = open("result.txt",O_CREAT|O_APPEND,mode);
	if (fd == -1) {
		perror("result.txt");
		exit(1);
	}
	fp = fdopen(fd,"a+");
	fprintf(fp,"이름 : %d 정답 갯수 : %d/%d\n",name,correct,indexs);
	fclose(fp);


	printf("결과가 기록되었습니다.\n");


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
	printf("문제를 입력하세요 :");
	scanf("%s",problem);
	printf("정답을 입력하세요 :");
	scanf("%s",reply);

    if(write(fd, problem, 50) < 0 ) 
	{
		perror("Write");
	}

	if(write(fd, reply, 50) < 0 )
	{
		perror("Write");
	}

		printf("문제가 추가 되었습니다.\n");

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
