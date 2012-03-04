#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926

double function1(double x){//evaluate function 1 at x
	return (cos(x*x)+sin(x*x*x));
}

double function2(double x){
	double ins=PI*x*x/2;
	return cos(ins);
}

double function3(double theta,double x){
	double si=sin(theta);
	double p1=x*x*si*si;
	double p2=sqrt(1-p1);
	return (1/p2);
}

double call_function(int function,double arg){
	if(function==1){return function1(arg);}
	if(function==2){return function2(arg);}
	if(function==3){return function3(arg,0.5);}//using 0.5 for the elliptic modulous
	//should never get here
	printf("Unsupported function number: %d\n",function);
	exit(2);
}

double simpsons_approx(int fun,double lo,double hi){//do a simpson's approximation on the appropriate function over the specified interval
	double p1=(hi-lo)/6.0;
	double pLo=call_function(fun,lo);
	double pAv=4.0 * call_function(fun,(lo+hi)/2 );
	double pHi=call_function(fun,hi);
	return p1*( pLo+pAv+pHi );
}

/*
Simpsons rule:
integral from a to b of f(x) is approximately
((b-a)/6)* [ f(a) + f(b) + 4*f((a+b)/2) ]
*/

double compound_simpsons(int fun,double lo,double hi,int intervals,bool printIntervals){
	//break up the space from lo to hi into (intervals) number of intervals
	//then call simpsons_approx for each interval
	double range=hi-lo;
	double step=range/intervals;
	double total=0.0;//running total for all approximations
	double intLo,intHi,tmp;
	for(int i=0;i<intervals;i++){
		intLo=lo+step*i;
		intHi=lo+step*(i+1);
		tmp=simpsons_approx(fun,intLo,intHi);
		if(printIntervals){printf("Interval %d from %f to %f returns %f\n",i+1,intLo,intHi,tmp);}
		total+=tmp;
	}
	return total;
}

int main(int argc, char** argv) {
	double lb=-999,ub=-999;
	int verbose=0,intervals=5;
	int function=0,def=0;
	
	//parse command line arguments
	for (int i = 1; i < argc; i++){ /* Skip argv[0] (program name). */
		if (strcmp(argv[i], "-verbose") == 0){
			verbose = 1;
		}else if( strcmp(argv[i],"-default-bounds")==0 ){
			def=1;
		}else{
			//take the first 3 chars of the argument in question
			char* sub;
			sub=strndup(argv[i],3);
			
			//and use them to populate argument variables
			if( strcmp(sub,"-lb")==0 ){
				lb=strtod(argv[i]+4,NULL);//convert the rest of the argument string to a double
			}else if( strcmp(sub,"-ub")==0 ){
				ub=strtod(argv[i]+4,NULL);
			}else if( strcmp(sub,"-fx")==0 ){
				function=atoi(argv[i]+5);//5 because we want to ignore the F after the =
			}else if( strcmp(sub,"-n=")==0 ){
				intervals=atoi(argv[i]+3);//3 because n is only one char (without the "="), while the others are two
			}else{
				printf("unrecognized parameter: %s\n",argv[i]);
				exit(1);//exit in error
			}
		}
	}
	
	if(def==1){//set lb and ub to the defaults
		double def_lb[3]={-PI,0,-PI};
		double def_ub[3]={PI,5,PI};
		lb=def_lb[function-1];
		ub=def_ub[function-1];
		printf("Defaulting bounds to (%f,%f)\n",lb,ub);
	}
	
	double ans=compound_simpsons(function,lb,ub,intervals,verbose==1);
	printf("The answer is: %f\n",ans);
	
	return 0;
}
