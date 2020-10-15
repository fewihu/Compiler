#ifndef INTRPRTR
#define INTRPRTR

typedef struct morph{
	
	int mc;
	union{
		double dval;
		char cval;
	};
}MORPHEM;

double expr(void);
double fac(void);
double term(void);

enum mcodes{mempty, mop, mdbl};

#endif
