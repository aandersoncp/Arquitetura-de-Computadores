#include <iostream> 
#include <fstream> 
#include <cstring> 
#include <cstdlib> 
#include <math.h>

using namespace std;

//definições de tipos
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long microcode;

//estrutura para guardar uma microinstrução decodificada
struct decoded_microcode{
    word nadd;
    byte jam;
    byte sft;
    byte alu;
    word reg_w;
    byte mem;
    byte reg_r;
};

word bus_a = 0, bus_b = 0, bus_c = 1, alu_out = 0;

word mar = 0, mdr = 0, pc = 0, sp = 0, lv = 0, cpp = 0, tos = 0, opc = 0, h = 0;
byte mbr = 0;

#define MEM_SIZE 0xFFFF+1 //0xFFFF + 0x1; // 64 KBytes = 64 x 1024 Bytes = 65536 (0xFFFF+1) x 1 Byte;
byte memory[MEM_SIZE]; //0x0000 a 0xFFFF (0 a 65535)

//estado da ALU para salto condicional
byte n = 0, z = 1;

//sinalizador para desligar máquina
bool halt = false;

//registradores de microprograma
word mpc = 0;

//memória de microprograma: 512 x 64 bits = 512 x 8 bytes = 4096 bytes = 4 KBytes.
//Cada microinstrução é armazenada em 8 bytes (64 bits), mas apenas os 4,5 bytes (36 bits) de ordem mais baixa são de fato decodificados.
//Os 28 bits restantes em cada posição da memória são ignorados, mas podem ser utilizados para futuras melhorias nas microinstruções para controlar microarquiteturas mais complexas.
microcode microprog[512];

//carrega microprograma
//Escreve um microprograma de controle na memória de controle (array microprog, declarado logo acima)
void load_microprog(){
  //Exemplo da aula 09
  microprog[0] = 70582801;
  microprog[1] = 271909393;
  microprog[2] = 403996674;
  microprog[3] = 540344849;
  microprog[4] = 672415746;
  microprog[5] = 3932418;
}

//carrega programa na memória principal para ser executado pelo emulador.
//programa escrito em linguagem de máquina (binário) direto na memória principal (array memory declarado mais acima).
void load_prog(){
  //Programa que soma 2 e 3
  memory[0] = 0; // 0
  memory[1] = 1; // 1
  memory[2] = 2; // 2
  memory[3] = 3; // 3
  //memory[4] = 0b00001110; // 14
  //memory[5] = 0b00000001; // 1
  //memory[6] = 0b00000010; // 2
  //memory[7] = 0b00000100; // 4
  //memory[8] = 0b00000111; // 
  //memory[9] = 0b00001011; //
  //memory[10] = 0b00001111; //1;
    
}

void load_microprogrom(){
    FILE* microprograma = fopen("microprog.rom", "r");
    fread(microprog, sizeof(unsigned long), 512, microprograma);
    fclose(microprograma);
}


void write_microcode(microcode w){ //Dado uma microinstrucao, exibe na tela devidamente espaçado pelas suas partes.

   unsigned int v[36];
   for(int i = 35; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 36; i++)
   {
       cout << v[i];
       if(i == 8 || i == 11 || i == 13 || i == 19 || i == 28 || i == 31) cout << " ";
   }
}

void write_byte(byte b) //Dado um byte (valor de 8 bits), exibe o valor binário correspondente na tela.
{
   unsigned int v[8];
   for(int i = 7; i >= 0; i--)
   {
       v[i] = (b & 1);
       b = b >> 1;
   }

   for(int i = 0; i < 8; i++)
       cout << v[i];
}
void write_word(word w) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor binário correspondente.
{
   unsigned int v[32];
   for(int i = 31; i >= 0; i--)
   {
       v[i] = (w & 1);
       w = w >> 1;
   }

   for(int i = 0; i < 32; i++)
       cout << v[i];
}
void write_dec(word d) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor decimal correspondente.
{
   cout << (int)d << endl;
}


decoded_microcode decode_microcode(microcode code){ //Recebe uma microinstrução binaria e separa suas partes preenchendo uma estrutura de microinstrucao decodificada, retornando-a.
    decoded_microcode dec;
    unsigned int v = 0;
    int i;
    for(i = 0; i < 4; i++){
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.reg_r = v;

    v = 0;
    for(i = 0; i < 3; i++){ 
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.mem = v;

    v = 0;
    for(i = 0; i < 9; i++){ 
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.reg_w = v;

    v = 0;
    for(i = 0; i < 6; i++){
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.alu = v;

    v = 0;
    for(i = 0; i < 2; i++){
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.sft = v;

    v = 0;
    for(i = 0; i < 3; i++){
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.jam = v;

    v = 0;
    for(i = 0; i < 9; i++){ 
       v = v + (code & 1)*pow(2, i);
       code = code >> 1;
    }
    dec.nadd = v;

    return dec;
}

void shift(byte s, word w){
	int left, right;
	right = (s & 1);
	s = s >> 1;
	left = (s & 1);

	if(left == 1 && right == 0){
		w = w << 8;
	} else if(left == 0 && right == 1){
		w = w >> 1;
	}
	bus_c = w;
}

void write_register(word reg_end){
	unsigned int v[9];
   for(int i = 8; i >= 0; i--){
       v[i] = (reg_end & 1);
       reg_end = reg_end >> 1;
   } 
   if(v[0] == 1){
   	h = bus_c;
   }
   if(v[1] == 1){
   	opc = bus_c;
   }
   if(v[2] == 1){
		tos = bus_c;
   }
   if(v[3] == 1){
		cpp = bus_c;
   }
   if(v[4] == 1){
		lv = bus_c;
   }
   if(v[5] == 1){
		sp = bus_c;
   }
   if(v[6] == 1){
		pc = bus_c;
   }
   if(v[7] == 1){
		mdr = bus_c;
   }
   if(v[8] == 1){
		mar = bus_c;
   }
}
void read_registers(byte reg_end){
   if(reg_end == 0){
   	bus_b = mdr;
   }
   if(reg_end == 1){
   	bus_b = pc;
   }
   if(reg_end == 2){
    int k = 0;
    byte x = mbr;
    for(int i = 0; i < 7; i++){
      x = x >> 1;
      if(i == 6){
        k = (x & 1);
      }
    }
    if(k == 0){ 
      bus_b = mbr;
    } else{
      bus_b = 4294967040 + mbr; // 4294967040 é 11111111111111111111111100000000 em binário(os 24 bits de mais alta ordem todos 1 e o resto 0)
    }
   }
   if(reg_end == 3){
   	bus_b = mbr;
   }
   if(reg_end == 4){
   	bus_b = sp;
   }
   if(reg_end == 5){
   	bus_b = lv;
   }
   if(reg_end == 6){
   	bus_b = cpp;
   }
   if(reg_end == 7){
   	bus_b = tos;
   }
   if(reg_end == 8){
   	bus_b = opc;
   }

   bus_a = h;
}
void mainmemory_io(byte control){
    int k;
    k = (control & 1);
    control = control >> 1;

    if(k == 1){
      mbr = memory[pc];
    }

    k = (control & 1);
    control = control >> 1;

    if(k == 1){
      word aux1, aux2, aux3, aux4;
      mdr = 0;
      mar = mar << 2;
      aux1 = memory[mar];
      aux2 = memory[mar + 1]*pow(2, 8);
      aux3 = memory[mar + 2]*pow(2, 16);
      aux4 = memory[mar + 3]*pow(2, 24);
      mdr = aux1 + aux2 + aux3 + aux4;
    }

    k = (control & 1);

    if(k == 1){
      int i; 
      byte m = 0;
      mar = mar << 2;
      for(i = 0; i < 4; i++){
        m = 0;
        for(int j = 0; j < 8; j++){
          m = (mdr & 1)*pow(2, j) + m;
          mdr = mdr >> 1;
        }
        memory[mar + i] = m;
      }
    }
}

void debug(bool clr = true)
{
    if(clr) system("clear");

    cout << "Microinstrução: ";
    write_microcode(microprog[mpc]);

    cout << "\n\nMemória principal: \nPilha: \n";
    for(int i = lv*4; i <= sp*4; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\n\nPC: \n";
    for(int i = (pc-1); i <= pc+20; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\nRegistradores - \nMAR: " << mar << " ("; write_word(mar);
    cout << ") \nMDR: " << mdr << " ("; write_word(mdr);
    cout << ") \nPC : " << pc << " ("; write_word(pc);
    cout << ") \nMBR: " << (int) mbr << " ("; write_byte(mbr);
    cout << ") \nSP : " << sp << " (";  write_word(sp);
    cout << ") \nLV : " << lv << " ("; write_word(lv);
    cout << ") \nCPP: " << cpp << " ("; write_word(cpp);
    cout << ") \nTOS: " << tos << " ("; write_word(tos);
    cout << ") \nOPC: " << opc << " ("; write_word(opc);
    cout << ") \nH  : " << h << " ("; write_word(h);
    cout << ")" << endl;
}

void alu(byte func, word a, word b){
    int k = 0;
    for(int i = 0; i < 6; i++){
      k = (func & 1)*pow(2, i) + k;
      func = func >> 1;
    } 
    if(k == 24){
      alu_out = a;
    }
    if(k == 20){
      alu_out = b;
    }
    if(k == 26){
      alu_out = ~a;
    }
    if(k == 44){
      alu_out = ~b;
    }
    if(k == 60){
      alu_out = a + b;
    }
    if(k == 61){
      alu_out = a + b + 1;
    }
    if(k == 57){
      alu_out = a + 1;
    }
    if(k == 53){
      alu_out = b + 1;
    }
    if(k == 63){
      alu_out = b - a;
    }
    if(k == 54){
      alu_out = b - 1;
    }
    if(k == 59){
      int j = a;
      j = (j >> 31) & 1;
      a = a << 1;
      a = a >> 1;
      if(j == 1){
        alu_out = - a;
      } else{
        alu_out = a;
      }
    }
    if(k == 12){
      alu_out = (a & b); 
    }
    if(k == 28){
      alu_out = (a | b);
    }
    if(k == 16){
      alu_out = 0;
    }
    if(k == 49){
      alu_out = 1;
    }
    if(k == 50){
      alu_out = -1;
    }
    if(alu_out == 0){
      z = 1;
      n = 0;
    } else{
      z = 0;
      n = 1;
    }
    cout << "alu_out: " << alu_out << endl;
}

word next_address(word next, byte jam){

  int jamz, jamn, jmpc, aux = 0;
  word aux2 = next;

  jamz = (jam & 1);
  jam = jam >> 1;

  jamn = (jam & 1);
  jam = jam >> 1;

  jmpc = (jam & 1);

  if(jmpc){
    aux2 = (mbr | aux2);
    next = aux2;
  }
  if(jam < 256){
    aux = 1;
  }

  if(jamn && n && aux){
    next = 256 + next;
  }

  if(jamz && z && aux){
    next = (256 + next);
  }
  
  return next;
}

int main(){
  
  load_microprog(); //carrega microprograma de controle

  load_prog(); //carrega programa na memória principal a ser executado pelo emulador. Já que não temos entrada e saída, jogamos o programa direto na memória ao executar o emulador.

  //load_microprogrom(); //carrega microprog.rom 

  decoded_microcode decmcode;

  //laço principal de execução do emulador. Cada passo no laço corresponde a um pulso do clock.
  //o debug mostra o estado interno do processador, exibindo valores dos registradores e da memória principal.
  //o getchar serve para controlar a execução de cada pulso pelo clique de uma tecla no teclado, para podermos visualizar a execução passo a passo.
  //Substitua os comentários pelos devidos comandos (na ordem dos comentários) para ter a implementação do laço.
  while(!halt){

    debug();

    decmcode = decode_microcode(microprog[mpc]);//implementar! Pega uma microinstrução no armazenamento de controle no endereço determinado pelo registrador contador de microinstrução e decodifica (gera a estrutura de microinstrução, ou seja, separa suas partes). Cada parte é devidamente passada como parâmetro para as funções que vêm a seguir.
    read_registers(decmcode.reg_r);//implementar! Lê registradores
    alu(decmcode.alu, bus_a, bus_b);//implementar! Executa alu
    shift(decmcode.sft, alu_out);//implementar! Executa deslocamento
    write_register(decmcode.reg_w);//implementar! Escreve registradores
    mainmemory_io(decmcode.mem);//implementar! Manipula memória principal
    mpc = next_address(decmcode.nadd, decmcode.jam);//implementar! Determina endereço da microinstruçãoo (mpc) a ser executada no próximo pulso de clock

    //cout << endl << endl << "===========SAÍDA ULA===========" << endl << endl;
    //write_dec(bus_c);
    //cout << endl  << "===========SAÍDA ULA===========" << endl;

    getchar();
  }

  debug(false);

  return 0;
  
}


