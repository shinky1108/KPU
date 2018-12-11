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
	char p[500];// 문제
	char r[500];// 답 
}question;

void readfile(); // 파일 읽어서 구조체에 저장 
void add(char *); 
void handler();
void clear(); //화면 지우는 함수
void quizStart(); // 퀴즈 시작 함수
void questionAdd(); // 문제 추가 함수
void result(char *,int,char *,int); //결과 저장함수
void study(); //공부 모드 함수
void modify(); // 문제 수정함수
void gradecard(); //성적표 출력함수
void note();

char reply[50] = {0}; 
int sig = 0; //시간 초과를 확인하기 위한 변수
int fin = 0; // 한문제가 종료되었음을 확인하는 변수
int readcnt=0;//읽은 횟수
int indexs=0;
int qsec = 0;///전역변수
int xnum =0; //오답노트 개수 
question questions[500];// 문제들을 저장하기 위한 question 구조체 배열


void readfile(char *filename)//인자로 받은 파일의 문제들을 읽어온다.
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
			printf("저장된 오답이 없습니다.\n");
			printf("우선 퀴즈를 실행해주세요.\n");
			printf("********************************************************\n");
			exit(1);
		}
		fclose(fp2);
	}
	FILE *fp = fopen(filename,"r");

	while(fgets(str, 200, fp) != NULL) {//한줄씩 끊어서 저장하기위해 fgets를 사용
		add(str);//add함수를 이용해 구조체에 저장
		readcnt++;//읽은 횟수 추가, 밑에 add에서 사용하기 위해 씀 
	}

	fclose(fp);
}

void add(char *str)// 문제를 구조체에 저장하기 위한 함수
{
	if(readcnt%2==0) //만약 짝수면 당연히 문제가 들어갈테고 
	{
		strcpy(questions[indexs].p,str);
	}
	else //만약 홀수면 답이 들어감
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

void clear() // clear 명령어를 사용하기 위한 함수
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

void quizStart(char *filename)//퀴즈 시작 함수
{
	int ran;
	int overlap = 0;
	srand(time(0));

	sigset_t st;
	sigfillset(&st);
	sigprocmask(SIG_UNBLOCK, &st, (sigset_t*)NULL);
	struct itimerval it;
	sigset(SIGALRM, handler);
	it.it_value.tv_sec = 10;// 시그널 10초 지정 , 한 문제당 10초 
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	char *name;
	FILE *rfp, *wfp;
	char buf1[BUF_SIZE];
	int n; 
	name = getlogin();// userid 
	int correct = 0;// 맞은 갯수 확인
	int cnt = 0;
	int pran[500]; // 랜덤 함수 중복 확인을 위한 배열 
	memset(pran,11,sizeof(pran));
	clear();//화면 지우기
	int i = 0;

	for(cnt; cnt<10; cnt++)
	{
		if(cnt==indexs)
			break;
		ran = rand();
		ran = ran%indexs;// 랜덤으로 index를 받아온다.
		for(i=0; i<500; i++)
		{
		if(pran[i]==ran)// 중복을 발견할 시 중복 확인 변수 1로 변경
		{
			overlap = 1;
			break;
		}
		}
		if(overlap==1)// 중복이 발생하면 다시 for문 처음으로 돌아감
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
		printf("%d번 문제 : %s\n",cnt+1,questions[ran].p);
		printf("답 : ");
		gets(reply);
		printf("******************************************************************\n");
		if(strncmp(questions[ran].r,reply,strlen(questions[ran].r)-1)==0) //만약 답과 답이 같다면 정답
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

			if((wfp = fopen("note.txt", "a+")) == NULL){
				perror("fopen: Write Error");
				exit(1);
			}

			fprintf(wfp, "%s%s",questions[cnt].p,questions[cnt].r);
			xnum++;
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


			if((wfp = fopen("note.txt", "a+")) == NULL){
				perror("fopen: Write Error");
				exit(1);
			}

			fprintf(wfp, "%s%s",questions[cnt].p,questions[cnt].r);
			xnum++;

			if(cnt!=indexs-1)
			{
				printf("다음 문제로 넘어갑니다.\n");
			}
			sleep(1);
			clear();
		}
	}
	result(name,correct,filename,cnt);
}

char *output[] = {
	"%G년 %m월 %d일 %H:%M"
};

void result(char *name, int correct, char *filename, int pnum)// 결과를 기록하기 위한 함수
{
	struct tm *tm;
	int n;
	time_t tt;
	time(&tt);
	char buf[257];
	tm = localtime(&tt);//현재 시간 저장
	FILE *fp;
	strftime(buf,sizeof(buf),output[0],tm);

	printf("************************************************\n\n");
	printf("%s 이름 : %s  정답 갯수 : %d/%d\n",buf,name,correct,pnum);
	printf("************************************************\n\n");
	if((fp= fopen("result.txt","a+")) == NULL) {
		perror(name);
		exit(1);
	}
	fprintf(fp,"%s 문제 종류 : %s 이름 : %s 정답 갯수 : %d/%d\n",buf,filename,name,correct,pnum);
	fclose(fp);
	printf("결과가 기록되었습니다.\n");

	return;
}

void questionAdd(char *file)// 원하는 문제 파일에 문제를 추가할 수 있다.
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
		printf("문제를 입력하세요(종료하려면 q 입력) :");
		gets(problem);
		if(problem[0]=='q')
			exit(1);
		fflush(stdin);
		printf("정답을 입력하세요 :");
		gets(reply);
		fflush(stdin);
		fprintf(fp,"%s\n%s\n",problem,reply);
		printf("************************************************\n\n");
		printf("문제가 추가 되었습니다.\n");
		printf("************************************************\n\n");
		sleep(1);
	}

	fclose(fp);

	return ;
}

void gradecard()/// 성적표 출력하기
{
	FILE *fp;
	int cnt=1;
	char buf[BUF_SIZE];
	int n; 
	char *name;
	name = getlogin();

	clear();
	printf("*********************************************************************************************\n\n");
	printf("                                  %s님의 성적표\n",name);

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
		printf("문제가 존재하지 않습니다.\n");
		printf("문제를 추가하고 싶다면 -a 옵션을 이용해주세요.\n");
		exit(1);
	}
	printf("****************%s 문제 입니다.****************\n\n",filename);

	for(cnt; cnt<indexs; cnt++){
		printf("%d번 문제 : %s",cnt+1,questions[cnt].p);
		printf("답 : %s\n",questions[cnt].r);
	}
	printf("***********************************************\n");
}

void modify(char *filename)// 원하는 문제를 수정할 수 있다.
{
	int cnt=0;
	int pnum=0; //문제 번호  받기a
	char newp[200]; //새로운 문제 받기 
	char newr[200]; // 새로운 답 받기
	FILE *rfp, *wfp;
	char buf[100];


	if(indexs==0)
	{
		clear();
		printf("***************************************\n\n");
		printf("문제가 존재하지 않습니다.\n");
		printf("다른 문제 파일을 입력해주세요.\n\n");
		printf("현재 입력한 파일 명은 : %s입니다.\n",filename);
		printf("***************************************\n\n");
	}


	clear();

	printf("************************************************\n\n");
	for(cnt; cnt<indexs; cnt++)
	{
		printf("%d번 문제 : %s\n", cnt+1, questions[cnt].p);
		printf("답 : %s\n", questions[cnt].r);

	}
	printf("\n\n************************************************\n");


	printf("수정하고 싶은 문제의 번호를 입력하세요(종료하려면 0 입력) :");
	scanf("%d", &pnum);
	if(pnum>indexs)
	{
		printf("\n\n************************************************\n\n");
		printf("존재하지 않는  문제입니다. 다시 입력해주세요.\n");
		printf("%d번 이내로 입력해주세요.\n",pnum);
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
	printf("%d번 문제 : %s\n", pnum, questions[pnum-1].p);//선택한 문제를 다시 출력해준다.
	printf("답 : %s\n", questions[pnum-1].r);
	printf("************************************************\n\n");
	printf("문제를 입력하세요 :");
	gets(newp);
	fflush(stdin);

	printf("답을 입력하세요 :");
	gets(newr);
	fflush(stdin);

	strcpy(questions[pnum-1].p, newp);// 바뀐 문제 구조체에 저장 
	strcpy(questions[pnum-1].r, newr);
	printf("\n\n************************************************\n\n");
	printf("문제가 수정되었습니다.*^^*\n");
	printf("************************************************\n");
	sleep(1);


	cnt = 0 ;//파일 다시 읽기 위해 0으로 초기화 


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

void note()//오답노트 출력
{
	char *name;
	int c=0; 
	name = getlogin();

	clear();
	if(indexs==0)
	{
		printf("***********************************************************************************\n");
		printf("틀린 문제가 존재하지 않습니다.\n");
		printf("시험을 보기 위해서는 -q 옵션을 이용해주세요.\n");
		printf("***********************************************************************************\n");
		exit(1);
	}

	printf("*****************************************************************************************\n");
	printf("                                  %s님의 오답노트\n",name);
	for(c; c<indexs; c++)
	{
		printf("문제 : %s\n",questions[c].p);
		printf("답 : %s\n",questions[c].r);
	}
	printf("*****************************************************************************************\n");
}

int main(int argc,char *argv[])
{
	int n;
	extern char *optarg;
	extern int optind;
	sigset_t st;
	sigfillset(&st);// 강제종료 방지
	sigprocmask(SIG_BLOCK, &st, (sigset_t *)NULL);


	while ((n = getopt(argc,argv,"a:q:rxs:m:p")) != -1) {
		switch (n) {
			case 'a': //a 옵션, 문제 추가하기 
				readfile(optarg);
				questionAdd(optarg);
				break;

			case 'q': //q 옵션, 퀴즈 보기
				readfile(optarg);
				quizStart(optarg);
				break;

			case 'r': //r 옵션, 성적 보기
				gradecard();
				break;

			case 's': //s 옵션, 공부 하기
				readfile(optarg);
				study(optarg);
				break; 

			case 'm': //m 옵션, 문제 수정 하기
				readfile(optarg);
				modify(optarg);
				break;

			case 'x': //x 옵션, 오답 노트 보기
				readfile("note.txt");
				note();
				break;

			case 'p': //p 옵션, 오답 노트로 시험보기
				readfile("note.txt");
				quizStart("note.txt"); 
		}
	}

	if(argc<2)
	{
		clear();
		printf("********************************************************\n");
		printf("********************************************************\n\n");
		printf(" -a [문제 텍스트 파일] \n");
		printf(" 문제와 답을 추가시킬 수 있습니다.\n");
		printf(" 문제 입력 시 q를 입력하면 문제 추가를 종료합니다.\n\n");
		printf(" -q [문제 텍스트 파일] \n");
		printf(" 자신의 실력을 테스트 할 수 있습니다.\n");
		printf(" 저장되어있는 문제중 랜덤으로 15문제가 선택됩니다.\n");
		printf(" 한 문제당 10초로 제한을 두며, 틀린 답 입력시 오답처리됩니다.\n");
		printf(" 15문제를 모두 풀었으면 성적표에 성적이 작성됩니다.\n\n");
		printf(" -s [문제 텍스트 파일] \n");
		printf(" 테스트 전에 문제와 답들을 보며 공부할 수 있습니다.\n\n");
		printf(" -m [문제 텍스트 파일] \n");
		printf(" 자신이 입력한 문제와 답을 수정할 수 있습니다.\n\n");
		printf(" -r \n");
		printf(" 자신의 성적을 확인할 수 있습니다.\n");
		printf(" 자신의 성적의 향상을 한눈에 확인할 수 있습니다.\n\n");
		printf(" -x \n");
		printf(" 틀렸던 문제를 한눈에 확인할 수 있습니다.\n");
		printf(" 퀴즈를 보면 자동으로 오답노트에 문자가 추가됩니다.\n\n");
		printf(" -p \n");
		printf(" 틀렸던 문제들로만 퀴즈를 할 수 있습니다.\n\n");
		printf("********************************************************\n");
		printf("********************************************************\n");
	}

	sigprocmask(SIG_UNBLOCK, &st, (sigset_t*)NULL);

	return 0;

}
