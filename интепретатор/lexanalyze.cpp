#include <iostream>
#include<cstring>
#include<stack>
#include <map>
using namespace std;

bool finident=false;
char* filename=NULL;
bool initializing = false;
bool temp_lex=false;
//bool unar=false;
string err;

template <class T, class EL> void stackget(T& st, EL& t){
	t=st.top();
	st.pop();
}
template <class T> void empty(T& st){
	while (!st.empty()) st.pop();
}


enum type_of_lex{
	LEX_PROGRAM,LEX_INT,LEX_STRING,LEX_IF,LEX_ELSE,LEX_WHILE,LEX_READ,LEX_WRITE,
	LEX_NOT,LEX_AND,LEX_OR,LEX_DO,LEX_GOTO,LEX_REAL,LEX_OBLOCK,LEX_CBLOCK,LEX_FINOP,
	LEX_COMMA, LEX_OBRACKET, LEX_CBRACKET, LEX_STAR, LEX_DIV, LEX_PLUS,
	LEX_MINUS, LEX_LESS, LEX_ABOVE, LEX_LESSE, LEX_ABOVEQ, LEX_EQ, LEX_NEQ, LEX_INIT, LEX_COLON,
	LEX_ID, LEX_NUMI, LEX_NUMR, LEX_STR,LEX_FIN,LEX_MARKER,LEX_NULL,/*тут начинаются лексемы для полиза*/ POLIZ_GO, POLIZ_FGO, POLIZ_LABEL, POLIZ_ADDRESS, LEX_UNAR_MINUS, LEX_UNAR_PLUS, LEX_PLACE

};
class Lex{
	type_of_lex t_lex;
	int v_lex;
	double v_lex_real;
	string string_lex;
	public:
	Lex(){
		t_lex=LEX_NULL;
		v_lex=0;
		v_lex_real=0;
		string_lex="/0";
	}
	Lex ( type_of_lex t , int v ) {
		t_lex = t;
		v_lex = v;
		v_lex_real=0;
		string_lex="/0";
	}
	Lex(type_of_lex t, double v){
		t_lex=t;
		v_lex=0;
		v_lex_real=v;
		string_lex="/0";
	}
	Lex(type_of_lex t, string s){
		t_lex=t;
		v_lex=0;
		v_lex_real=0;
		string_lex=s;

	}
	type_of_lex get_type () { return t_lex; }
	int get_value () { return v_lex; }
	double get_r_value(){return v_lex_real;}
	string get_s_value(){return string_lex;}
	void put_type(type_of_lex t){
		t_lex=t;
	}

	friend ostream& operator << (ostream &s, Lex l);
};

Lex temp_lex1;
Lex temp_lex2;

//ИДЕНТФИИКАТОР + ТАБЛИЦЫ МЕТОК И ИДЕНТИФИКАТОРОВ
class Ident{
	string name;
	bool declare;
	bool mark;
	type_of_lex type;
	bool assign;
	int value;
	double r_value;
	string s_value;
	public:
	int placemark;
	Ident() {mark =false; declare = false; assign = false; placemark=-1; }
	string get_name () { return name; }
	void put_name (const string n)
		{
			name=n;
		}
	bool get_mark (){return mark;}
	void put_mark(){mark =true;}
	bool get_declare () { return declare; }
	void put_declare () { declare = true; }
	type_of_lex get_type() { return type; }
	void put_type (type_of_lex t) { type = t; }
	bool get_assign () { return assign; }
	void put_assign (){ assign = true; }
	int get_value () { return value; }
	void put_value (int v){ value = v; }
	double get_r_value(){return r_value;}
	void put_r_value(double v){r_value=v;}
	string get_s_value(){return s_value;}
	void put_s_value(string s){s_value=s;}
};

class Table_mark{
	Ident *p;
	int size;
	int top;
	
	public:
	multimap<string,int> emptymarks;
	Table_mark(int max_size){
		p=new Ident[size=max_size];
		top =0;
	}
	~Table_mark(){
		delete []p;
	}
	Ident& operator[](int k){
		return p[k];
	}
	int put(const string buf);
	void copy_mark(Lex l,int pl);
	bool check_marks();
};


class Table_ident{
	Ident *p;
	int size;
	int top;
	public:
	Table_ident(int max_size){ 
		p=new Ident[size=max_size]; top=0;
	}
	~Table_ident(){delete []p;}
	Ident& operator[](int k){return p[k];}
	int put(const string buf);
};


int Table_mark::put(const string buf){
	if (top==0){
		p[top].placemark=-1;
		p[top].put_name(buf);
		p[top].put_mark();	
		top++;
	}
	else{
		for (int j=0; j<top; j++)
		if(buf==p[j].get_name()){ 
			if (p[j].get_mark()) throw true;
			else{
				p[j].put_mark();
				return j;
			}
		}
		p[top].placemark=-1;	
		p[top].put_name(buf); p[top].put_mark(); top++;
	}
	return top-1;
};

int Table_ident::put(const string buf){
	if (top==0){
		p[top].put_name(buf);	
		top++;
	}
	else{
		for (int j=0; j<top; j++)
		if(buf==p[j].get_name()) return j;
		p[top].put_name(buf); top++;
	}
	return top-1;
};
//ИДЕНТИФИКАТОР+ ТАБЛИЦЫ МЕТОК И ИДЕНТФИКАТОРОВ

//СКАНЕР
class Scanner{
	enum state{H,IDENT, NUMB,RAT, COR, COM,CD,COMEND,ALE,NEQ,STR, EQ,DELIM,ERR,FIN };
	state CS;
	FILE * fp;
	char c;
	string buf;
	void clear () {
		buf.clear();
	}
	void add(char c){
		buf.push_back(c);
	}
	int look( const string buf, string* list){
		int i=0;
		while (list[i]!="/0") {
			if (buf==list[i]) return i;
			++i;
		}
		return -1;
	}
	void gc(){
		c=fgetc(fp);
	}

	public:
	Lex getlex();
	Scanner(const char* program){
		fp=fopen(program, "r");
		CS=H;
		clear();
		gc();
	}
	static string TW [15];
	static type_of_lex words [15];
	static string TD [19];
	static type_of_lex dlms [19];

};

bool isadigit(char c){
	if ((c>='0')&&(c<='9')) return true;
	else return false;
}

bool isaletter(char c){
	if (((c>='a')&&(c<='z'))||((c<='Z')&&(c>='A'))) return true;
	else return false;
}

Table_ident TID(100);
Table_mark TM(100);
//семантический анализ
stack <int> st_int;
stack <type_of_lex> st_lex;
//семантический анализ

ostream& operator << (ostream &s, Lex l){
		if (l.t_lex==LEX_NUMR){
			s<</*"Тип лексемы "<<l.t_lex<<*/"значение лексемы:"<<l.v_lex_real<<endl;
		}
		else if (l.t_lex==LEX_STR) s/*<<"Тип лексемы "<<l.t_lex<<", значение лексемы:  "*/<<'"'<<l.string_lex<<'"'<<endl;	
		else if (l.t_lex==LEX_NUMI) s/*<<"Тип лексемы "<<l.t_lex<<", значение лексемы: "*/<<l.v_lex<<endl;
		else if(l.t_lex==LEX_ID) s<</*"Тип лексемы "<<l.t_lex<<", значение лексемы: "<<*/TID[l.v_lex].get_name()/*<<", номер в таблице идентификаторов: "<<l.v_lex*/<<endl;
		else if(l.t_lex==POLIZ_ADDRESS) s<<"Лексема - адрес переменной "<< TID[l.v_lex].get_name()<<endl;
		else if(l.t_lex==LEX_MARKER) s<<"Тип лексемы"<<l.t_lex<<", значение лексемы "<<TM[l.v_lex].get_name()<<"- метка "<<"номер в таблице меток "<<l.v_lex <<endl;
		else if(l.t_lex==LEX_UNAR_PLUS) s<< "Унарный плюс +"<<endl;
		else if (l.t_lex==LEX_UNAR_MINUS) s<<"Унарный минус -"<<endl;
		else if(l.t_lex==POLIZ_FGO) s<<"Переход по лжи !F"<<endl;
		else if (l.t_lex==POLIZ_GO) s<<"Переход по условию !"<<endl;
		else if (l.t_lex==POLIZ_LABEL) s<<"Адрес перехода в ПОЛИЗе "<<l.v_lex<<endl;
		else if (l.t_lex<=13) s/*<<"Тип лексемы "<<l.t_lex<<", значение лексемы: "*/<< Scanner::TW[l.v_lex]<<endl;
		else s/*<<"Тип лексемы "<<l.t_lex<<", значение лексемы: "*/<<Scanner::TD[l.v_lex]<<endl;
		return s;
}


Lex Scanner::getlex(){
	int d=0,j=0, numdiv=0;
	double  tail=0;
	string s;
	do{
		switch(CS){
			case H:
				if (c==' '||c=='\n'||c=='\t') gc();
				else if (isaletter(c)){
					clear();
					add(c);
					gc();
					CS=IDENT;
				}
				else if(isadigit(c)){
					d=c-'0';
					gc();
					CS=NUMB;
				}
				else if(c=='/'){
					clear();
					add(c);
					gc();
					CS=CD;
				}
				else if(c=='<'||c=='>'){
					clear(); add(c); gc(); CS=ALE;
				}
				else if(c=='!'){
					clear();
					add(c);
					gc();
					CS=NEQ;
				}
				else if(c=='"'){
					s.clear();
					gc();
					CS=STR;
				}
				else if(c=='='){
					clear();
					add(c);
					gc();
					CS=EQ;
				}
				else if(c=='@'||feof(fp)){
					CS=FIN;
				}
				else{
					CS=DELIM;
				}
			break;
			case IDENT:
				if ((isadigit(c)||isaletter(c))&&(!feof(fp))&&(!(finident))){
					add(c);
					gc();
				}
				else if(c==':'){
							j=TM.put(buf);
					gc();
					CS=H;
					finident=false;
					return Lex(LEX_MARKER,j);
				}
				else if (c==' '){
					finident=true;
					gc();
				} 
				else if ((j=look(buf,TW))>=0){ CS=H; finident=false; return Lex(words[j],j);}
				else {
					finident =false;
					j=TID.put(buf);
					CS=H;
					return Lex(LEX_ID,j);
				}
			break;
			case NUMB:
				if (isadigit(c)){
					d=d*10+c-'0';
					gc();
				}
				else if(isaletter(c)){
					CS=ERR;
				}
				else if(c=='.'){
					gc();
					CS=RAT;
				}
				else{
					CS=H;
					return Lex(LEX_NUMI,d);
				}
			break;
			case RAT:
				if (isadigit(c)){
					tail=c-'0';
					++numdiv;
					gc();
					CS=COR;
				}
				else CS=ERR;
			break;
			case COR:
				if(isadigit(c)){
					tail=tail*10+c-'0';
					++numdiv;
					gc();
				}
				else if(isaletter(c)){
					CS=ERR;
				}
				else{
					CS=H;
					for (int p=1;p<=numdiv;p++){tail=tail/10;}
					return Lex(LEX_NUMR,(double)d+tail);
				}
			break;
			case COM:
				if(c=='*'){
					gc();
					CS=COMEND;
				}
				else if(c=='@'||feof(fp)){
					CS=ERR;
				}
				else{
					gc();
				}
			break;
			case CD:
				if (c=='*'){
					clear();
					gc();
					CS=COM;
				}
				else{
					j=look(buf,TD);
					CS=H;
					return Lex(dlms[j],j);
				}
			break;
			case COMEND:
				if (c=='/'){
					gc();
					CS=H;
				}
				else if(c=='@'||feof(fp)){
					CS=ERR;
				}
				else{
					CS=COM;
				}
			break;
			case ALE:
				if(c=='='){add(c); gc(); j=look(buf,TD); CS=H; return Lex(dlms[j],j);}
				else{j=look(buf,TD); CS=H; return Lex(dlms[j],j); }
			break;
			case NEQ:
				if (c=='='){add(c); gc(); j=look(buf,TD); CS=H; return Lex(dlms[j],j);}
				else{CS=ERR;}
			break;
			case STR:
				if(c!='"'){s.push_back(c);gc();}
				else if(c=='@'||feof(fp)){
					CS=ERR;
				}
				else if(c=='"'){
					gc();
					CS=H;
					return Lex(LEX_STR,s);
				}
			break;
			case EQ:
				if(c=='='){add(c);gc();CS=H;j=look(buf,TD); return Lex(dlms[j],j);}
				else{
					CS=H;
					j=look(buf,TD); return Lex(dlms[j],j);
				}
			break;
			case DELIM:
				if(c=='+'||c=='-'||c=='{'||c=='}'||c==';'||c==','||c=='('||c==')'||c=='*'){
					clear(); add(c); gc(); j=look(buf,TD); CS=H; return Lex(dlms[j],j);
				}
				else throw c;
			break;
			case ERR:
				throw c;
			break;
			case FIN:
				fclose(fp);
				return Lex(LEX_FIN,38);
			break;
		}
	} while (1);

}
//СКАНЕР

//ПАРСЕР
class Parser{
	Scanner scan;
	Lex cur_lex;
	type_of_lex cur_type;
	type_of_lex declare_type;
	int cur_val;
	void gl(){
		cur_lex=scan.getlex();
		cur_type=cur_lex.get_type();
		cur_val=cur_lex.get_value();
	}
	//прототипы для синтаксического анализатора
	void DESCRIPTIONS();
	void DESCRIPTION();
	void IDENTIFICATOR();
	void VARIABLE();
	void CONSTANT();
	void OPERATORS();
	void OPERATOR();
	void EXPRESSION();
	void EXOR();
	void EXAND();
	void EXSIGN();
	void EXPM();
	void EXMD();
	void EXNUN();
	void EXFIN();
	//прототипы для семантического анализа
	void check_description(type_of_lex);
	void check_init();
	void check_unar_op();
	void check_op();
	void check_logic();
	void check_sign();
	void check_condition();
	public:
	void put_cur_type(type_of_lex t){
		cur_type=t;
	}
	void put_declare_type(type_of_lex p){
		declare_type=p;
	}
	type_of_lex get_declare_type(){
		return declare_type;
	}
	Parser(const char* fnm):scan(fnm),cur_lex(LEX_NULL,0){
	}
	void analyze();
};
//ПАРСЕР

//ПОЛИЗАТОР
class Poliz{
	Lex* p;
	int size;
	int free_pos;
public:
	Poliz(int max_size){
		size=max_size;
		free_pos=0;
		p=new Lex[size];
	}
	~Poliz(){
		delete[]p;
	}
	void put_lex(Lex l){
		p[free_pos++]=l;
	}
	void put_lex(Lex l, int place){
		p[place]=l;
	}
	int blank(){
		return(free_pos++);
	}
	int get_free(){
		return free_pos;
	}
	Lex& operator[](int index){
		if (index>=size){
			err="Индекс выходит за допустимые границы ПОЛИЗа\n";
			throw err;
		}
		else if (index>free_pos){
			err="Индекс выходит за доступные границы ПОЛИЗа\n";
			throw err;
		}
		else return p[index];

	}
	void print(){
		for (int i=0; i<free_pos;i++){
			cout<<i<<' '<<p[i];
		}
	}
	void INTERPRETATOR(int);
};

Poliz program(1000);
stack <Lex> st_interpret;
//ПОЛИЗАТОР


//ИНТЕРПРЕТАТОР
void Poliz::INTERPRETATOR(int i){
	int index=i;
	int val,val1,val2;
	double r_val,r_val1,r_val2;
	string s_val,s_val1,s_val2;
	Lex tl,initialize, op1,op2;
	int jmp;
	bool jmpflag=false;
	type_of_lex type;
	while(index<free_pos){
		tl=program[index];
		switch (tl.get_type()){
			case LEX_ID:
				val=tl.get_value();
				if (TID[val].get_assign()){
					if (TID[val].get_type()==LEX_INT) st_interpret.push(Lex(LEX_NUMI,TID[val].get_value()));
					else if(TID[val].get_type()==LEX_STRING) st_interpret.push(Lex(LEX_STR,TID[val].get_s_value()));
					else if(TID[val].get_type()==LEX_REAL) st_interpret.push(Lex(LEX_NUMR,TID[val].get_r_value()));
				}
				else {
					cout<<index<<' '<<TID[val].get_name();
					err="Неинициализированная переменная\n";
					throw err;
				}
			break;

			case LEX_NUMI: case LEX_NUMR: case LEX_STR:
				st_interpret.push(program[index]);
			break;

			case POLIZ_GO:
				stackget(st_interpret,tl);
				jmp=tl.get_value();
				index=jmp;
				jmpflag=true;
			break;

			case POLIZ_FGO:
				stackget(st_interpret,tl);
				jmp=tl.get_value();
				stackget(st_interpret,tl);
				if (tl.get_value()==0) {
					index=jmp;
					jmpflag=true;
				}
			break;

			case POLIZ_LABEL:
				st_interpret.push(program[index]);
			break;

			case LEX_FINOP: case LEX_COMMA:
				empty(st_interpret);
			break;

			case LEX_READ:
				stackget(st_interpret,tl);
				type=TID[tl.get_value()].get_type();
				if (type==LEX_INT){
					cin>>val;
					TID[tl.get_value()].put_value(val);
					TID[tl.get_value()].put_assign();
				}
				else if(type==LEX_REAL){
					cin>>r_val;
					TID[tl.get_value()].put_r_value(r_val);
					TID[tl.get_value()].put_assign();
				}
				else if(type==LEX_STRING){
					cin>>s_val;
					TID[tl.get_value()].put_s_value(s_val);
					TID[tl.get_value()].put_assign();
				}
			break;

			case LEX_WRITE:
				stackget(st_interpret,tl);
				if (tl.get_type()==LEX_NUMI) cout<<tl.get_value()<<endl;
				else if(tl.get_type()==LEX_NUMR) cout<<tl.get_r_value()<<endl;
				else cout<<tl.get_s_value()<<endl;
			break;

			case LEX_INIT:
				stackget(st_interpret,tl);
				stackget(st_interpret,initialize);
				val=initialize.get_value();
				if (tl.get_type()==LEX_STR){
					TID[val].put_s_value(tl.get_s_value());
					TID[val].put_assign();
					st_interpret.push(Lex(LEX_STR,tl.get_s_value()));
				}
				else if(TID[val].get_type()==LEX_INT){
					if (tl.get_type()==LEX_NUMI) TID[val].put_value(tl.get_value());
					else TID[val].put_value(tl.get_r_value());
					TID[val].put_assign();
					st_interpret.push(Lex(LEX_NUMI,TID[val].get_value()));
				}
				else if(TID[val].get_type()==LEX_REAL){
					if (tl.get_type()==LEX_NUMI) TID[val].put_r_value(tl.get_value());
					else TID[val].put_r_value(tl.get_r_value());
					TID[val].put_assign();
					st_interpret.push(Lex(LEX_NUMR,TID[val].get_r_value()));
				}
			break;

			case LEX_EQ: case LEX_NEQ: case LEX_LESS: case LEX_ABOVE:
				stackget(st_interpret,op2);
				stackget(st_interpret,op1);

				if (op2.get_type()==LEX_NUMI) val2=op2.get_value();
				else if(op2.get_type()==LEX_NUMR) r_val2=op2.get_r_value();
				else s_val2=op2.get_s_value();

				if (op1.get_type()==LEX_NUMI) val1=op1.get_value();
				else if(op1.get_type()==LEX_NUMR) r_val1=op1.get_r_value();
				else s_val1=op1.get_s_value();

				if (op2.get_type()==LEX_STR){
					if (tl.get_type()==LEX_EQ){
						if (s_val1==s_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else if(tl.get_type()==LEX_NEQ){
						if (s_val1!=s_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else if(tl.get_type()==LEX_LESS){
						if(s_val1<s_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else{
						if(s_val1>s_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
				}
				else{
					if (op2.get_type()==LEX_NUMI) r_val2=val2;
					if (op1.get_type()==LEX_NUMI) r_val1=val1;
					if (tl.get_type()==LEX_EQ){
						if(r_val1==r_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else if(tl.get_type()==LEX_NEQ){
						if(r_val1!=r_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else if(tl.get_type()==LEX_LESS){
						if(r_val1<r_val2) {
							st_interpret.push(Lex(LEX_NUMI,1));
						}
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
					else{
						if(r_val1>r_val2) st_interpret.push(Lex(LEX_NUMI,1));
						else st_interpret.push(Lex(LEX_NUMI,0));
					}
				}
			break;

			case LEX_ABOVEQ: case LEX_LESSE:
				stackget(st_interpret,op2);
				stackget(st_interpret,op1);
				if (op2.get_type()==LEX_NUMI) r_val2=op2.get_value();
				else if(op2.get_type()==LEX_NUMR) r_val2=op2.get_r_value();
				if (op1.get_type()==LEX_NUMI) r_val1=op1.get_value();
				else if(op1.get_type()==LEX_NUMR) r_val1=op1.get_r_value();
				if (tl.get_type()==LEX_ABOVEQ) {
					if (r_val1>=r_val2) st_interpret.push(Lex(LEX_NUMI,1));
					else st_interpret.push(Lex(LEX_NUMI,0)); 
				}
				else{
					if (r_val1<=r_val2) st_interpret.push(Lex(LEX_NUMI,1));
					else st_interpret.push(Lex(LEX_NUMI,0));
				}

			break;

			case LEX_PLUS:
				stackget(st_interpret,op2);
				stackget(st_interpret,op1);
				if (op2.get_type()==LEX_NUMI) val2=op2.get_value();
				else if(op2.get_type()==LEX_NUMR) r_val2=op2.get_r_value();
				else s_val2=op2.get_s_value();

				if (op1.get_type()==LEX_NUMI) val1=op1.get_value();
				else if(op1.get_type()==LEX_NUMR) r_val1=op1.get_r_value();
				else s_val1=op1.get_s_value();

				if (op2.get_type()==LEX_STR){
					st_interpret.push(Lex(LEX_STR,s_val1+s_val2));
				}
				else {
					if((op1.get_type()==LEX_NUMR)||(op2.get_type()==LEX_NUMR)){
						if (op1.get_type()==LEX_NUMI) r_val1=val1;
						if (op2.get_type()==LEX_NUMI) r_val2=val2;
						st_interpret.push(Lex(LEX_NUMR,r_val1+r_val2));
					}
					else st_interpret.push(Lex(LEX_NUMI,val1+val2));
				}

			break;

			case LEX_MINUS: case LEX_STAR: case LEX_DIV:
				stackget(st_interpret,op2);
				stackget(st_interpret,op1);
				if (op2.get_type()==LEX_NUMI) val2=op2.get_value();
				else if(op2.get_type()==LEX_NUMR) r_val2=op2.get_r_value();
				if (op1.get_type()==LEX_NUMI) val1=op1.get_value();
				else if(op1.get_type()==LEX_NUMR) r_val1=op1.get_r_value();

				if (tl.get_type()==LEX_MINUS){
					if((op1.get_type()==LEX_NUMR)||(op2.get_type()==LEX_NUMR)){
						if (op1.get_type()==LEX_NUMI) r_val1=val1;
						if (op2.get_type()==LEX_NUMI) r_val2=val2;
						st_interpret.push(Lex(LEX_NUMR,r_val1-r_val2));
					}
					else st_interpret.push(Lex(LEX_NUMI,val1-val2));
				}
				else if(tl.get_type()==LEX_STAR){
					if((op1.get_type()==LEX_NUMR)||(op2.get_type()==LEX_NUMR)){
						if (op1.get_type()==LEX_NUMI) r_val1=val1;
						if (op2.get_type()==LEX_NUMI) r_val2=val2;
						st_interpret.push(Lex(LEX_NUMR,r_val1*r_val2));
					}
					else st_interpret.push(Lex(LEX_NUMI,val1*val2));

				}
				else if(tl.get_type()==LEX_DIV){
					if((op1.get_type()==LEX_NUMR)||(op2.get_type()==LEX_NUMR)){
						if (op1.get_type()==LEX_NUMI) r_val1=val1;
						if (op2.get_type()==LEX_NUMI) r_val2=val2;
						st_interpret.push(Lex(LEX_NUMR,r_val1/r_val2));
					}
					else st_interpret.push(Lex(LEX_NUMI,val1/val2));

				}
			break;

			case LEX_UNAR_MINUS: case LEX_UNAR_PLUS:
				stackget(st_interpret,op1);
				if (op1.get_type()==LEX_NUMI){
					val1=op1.get_value();
					if (tl.get_type()==LEX_UNAR_MINUS) st_interpret.push(Lex(LEX_NUMI,-val1));
					else st_interpret.push(Lex(LEX_NUMI,val1));
				}
				else{
					r_val1=op1.get_r_value();
					if (tl.get_type()==LEX_UNAR_MINUS) st_interpret.push(Lex(LEX_NUMR,-r_val1));
					else st_interpret.push(Lex(LEX_NUMR,r_val1));
				}
			break;

			case POLIZ_ADDRESS:
				st_interpret.push(program[index]);
			break;

			default:
			break;
		}
	if (!jmpflag) {
		index++;
	}
	else jmpflag=false;
	}
}
//ИНТЕРПРЕТАТОР

//РАЗДЕЛ МЕТОДОВ СЕМАНТИЧЕСКОГО АНАЛИЗА
void Table_mark:: copy_mark(Lex l, int pl){//для копирования из TID в TM в случае goto
	string s;
	for (int j=0; j<top; j++){
		if (TM[j].get_name()==TID[l.get_value()].get_name()) {
			if (TM[j].get_mark()) program.put_lex(Lex(POLIZ_LABEL,TM[j].placemark),pl);
			else TM.emptymarks.insert(pair<string,int>(TID[l.get_value()].get_name(),pl));
			return;
		}
	}
	TM.emptymarks.insert(pair<string,int>(TID[l.get_value()].get_name(),pl));
	p[top].put_name(TID[l.get_value()].get_name()); top++;
}

void Parser::check_description(type_of_lex type){
	int i;
	while (!st_int.empty()){
		stackget(st_int,i);
		if (TID[i].get_declare()) {err ="Повторное описание"; throw err;}
		else {
			TID[i].put_declare();
			TID[i].put_type(type);
			if (TID[i].get_type()==LEX_STRING) {
			}
		}
	}
}
bool Table_mark::check_marks(){
	for (int i=0; i<top;i++){
		if (!TM[i].get_mark()){
			return false;
		}
	}
	return true;
}

void Parser:: check_init(){
	type_of_lex t1,t2;
	stackget(st_lex,t2);
	stackget(st_lex,t1);
	if ((t1==LEX_STRING)||(t2==LEX_STRING)){
		if (t1!=t2){
			err="Несоответствие типов присваивания\n";
			throw err;
		}
		else st_lex.push(LEX_STRING);
	}
	else{
			if (t1==LEX_INT) st_lex.push(LEX_INT);
			else if(t1==LEX_REAL) st_lex.push(LEX_REAL);
	}
}
void Parser::check_unar_op(){
	type_of_lex t1,t2;
	stackget(st_lex,t2);
	if (st_lex.empty()) {
		st_lex.push(t2);
		return;
	}
	else stackget(st_lex,t1);
	if ((t1!=LEX_NOT)&&(t1!=LEX_PLUS)&&(t1!=LEX_MINUS)){
		st_lex.push(t1);
		st_lex.push(t2);
		return;
	}
	if ((t1==LEX_PLUS)||(t1==LEX_MINUS)){
		if ((t2==LEX_REAL)||(t2==LEX_INT)) {
			st_lex.push(t2);
			return;
		}
		else {
			err="Несоответствие операндов в унарных операциях\n";
			throw err;
		}
	}
	else if (t1==LEX_NOT){
		if (t2!=LEX_INT){
			err="Несоответствие операндов в унарных операциях\n";
			throw err;
		}
		else st_lex.push(t2);
	}
}

void Parser::check_op(){
	type_of_lex t1,op,t2;
	stackget(st_lex,t1);
	stackget(st_lex,op);
	stackget(st_lex, t2);
	if ((t1==LEX_STRING)||(t2==LEX_STRING)){
		if (t1==t2){
			if (op!=LEX_PLUS){
				err="Ошибка в операциях со строками\n";
				throw err;
			}
			st_lex.push(t1);
		}
		else{
			err="Несоответствие типов операндов\n";
			throw err;
		}
	}
	else{
		if ((t1==t2)&&(t1==LEX_INT)) st_lex.push(LEX_INT);
		else if ((t1==t2)&&(t1==LEX_REAL)) st_lex.push(LEX_REAL);
		else st_lex.push(LEX_REAL);
	}
}
void Parser::check_sign(){
	type_of_lex t1,op,t2;
	stackget(st_lex,t1);
	stackget(st_lex,op);
	stackget(st_lex, t2);
	if ((t1==LEX_STRING)||(t2==LEX_STRING)){
		if (t1!=t2){
			err="Несоответствие типов\n";
			throw err;
		}
		if ((op==LEX_ABOVEQ)||(op==LEX_LESSE)){
			err="Неприменимые к строкам операции\n";
			throw err;
		}
		st_lex.push(LEX_INT);
	}
	else{
		st_lex.push(LEX_INT);
	}

}
void Parser::check_logic(){
	type_of_lex t1,op,t2;
	stackget(st_lex,t1);
	stackget(st_lex,op);
	stackget(st_lex, t2);
	if ((t1==t2)&&(t1==LEX_INT)){
		st_lex.push(LEX_INT);
	}
	else{
		err="Несоответствие типов операндов логических операций\n";
		throw err;
	}
}
void Parser::check_condition(){
	type_of_lex t;
	stackget(st_lex,t);
	if (t!=LEX_INT){
		err="Недопустимый тип выражения условия\n";
		throw err;
	}
	else st_lex.push(t);
}
//РАЗДЕЛ МЕТОДОВ СЕМАНТИЧЕСКОГО АНАЛИЗА

//РАЗДЕЛ АНАЛИЗА ОПИСАНИЙ, ПОЛИЗ ЕСТЬ
void Parser::CONSTANT(){
	if ((cur_type==LEX_PLUS)||(cur_type==LEX_MINUS)){
		temp_lex1=cur_lex;//ПОЛИЗ
		gl();
		if ((cur_type==LEX_NUMI)||(cur_type==LEX_NUMR)){
			if (((cur_type==LEX_NUMR)&&(get_declare_type()!=LEX_REAL))||((cur_type==LEX_NUMI)&&(get_declare_type()!=LEX_INT))) 
			{err="Неправильный тип константы в описании"; throw err;};
		program.put_lex(cur_lex);
		gl();
		}
		else throw cur_type;

		if (temp_lex1.get_type()==LEX_PLUS){//ПОЛИЗ
			program.put_lex(Lex(LEX_UNAR_PLUS,temp_lex1.get_value()));
		}
		else{
			program.put_lex(Lex(LEX_UNAR_MINUS,temp_lex1.get_value()));
		}
	}
	else if((cur_type==LEX_NUMR)||(cur_type==LEX_STR)||(cur_type==LEX_NUMI)){ 
		if (((cur_type==LEX_NUMR)&&(get_declare_type()==LEX_STRING))||((cur_type==LEX_NUMI)&&(get_declare_type()==LEX_STRING))||((cur_type==LEX_STR)&&(get_declare_type()!=LEX_STRING))) 
			{err="Неправильный тип константы в описании"; throw err;};
		program.put_lex(cur_lex);//ПОЛИЗ
		gl();
	}
	else throw cur_type;
}

void Parser::IDENTIFICATOR(){//ПОЛИЗ ОБРАБОТАН
	if (cur_type==LEX_ID){
		temp_lex1=cur_lex;
		st_int.push(cur_lex.get_value());
		gl();
		if (cur_type==LEX_INIT){
			program.put_lex(Lex(POLIZ_ADDRESS,temp_lex1.get_value()));
			temp_lex2=cur_lex;
			gl();
			CONSTANT();
			program.put_lex(temp_lex2);
			if((cur_type==LEX_COMMA)||(cur_type==LEX_FINOP)){//кладем в стек конец операндов
				program.put_lex(cur_lex);
			}
		}
	}
	else throw cur_type;

}


void Parser:: VARIABLE(){
	IDENTIFICATOR();
}

void Parser:: DESCRIPTION(){
	gl();
	VARIABLE();
	while(cur_type==LEX_COMMA){
		gl();
		VARIABLE();
	}
}


void Parser:: DESCRIPTIONS(){
	while ((cur_type==LEX_INT)||(cur_type==LEX_STRING)||(cur_type==LEX_REAL)){
		put_declare_type(cur_type);
		DESCRIPTION();
		check_description(declare_type);
		if (cur_type!=LEX_FINOP) throw cur_type;
		else {
			gl();
		}
	}
}
//РАЗДЕЛ АНАЛИЗА ОПИСАНИЙ, ПОЛИЗ ЕСТЬ

//ОПИСАНИЕ ВЫРАЖЕНИЯ+ ПОЛИЗ
void Parser::EXFIN(){
	Lex tl;
	if ((cur_type==LEX_STR)||(cur_type==LEX_NUMI)||(cur_type==LEX_NUMR)||(cur_type==LEX_ID)){
		tl=cur_lex;
		if (cur_type==LEX_STR) st_lex.push(LEX_STRING);
		else if (cur_type==LEX_NUMI) st_lex.push(LEX_INT);
		else if (cur_type==LEX_NUMR) st_lex.push(LEX_REAL);
		else {
			if (!TID[cur_val].get_declare()){
				err="Неописанная переменная\n";
				throw err;
			}
			st_lex.push(TID[cur_val].get_type());
		}
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		program.put_lex(tl);
	}
	else if (cur_type==LEX_OBRACKET){
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXPRESSION();
		if (cur_type==LEX_CBRACKET){
			if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		}
		else throw cur_type;
	}
	else {
		throw cur_type;
	}
}
void Parser::EXNUN(){
	Lex tl;
	int pl1,pl2,pl3;
	bool unar=false;
	if((cur_type==LEX_NOT)||(cur_type==LEX_MINUS)||(cur_type==LEX_PLUS)){
		tl=cur_lex;
		unar=true;
		st_lex.push(cur_type);
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
	}
	//сюда необходимо сделать вставку на проверку not или унарного минуса
	EXFIN();
	if (unar) {
		check_unar_op();
		unar=false;
		if (tl.get_type()==LEX_PLUS){//ПОЛИЗ
			program.put_lex(Lex(LEX_UNAR_PLUS,tl.get_value()));
		}
		else if(tl.get_type()==LEX_MINUS){
			program.put_lex(Lex(LEX_UNAR_MINUS,tl.get_value()));
		}
		else {
			pl1=program.blank();
			program.put_lex(Lex(POLIZ_FGO,0));
			program.put_lex(Lex(LEX_NUMI,0));
			pl2=program.blank();
			program.put_lex(Lex(POLIZ_GO,0));
			program.put_lex(Lex(LEX_NUMI,1));
			pl3=program.get_free();
			program.put_lex(Lex(POLIZ_LABEL,pl3-1),pl1);
			program.put_lex(Lex(POLIZ_LABEL,pl3),pl2);
		}
	}
}
void Parser::EXMD(){
	Lex tl;
	EXNUN();
	while((cur_type==LEX_STAR)||(cur_type==LEX_DIV)){
		tl=cur_lex;
		st_lex.push(cur_type);
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXNUN();
		check_op();
		program.put_lex(tl);
	}
}
void Parser::EXPM(){
	Lex tl;
	EXMD();
	while((cur_type==LEX_PLUS)||(cur_type==LEX_MINUS)){
		tl=cur_lex;
		st_lex.push(cur_type);
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXMD();
		check_op();
		program.put_lex(tl);
	}
}
void Parser::EXSIGN(){
	Lex tl;
	EXPM();
	while ((cur_type==LEX_LESS)||(cur_type==LEX_ABOVE)||(cur_type==LEX_LESSE)||(cur_type==LEX_ABOVEQ)||(cur_type==LEX_EQ)||(cur_type==LEX_NEQ)){
		tl =cur_lex;
		st_lex.push(cur_type);
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXPM();
		check_sign();
		program.put_lex(tl);
	}
}
void Parser::EXAND(){
	int pl1,pl2,pl3,pl4;
	EXSIGN();
	while(cur_type==LEX_AND){
		st_lex.push(cur_type);
		pl1=program.blank();
		program.put_lex(Lex(POLIZ_FGO,0));
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXSIGN();
		check_logic();
		pl2=program.blank();
		program.put_lex(Lex(POLIZ_FGO,0));
		program.put_lex(Lex(LEX_NUMI,1));
		pl3=program.blank();
		program.put_lex(Lex(POLIZ_GO,0));
		program.put_lex(Lex(LEX_NUMI,0));
		pl4=program.get_free();
		program.put_lex(Lex(POLIZ_LABEL,pl4-1),pl1);
		program.put_lex(Lex(POLIZ_LABEL,pl4-1),pl2);
		program.put_lex(Lex(POLIZ_LABEL,pl4),pl3);

	}
}
void Parser::EXOR(){
	int pl1, pl2,pl3,pl4,pl5;
	EXAND();
	while (cur_type==LEX_OR){
		st_lex.push(cur_type);
		pl1=program.blank();
		program.put_lex(Lex(POLIZ_FGO,0));
		program.put_lex(Lex(LEX_NUMI,1));
		pl2=program.blank();
		program.put_lex(Lex(POLIZ_GO,0));
		if (temp_lex){
			cur_lex=temp_lex2;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			temp_lex=false;
		}
		else gl();
		EXAND();
		check_logic();
		pl3=program.blank();//РАССТАНОВКА АДРЕСОВ ПЕРЕХОДА В ПОЛИЗЕ
		program.put_lex(Lex(POLIZ_FGO,0));
		program.put_lex(Lex(LEX_NUMI,1));
		pl4=program.blank();
		program.put_lex(Lex(POLIZ_GO,0));
		program.put_lex(Lex(LEX_NUMI,0));
		pl5=program.get_free();
		program.put_lex(Lex(POLIZ_LABEL,pl2+2),pl1);
		program.put_lex(Lex(POLIZ_LABEL,pl5),pl2);
		program.put_lex(Lex(POLIZ_LABEL,pl5-1),pl3);
		program.put_lex(Lex(POLIZ_LABEL,pl5),pl4);
	}
}
//ОПИСАНИЕ ВЫРАЖЕНИЯ+ПОЛИЗ
//РАЗДЕЛ ОПИСАНИЯ ОПЕРАТОРОВ

void Parser::EXPRESSION(){
	Lex tl;
	if (cur_type==LEX_ID){
		temp_lex1=cur_lex;
		if(!TID[cur_val].get_declare()){
			err="Неописанная переменная\n";
			throw err;
		}
		gl();
		if (cur_type==LEX_INIT){
			tl=cur_lex;
			program.put_lex(Lex(POLIZ_ADDRESS,temp_lex1.get_value()));//кладем адрес присваивания в ПОЛИЗ
			st_lex.push(TID[temp_lex1.get_value()].get_type());
			gl();
			EXPRESSION();
			check_init();
			program.put_lex(tl);//кладем присваивание в ПОЛИЗ

		}
		else{
			temp_lex=true;
			temp_lex2=cur_lex;
			cur_lex=temp_lex1;
			cur_type=cur_lex.get_type();
			cur_val=cur_lex.get_value();
			EXOR();
		}
	}
	else {
		EXOR();
	}//ПОЛИЗ ЕСТЬ
}

void Parser::OPERATOR(){
	if (cur_type==LEX_IF){//семантика - check, ПОЛИЗ - CHECK
		int pl1,pl2,pl3,pl4;
		gl();
		if (cur_type==LEX_OBRACKET){
			gl();
			EXPRESSION();
			check_condition();
			pl1=program.blank();
			program.put_lex(Lex(POLIZ_FGO,0));
			if (cur_type==LEX_CBRACKET){
				gl();
				OPERATOR();
				pl2=program.blank();
				program.put_lex(Lex(POLIZ_GO,0));
				if (cur_type==LEX_ELSE){
					gl();
					pl3=program.get_free();
					OPERATOR();
					pl4=program.get_free();
					program.put_lex(Lex(POLIZ_LABEL,pl3),pl1);
					program.put_lex(Lex(POLIZ_LABEL,pl4),pl2);
				}
				else throw cur_type;
			}
			else throw cur_type;
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_WHILE){// семантика - check, ПОЛИЗ - CHECK
		int pl1,pl2,pl3,pl4;
		gl();
		if (cur_type==LEX_OBRACKET){
			gl();
			pl1=program.get_free();
			EXPRESSION();
			check_condition();
			pl2=program.blank();
			program.put_lex(Lex(POLIZ_FGO,0));
			if (cur_type==LEX_CBRACKET){
				gl();
				OPERATOR();
			}
			else throw cur_type;
			pl3=program.blank();
			program.put_lex(Lex(POLIZ_GO,0));
			pl4=program.get_free();
			program.put_lex(Lex(POLIZ_LABEL,pl4),pl2);
			program.put_lex(Lex(POLIZ_LABEL,pl1),pl3);
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_READ){// семантика - check, отсутствует bool => нет ограничений, ПОЛИЗ - check
		Lex tl=cur_lex;
		gl();
		if (cur_type==LEX_OBRACKET){
			gl();
			if (cur_type==LEX_ID){
				program.put_lex(Lex(POLIZ_ADDRESS,cur_lex.get_value()));
				gl();
				if (cur_type==LEX_CBRACKET){
					gl();
					if (cur_type==LEX_FINOP){
						program.put_lex(tl);
						gl();
					}
					else throw cur_type;
				}
				else throw cur_type;
			}
			else throw cur_type;
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_WRITE){//проверка не требуется, поскольку write допускает любые типы, ПОЛИЗ - check
		Lex tl=cur_lex;
		gl();
		if (cur_type==LEX_OBRACKET){
			gl();
			EXPRESSION();
			program.put_lex(tl);
			while (cur_type==LEX_COMMA){
				gl();
				EXPRESSION();
				program.put_lex(tl);
			}
			if (cur_type==LEX_CBRACKET){
				gl();
				if (cur_type==LEX_FINOP){
					gl();
				}
				else throw cur_type;
			}
			else throw cur_type;
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_OBLOCK){//CHECK
		gl();
		OPERATORS();
	}
	else if (cur_type==LEX_DO){//семантика - check, полиз - check
		int pl1,pl2, pl3,pl4;
		gl();
		pl1=program.get_free();
		OPERATOR();
		if (cur_type==LEX_WHILE){
			gl();
			if (cur_type==LEX_OBRACKET){
				gl();
				EXPRESSION();
				check_condition();
				pl2=program.blank();
				program.put_lex(Lex(POLIZ_FGO,0));
				pl3=program.blank();
				program.put_lex(Lex(POLIZ_GO,0));
				pl4=program.get_free();
				program.put_lex(Lex(POLIZ_LABEL,pl4),pl2);
				program.put_lex(Lex(POLIZ_LABEL,pl1),pl3);
				if (cur_type==LEX_CBRACKET){
					gl();
					if (cur_type==LEX_FINOP){
						gl();
					}
					else throw cur_type;
				}
				else throw cur_type;
			}
			else throw cur_type;
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_GOTO){//семантика - check, таблицы проработаны, полиз CHECk
		int pl;
		gl();
		if (cur_type==LEX_ID){
			pl=program.blank();
			program.put_lex(Lex(POLIZ_GO,0));
			TM.copy_mark(cur_lex,pl);//смотрим, была ли описана метка ранее, если нет - добавляем её в таблицу, не меняя mark
			gl();
			if (cur_type==LEX_FINOP){
				gl();
			}
			else throw cur_type;
		}
		else throw cur_type;
	}
	else if(cur_type==LEX_MARKER){//семантика - check, нет необходимости проверять соответствие типов, поскольку лексема-метка сразу заносится в таблицу меток
		int pl;

		pl =program.get_free();
		/*if ((TM[cur_val].placemark!=pl)&&(TM[cur_val].placemark!=-1)){
			program.put_lex(Lex(POLIZ_LABEL,pl),TM[cur_val].placemark);
			TM[cur_val].placemark=pl;
		}
		else{
			TM[cur_val].placemark=pl;
		}*/
		/*TM[cur_val].placemark=pl;
		while(!TM.st_plmark.empty()){
			pl=TM.st_plmark.top();
			TM.st_plmark.pop();
			program.put_lex(Lex(POLIZ_LABEL,TM[cur_val].placemark),pl);
		}*/
		TM[cur_val].placemark=pl;
		multimap <string,int>:: iterator iter;
		while (TM.emptymarks.count(TM[cur_val].get_name())!=0){
			iter=TM.emptymarks.find(TM[cur_val].get_name());
			program.put_lex(Lex(POLIZ_LABEL,TM[cur_val].placemark),(*iter).second);
			TM.emptymarks.erase(iter);

		}
		gl();
		OPERATOR();
	}
	else {//ПОЛИЗ + КОНТЕКСТ - ЕСТЬ
		EXPRESSION();
		if (cur_type==LEX_FINOP){
			program.put_lex(cur_lex);
			gl();
		}
		else throw cur_type;
	}

}


void Parser:: OPERATORS(){
	while(cur_type!=LEX_CBLOCK){
		OPERATOR();
		if (cur_type==LEX_FIN) throw cur_type;
	}
	gl();
}


//РАЗДЕЛ ОПИСАНИЯ ОПЕРАТОРОВ

void Parser:: analyze(){
	gl();
	if (cur_type!=LEX_PROGRAM){
		throw cur_type;
	}
	else{
		gl();
		if (cur_type==LEX_OBLOCK){
			gl();
			DESCRIPTIONS();
			if (cur_type!=LEX_CBLOCK) OPERATORS();
			gl();
			if ((cur_type==LEX_FIN)&&(TM.check_marks())) {
				cout<<"Программа написана корректно"<<endl;
				program.print();
				program.INTERPRETATOR(0);
			}
			else if (!TM.check_marks()){
				err="Встретились необъявленные метки\n";
				throw err;
			}
			//else throw cur_type;
		}
		else throw cur_type;
	}

}

//ПАРСЕР


string Scanner::TW []={"program","int", "string", "if", "else", "while","read","write","not","and","or","do","goto","real","/0"};
string Scanner::TD []={"{","}",";",",","(",")","*","/","+","-","<",">","<=",">=","==","!=","=","/0"};
type_of_lex Scanner::words[]={LEX_PROGRAM,LEX_INT,LEX_STRING,LEX_IF,LEX_ELSE,LEX_WHILE,LEX_READ,LEX_WRITE,LEX_NOT,LEX_AND,LEX_OR,LEX_DO,LEX_GOTO,LEX_REAL,LEX_NULL};
type_of_lex Scanner::dlms[]={LEX_OBLOCK,LEX_CBLOCK,LEX_FINOP,LEX_COMMA, LEX_OBRACKET, LEX_CBRACKET, LEX_STAR, LEX_DIV, LEX_PLUS,
	LEX_MINUS, LEX_LESS, LEX_ABOVE, LEX_LESSE, LEX_ABOVEQ, LEX_EQ, LEX_NEQ, LEX_INIT,  LEX_NULL};


int main(int argc, char* argv[]){
	/*type_of_lex t=LEX_NULL;
	Lex l(LEX_NULL,1);
	Scanner scan(argv[1]);
	try{
		while (t!=LEX_FIN){
			l=scan.getlex();
			t=l.get_type();
			if(t!=LEX_FIN) cout<<l;
		}
	}
	catch(bool){
		cout<<"Ошибка, повторное описание метки"<<endl;
		return 0;
	}
	catch(char c){
		cout<<"Встретилась лексема, не принадлежащая языку"<<endl;
		return 0;
	}*/
	Parser prs(argv[1]);
	try{
		prs.analyze();
	}
	catch (string s){
		cout<<s<<endl;
	}
	catch(char c){
		cout<<"Встретилась лексема, не принадлежащая языку"<<endl;
		return 0;
	}
	catch(type_of_lex c){
		cout<<"Программа написана некорректно"<<endl;
	}
	catch(bool){
		cout<<"Ошибка, повторное описание метки"<<endl;
		return 0;
	}
	return 0;
}