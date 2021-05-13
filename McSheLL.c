#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <signal.h>
#define N 5
int col=0,col1=0,status=0;
int fdstatus[2]={0};
char **STRFULL;

void sigconv(int sig){
    printf("Impossible to execute operation\n");
    return;
}

void countcol(int*col, char** str){
    (*col)=0;//подсчёт количества лексемм
    int i=0;
    while(str[i]!=NULL){
        ++i;
    }
    (*col)=i;
    return;
}

void clean(char** str){
    int i=0;
    if (str!=NULL){
        while (i<col){
            if (str[i]!=NULL) free(str[i]);
            ++i;
        }
        free(str);
    }
}

void errproccess(char*** str){
    printf("Impossible to execute redirection\n");
    clean(*str);
    exit(1);
}

int checkconv(char** str){
    int f=0,i=0;
    while ((str[i]!=NULL)&&(f==0)){
        if (strcmp(str[i],"|")==0) f=1;
        i++;
    }
    return(f);

}

int checkredir(char** str){
    int i=0,f=0;
    while (str[i]!=NULL){
        if ((strcmp((str[i]),">")==0)||(strcmp(str[i],">>")==0)||(strcmp(str[i],"<")==0)){
            f=1;
            break;
        }
        ++i;
    }
    return(f);
}


void countredir(int* colin,int* colout,int* coloutend,char** str){
    int i=0;
    while (str[i]!=NULL){
        if (strcmp(str[i],">")==0) ++(*colout);
        else if (strcmp(str[i],">>")==0) ++(*coloutend);
        else if (strcmp(str[i],"<")==0) ++(*colin);
        ++i;
    }
    return;
}

void execredir(char** str,int* pos){
    int coloutend=0,colout=0,colin=0,i=0;
    countredir(&colin,&colout,&coloutend,str);
            while ((colin>0)||(coloutend>0)||(colout>0)){
                int fd=0;
                if (strcmp(str[i],">")==0){
                    colout-=1;
                    if (str[i+1]!=NULL) fd=open(str[i+1],O_WRONLY|O_CREAT|O_TRUNC,0666);
                    if ((fd==-1)||(str[i+1]==NULL)){
                        errproccess(&str);
                    }
                    dup2(fd,1);
                    close(fd);
                }

                else if (strcmp(str[i],">>")==0){
                    coloutend-=1;
                    if (str[i+1]!=NULL) fd=open(str[i+1],O_APPEND|O_CREAT|O_WRONLY,0666);
                    if ((fd==-1)||(str[i+1]==NULL)){
                        errproccess(&str);
                    }
                    dup2(fd,1);
                    close(fd);

                }
                else if (strcmp(str[i],"<")==0){
                    colin-=1;
                    if (str[i+1]!=NULL) fd=open(str[i+1],O_RDONLY);
                    if ((fd==-1)||(str[i+1]==NULL)){
                        errproccess(&str);
                    }
                    dup2(fd,0);
                    close(fd);
                }
            ++i;
            }
            if ((str[i+1]!=NULL)&&(pos!=NULL)) (*pos)=i+1;    
}




void execute(char** str,int pos,int* fd,int color,int fconv,int flagphone){
    int stat,er,fin;
    if (strcmp(str[pos],"cd")==0){//ветка выполнения команды cd
        if (str[pos+1]==NULL) er=chdir(getenv("HOME"));
        else er=chdir(str[pos+1]);
        if (er==-1) printf("Impossible to execute operation\n");
        if (er==-1) {
            er=1;
            write(fdstatus[1],&er,sizeof(int));
        }
        else{
            er=0;
            write(fdstatus[1],&er,sizeof(int));
        }
    }
    else{//ветка выполнения обычной команды, отличной от cd, и перенаправления
            int pid;
            pid=fork();
            if (pid==0){
                if (flagphone==0) signal(SIGINT,SIG_DFL);
                if (fconv==1){
                    if (color>=0) dup2(fd[1],1);
                    close(fd[0]);
                    close(fd[1]);
                }
                if (fconv==0){//если перенаправление встречается в обычной команде или просто команда обычная
                    /*if (flagphone==1){
                        while(getppid()!=1);
                    }*/
                    if (checkredir(str)==0) er=execvp(str[0],str);
                    else{
                        if ((strcmp((str[0]),">")==0)||(strcmp(str[0],">>")==0)||(strcmp(str[0],"<")==0)){
                            int posredir=0;
                            execredir(str,&posredir);
                            if (posredir!=0) er=execvp(str[posredir],&(str[posredir]));
                            else er=-1;
                        }
                        else{
                            int i=0;
                            while ((strcmp((str[i]),">")!=0)&&(strcmp(str[i],">>")!=0)&&(strcmp(str[i],"<")!=0)){
                                ++i;
                            }
                            execredir(str,NULL);
                            str[i]=NULL;
                            er=execvp(str[0],str);
                        }
                    }
                }
                else er=execvp(str[pos],str+(pos*sizeof(char)));
                if (er==-1) {
                    if (fconv==1) {
                        kill(getppid(),SIGUSR1);
                    }
                    else printf("Impossible to execute operation\n");
                    clean(STRFULL);
                    exit(1);

                }

                
            }
            else
            {


                if (fconv==1){
                    dup2(fd[0],0);
                    close(fd[0]);
                    close(fd[1]);
                }
                if (pid==-1) {
                    printf("fork error\n");
                    if (fconv==1) kill(getppid(),SIGUSR1);
                }
                waitpid(pid,&stat,0);
                fin = WIFEXITED(stat);
                //if (WEXITSTATUS(stat)) fin=0;
                if ((color<0)||(fconv==0)) {
                    if (fin!=0){
                        if (WEXITSTATUS(stat)) fin=1;
                        else fin=0;
                        write(fdstatus[1],&fin,sizeof(int));
                    }
                    else{
                        fin=1;
                        write(fdstatus[1],&fin,sizeof(int));
                    }
                }
            }
        }
    }


void conv(char** str,int flagphone){
    int pid=fork();
    if (pid==0){
        /*if (flagphone==1){
            while(getppid()!=1);
        }*/
        if (flagphone==0) signal(SIGINT,SIG_DFL);
        int posredir=0;
        if (checkredir(str)){
            execredir(str,&posredir);
        }
        int fd[2],i=0,posmas=posredir,color=0;
        while (str[i]!=NULL){
            if (strcmp(str[i],"|")==0) color+=1;
            ++i;
        }
        if (strcmp(str[posmas],"|")==0){
            printf("Wrong input of conveyor\n");
            clean(str);
            exit(1);
        }
        while (posmas<=col1){
            i=0;
            if (color>0){
                while (strcmp(str[posmas+i],"|")!=0){
                    ++i;
                }
                if ((posmas+i)==(col1-1)){
                    printf("Wrong input of conveyor\n");
                    break;
                }
                free(str[posmas+i]);
                str[posmas+i]=NULL;
                color-=1;
            }
            else{
                while (str[posmas+i]!=NULL){
                    ++i;
                }
                color-=1;
            }
            pipe(fd);
            signal(SIGUSR1,sigconv);
            execute(str,posmas,fd,color,1,flagphone);
            posmas=posmas+i+1;
        }
        clean(STRFULL);
        while (wait(NULL)!=-1);
        exit(0); 
    }
    else {
        if (pid==-1) printf("Impossible to execute conveyor\n");
        else {
            /*if (flagphone==1) {
                clean(str);
                exit(1);
            }*/
            wait(NULL);
        }
    }
}

void execdif(char** str,int flagphone){
    countcol(&col1,str);
    if (flagphone==1){
        if (checkconv(str)) conv(str,1);
        else execute(str,0,NULL,0,0,1);
    }
    else{
        if (checkconv(str)) conv(str,0);
        else execute(str,0,NULL,0,0,0); 
    }
}


/*int checkphone(char** str){
    int i=0,flag=0;
    while (str[i]!=NULL){
        if (strcmp(str[i],"&")==0){
            flag=1;
            free(str[i]);
            str[i]=NULL;
            col-=1;
            break;
        }
        ++i;
    }
    return flag;
}*/

void parsewithconditions(char** str,int flagphone){//парсим стрчоку по && и ||
    int i=0, curpos =0,statexec=0,flagsym=0,flagend=0,flagexecold=0,flagexecnew=0;
    while (str[i]!=NULL){
        if ((strcmp(str[i],"&&")==0)||(strcmp(str[i],"||")==0)){
            flagsym=1;
            flagend+=1;//количество символов && и ||
        }
        ++i;
    }
    i=0;
    if (flagsym==0) {
        if (flagphone==1) execdif(str,1);
        else execdif(str,0);
        read(fdstatus[0],&statexec,sizeof(int));
    }
    else{
        while(flagend>=0){
            flagexecold=flagexecnew;
            while(str[i]!=NULL){
                if ((strcmp(str[i],"&&")==0)||(strcmp(str[i],"||")==0)) break;
                else ++i;
            }
            if (str[i]==NULL) flagend-=1;
            else{
                if (strcmp(str[i],"&&")==0){
                    flagend-=1;
                    free(str[i]);
                    str[i]=NULL;
                    flagexecnew=0;
                }
                else {
                    flagend-=1;
                    free(str[i]);
                    str[i]=NULL;
                    flagexecnew=1;
                }
            }

            if (statexec==flagexecold){
                if (flagphone==1) execdif(&(str[curpos]),1);
                else execdif(&(str[curpos]),0);
                read(fdstatus[0],&statexec,sizeof(int));
            }
            curpos=i+1;
            i=curpos;
        }
    }

}

void execphone(char** str){
    int pid=0;
    pid=fork();//запускаем процесс - сын
    if (pid==0){
        
        int pid1=0;
        pid1=fork();//запускаем процесс - внук (процесс для фона)
        if (pid1==0){
            int fd=0;
            fd=open("/dev/null",O_RDONLY);
            dup2(fd,0);
            close(fd);
            while (getppid()!=1);
            countcol(&col1,str);
            parsewithconditions(str,1);
            clean(STRFULL);
            exit(1);
        }
        else {
            if (pid1==-1) printf("Fork error\n");
            clean(STRFULL);
            exit(1);
        }
    }
    else while(wait(NULL)!=-1);
}

int checkparseouter(char** str){//подсчитываем количество команд для разбиения
    int i=0,res=0;
    while (str[i]!=NULL){
        if ((strcmp(str[i],";")==0)||(strcmp(str[i],"&")==0)){
            ++res;
        }
        ++i;
    }
    if ((strcmp(str[i-1],"&")==0)||(strcmp(str[i-1],";")==0)) return (res-1);
    else return (res);
}




void parse(char** str){//парсим строчку по & и ;
    /*countcol(&col,str);
    if (checkphone(str)){
       execphone(str);
    }
    else{
         if (checkconv(str)) conv(str,0);
         else execute(str,0,NULL,0,0,0);
    }
    clean(str);*/ 
    int numberofexecutions=0,curpos=0,flaglast=0;
    pipe(fdstatus);
    numberofexecutions=checkparseouter(str);//проверка общих итераций по ; и &
    countcol(&col,str);
    STRFULL=str;
    for (int i=0;i<numberofexecutions+1;i++){
        int j=curpos;
        flaglast=0;
        while(str[j]!=NULL){
            if ((strcmp(str[j],";")==0)||(strcmp(str[j],"&")==0)){
                if (strcmp(str[j],"&")==0){
                    free(str[j]);
                    str[j]=NULL;
                    if (str[curpos]!=NULL)execphone(&(str[curpos]));
                    curpos=j+1;
                }
                else{
                    free(str[j]);
                    str[j]=NULL;
                    if (str[curpos]!=NULL) parsewithconditions(&(str[curpos]),0);   
                    curpos=j+1;
                }
                flaglast=1;
                break;  
            }
            ++j;
        }
        if (flaglast==0){
            parsewithconditions(&(str[curpos]),0);
        } 
    }
    clean(STRFULL);
    close(fdstatus[1]);
    close(fdstatus[0]);
}





//БЛОК ВВОДА И ВЫВОДА ИНФОРМАЦИИ
void err(char*** str){
    printf("Impossible to allocate memory\n");
    clean(*str);
    exit(1);
}



/*void outp(char** str){
    int i=0;
    if (str!=NULL){
        while (str[i]!=NULL) {
            printf("%s\n",str[i]);
        i++;
        }
    }
    return;
}*/

char* inchar(int* a, char*** str){
    int count=0,c=*a,i;
    char *s=NULL;
    while ((c==*a)&&(count<2)){
                ++count;
                c=getchar();
            }
    s=malloc(count+1);
    if (s==NULL) err(str);
    for(i=0;i<count;i++){
        s[i]=*a;
    }
    s[count]='\0';
    *a=c;
    return(s);
}



void insert(char* s, char*** str){
    int i=0;
    if (col==0) {
        *str=malloc(sizeof(char*)*N);
        col=N;
        if (*str==NULL) {
            free(s);
            err(str);
        }
        *str[0]=NULL;
    }
    while ((*str)[i]!=NULL){
        ++i;
    }
    if (i==col-1) {
        col+=N;
        *str=(char**)realloc(*str,col*sizeof(char*));
    }
    if (*str==NULL) err(str);
    (*str)[i]=s;
    (*str)[i+1]=NULL;
    return;
}


void inputstr(char*** str, int* endofinp,int fileinputflag){
    int c='a',fnewline=0,length,fcov=0;
    char *s=NULL;
        int M=N;
    while ((fnewline==0)&&(*endofinp==0)){
        length=0;
        M=N;
        if ((c!='|')&&(c!='&')&&(c!='<')&&(c!='>')&&(c!=';')&&(c!=')')&&(c!='(')) c=getchar();
        if ((c=='|')||(c=='&')||(c=='>')){
            s=inchar(&c,str);
            insert(s,str);
            s=NULL;
        }
        else if ((c==';')||(c=='<')){
            s=malloc(2);
            if (s==NULL) err(str);
            s[0]=c;
            s[1]='\0';
            c=getchar();
            insert(s,str);
            s=NULL;
        }
        else if ((c=='(')||(c==')')){
            s=malloc(2);
            if (s==NULL) err(str);
            s[0]=c;
            s[1]='\0';
            insert(s,str);
            s=NULL;
            c=getchar();
        }
        if ((c!=EOF)&&(c!='\n')&&(c!=' ')&&(c!='&')&&(c!='>')&&(c!=';')&&(c!='<')&&(c!='|')&&(c!='(')&&(c!=')')) {
            s=malloc(M);
            if (s==NULL) err(str);
        }
        while (((c!=EOF)&&(c!='\n')&&(c!=' ')&&(c!='&')&&(c!='>')&&(c!=';')&&(c!='<')&&(c!='|')&&(c!='(')&&(c!=')'))||(fcov==1)){
            if (c=='"') {
                fcov=fcov^1;
                c=getchar();
            }
            else{
                if((fcov==1)&&((c==EOF)||(c=='\n'))){
                    if (fileinputflag==0) printf("Enter closing cov\n");
                    if (fileinputflag==1){
                        printf("Abscence of closing cov in file\n");
                        clean(*str);
                        free(s);
                        exit(1);
                    }
                    c=getchar();
                }
                else{
                    length++;
                    if (length==M-1) s=realloc(s,M+=N);
                    if (s==NULL) err(str);
                    s[length-1]=c;
                    c=getchar();
                }
            }
        }
        if ((s!=NULL)&&(length!=0)){
            s[length]='\0';
            insert(s,str);
            s=NULL;
        }
        if (c==EOF) *endofinp=1;
        else if (c=='\n') fnewline=1;
    }

}

//БЛОК ВВОДА И ВЫВОДА ИНФОРМАЦИИ

int main(int argc, char* argv[]){
    char **str=NULL;
    signal(SIGINT,SIG_IGN);
    int fileinp=0,endofinp=0, fileinputflag=0;
    if (argc==2){
        if ((fileinp=open(argv[1],O_RDONLY))==-1){
            printf("Impossible to open file, continue input in command line\n");
        }
        else {
            dup2(fileinp,0);
            close(fileinp);
            fileinputflag=1;
        }
    }
    else if(argc>2){
        printf("Impossible to execute program with such number of parametres\n");
        exit(1);
    }

    while (endofinp==0){
        col=0;
        printf("Please, input data\n");
        inputstr(&str,&endofinp,fileinputflag);
        if (str!=NULL){
            parse(str);
        }
        str=NULL;
    } 
    return 0;
}
