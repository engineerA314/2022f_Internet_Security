#include <iostream>
#include <fstream>
#include <string>
#include <gmpxx.h>
#include <gmp.h>
#include <utility>

using namespace std;

pair<mpz_class, mpz_class> EEA(mpz_class N, mpz_class e){
  mpz_class r0=N, r1=e;
	mpz_class s0=1, s1=0;
	mpz_class t0=0, t1=1;

  while(r1){
		mpz_class q=r0/r1;
		mpz_class r2=r0-r1*q;
		r0=r1;
		r1=r2;
		mpz_class s2=s0-s1*q;
		s0=s1;
		s1=s2;
		mpz_class t2=t0-t1*q;
		t0=t1;
		t1=t2;
	}
  return {s0, t0};
}

int main(int argc, char *argv[]){
  if(strcmp(argv[1], "-keygen") == 0){

    mpz_class p, q;

    ifstream ifs;
    ifs.open("primes.txt");
    if(!ifs){
      printf("file 'primes.txt does not exist.");
      return 0;
    }

    ifs >> p;
    ifs >> q;  

    if(mpz_probab_prime_p(p.get_mpz_t(), 50) == 0 || mpz_probab_prime_p(q.get_mpz_t(), 50) == 0){
      cout << mpz_probab_prime_p(p.get_mpz_t(), 50) << endl;
      printf("error, input is not prime\n");
      return 0;
    }

    if(p == q){
      printf("p and q must be different.\n");
      return 0;
    }

    mpz_class n = p * q;

    mpz_class phi_n = (p - 1) * (q - 1);

    //select e that gcd(e, phi_n) = 1
    mpz_class i;
    mpz_class result = 0;

    mpz_class temp1 = p -1;
    mpz_class temp2 = q - 1;

    while(temp1 % 2 == 0){
      temp1 = temp1 / 2;
    }

    while(temp2 % 2 == 0){
      temp2 = temp2 / 2;
    }
    
    for(i = 3; i < phi_n; mpz_nextprime(i.get_mpz_t(),i.get_mpz_t())){
      if((temp1 % i != 0) && (temp2 % i != 0)){
        break;
      }
    }

    mpz_class e = i;
    //get d by using EEA
    mpz_class r0=phi_n, r1=e;
	  mpz_class s0=1, s1=0;
	  mpz_class t0=0, t1=1;

    while(r1){
		  mpz_class q=r0/r1;
		  mpz_class r2=r0-r1*q;
		  r0=r1;
		  r1=r2;
		  mpz_class s2=s0-s1*q;
		  s0=s1;
		  s1=s2;
		  mpz_class t2=t0-t1*q;
		  t0=t1;
		  t1=t2;
	  }

    mpz_class d = t0;
    if(d < 0){
      d = phi_n + d;
    }
    
    ofstream ofs;
    ofs.open("public.key.txt");
    if(!ofs){
      printf("file open error\n");
      return 0;
    }
    ofs << e << endl;
    ofs << n << endl;

    ofstream ofs2;
    ofs2.open("private.key.txt");
    if(!ofs2){
      printf("file open error\n");
      return 0;
    }
    ofs2 << d << endl;
    ofs2 << n << endl;

  }
  else if(strcmp(argv[1], "-encrypt") == 0){

    mpz_class n, e, p, c;

    ifstream pkfile;
    pkfile.open("public.key.txt");
    if(!pkfile){
      printf("file 'public.key.txt does not exist.\n");
      return 0;
    }
    pkfile >> e;
    pkfile >> n;

    ifstream plaintextFile;
    plaintextFile.open("plaintext.txt");
    if(!plaintextFile){
      printf("file 'plaintext.txt does not exist.\n");
      return 0;
    }

    plaintextFile >> p;

    if(p > n){
      printf("plaintext is too long.\n");
      return 0;
    }

    mpz_powm(c.get_mpz_t(), p.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());

    ofstream ofs;
    ofs.open("cyphertext.txt");
    if(!ofs){
      printf("file open error\n");
      return 0;
    }

    ofs << c << endl;

  }else if(strcmp(argv[1], "-decrypt") == 0){
    //use CRT
    mpz_class n, d, plain, c, p , q;

    ifstream pkfile;
    pkfile.open("private.key.txt");
    if(!pkfile){
      printf("file 'private.key.txt does not exist.\n");
      return 0;
    }
    pkfile >> d;
    pkfile >> n;

    ifstream primefile;
    primefile.open("primes.txt");
    if(!primefile){
      printf("file 'primes.txt does not exist.\n");
      return 0;
    }
    primefile >> p;
    primefile >> q;

    ifstream cyphertextFile;
    cyphertextFile.open("cyphertext.txt");
    if(!cyphertextFile){
      printf("file 'cyphertext.txt does not exist.\n");
      return 0;
    }
    cyphertextFile >> c;

    mpz_class c1, c2;
    mpz_class d1, d2, m1, m2, y1, y2;

    c1 = c % p;
    c2 = c % q;    

    d1 = d % (p - 1);
    d2 = d % (q - 1);

    mpz_powm(m1.get_mpz_t(), c1.get_mpz_t(), d1.get_mpz_t(), p.get_mpz_t());
    mpz_powm(m2.get_mpz_t(), c2.get_mpz_t(), d2.get_mpz_t(), q.get_mpz_t());

    mpz_class r0=p, r1=q;
	  mpz_class s0=1, s1=0;
	  mpz_class t0=0, t1=1;

    while(r1){
		  mpz_class q1=r0/r1;
		  mpz_class r2=r0-r1*q1;
		  r0=r1;
		  r1=r2;
		  mpz_class s2=s0-s1*q1;
		  s0=s1;
		  s1=s2;
		  mpz_class t2=t0-t1*q1;
		  t0=t1;
		  t1=t2;
	  }

    y1 = s0;  //inverse of q for mod p
    y2 = t0;  //inverse of p for mod q

    if(y1 < 0){
      y1 = q + y1;
    }

    if(y2 < 0){
      y2 = p + y2;
    }

    plain = (m1 * y2 * q ) + (m2 * y1 * p);
    plain = plain % n;

    ofstream ofs;
    ofs.open("plaintext.decrypt.txt");
    if(!ofs){
      printf("file open error\n");
      return 0;
    }

    ofs << plain << endl;

  }
  else{
    printf("wrong argument\n");
  }
  return 0;

}