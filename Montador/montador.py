import sys

comandos = {'nop': 0x01, 'iadd': 0x02, 'isub': 0x05, 'iand': 0x08, 'ior': 0x0B, 'dup': 0x0E, 'pop': 0x10, 'swap': 0x13, 'bipush': 0x19, 'iload': 0x1C, 'istore': 0x22, 'wide': 0x28, 'ldc_w': 0x32, 'iinc': 0x36, 'goto': 0x3C, 'iflt': 0x43, 'ifeq': 0x47, 'if_icmpeq': 0x4B, 'invokevirtual': 0x55, 'ireturn': 0x6B}
arquivo = [] #lista com elementos sendo as linhas do programa
all_words = [] #lista com elementos sendo todas as strings do programa
num_all_words = [] #lista/vetor que tem o número correspondente do byte de cada elemento da lista all_words
codigo = [] #lista/vetor com os decimais correpondentes a tradução do programa, junto com inicialização e tamanho do programa
ident_ind = [] #lista com os índices (o byte que representa) dos identificadores de linha
ident = {} #dicionário com os identificadores e seus respectivos índices (o byte que representa)
variaveis = {} #dicionário com as variáveis e tendo seus valores sendo as próprias variáveis
varia_ind = {} #dicionário com as variáveis e tendo seus valores o índice da ordem que elas aparecem

inicial = []
registadores = [29440, 6, 4097, 1024]

def inic(registadores):  #Função que coloca na lista/vetor inicial os primeiros 20 bytes de inicialização
      i = 0
      while i < 4:
         sp = registadores[i]
         inicial.append(sp%256)
         sp = sp//256
         inicial.append(sp%256)
         sp = sp//256
         inicial.append(sp%256)
         sp = sp//256
         inicial.append(sp%256)
         i = i + 1
      sp = 4097 + len(variaveis)
      inicial.append(sp%256)
      sp = sp//256
      inicial.append(sp%256)
      sp = sp//256
      inicial.append(sp%256)
      sp = sp//256
      inicial.append(sp%256)

def verificador(arquivo): #Verifica se a sintaxe do arquivo está correta
   ok = True
   for linhas in arquivo:
      if len(linhas) == 1:
         if linhas[0] not in comandos:
            ok = False
      if len(linhas) == 2:
         if linhas[0] not in comandos:
            if linhas[1] not in comandos:
               ok = False
      if len(linhas) == 3:
         if linhas[1] not in comandos:
            ok = False
   return ok

def identificadores(arquivo):  #Identifica o byte que o identificador representam
   i = 0
   for linhas in arquivo:
   	if len(linhas) == 1:
         i = i + 1
   	if len(linhas) == 2:
         if linhas[0] not in comandos:
         	i = i + 1
         	ident[linhas[0]] = i
         	ident_ind.append(i)
         	i = i + 1
         else:
         	i = i + 2
   	if len(linhas) == 3:
         if linhas[0] not in comandos:
         	i = i + 1
         	ident[linhas[0]] = i
         	ident_ind.append(i)
         	i = i + 2
         else: 
            i = i + 3

def contador(arquivo): #Constroi uma lista ajudando onde há variáveis, conta os bytes e diz onde é o byte dos identificadores de linhas
   j = 1
   for linhas in arquivo:
      if len(linhas) == 1:
         num_all_words.append(j)
         j = j + 1
      if len(linhas) == 2:
         if linhas[0] not in comandos:
            ident[linhas[0]] = j
            num_all_words.append(0)   
            num_all_words.append(j)
            j = j + 1 
         else:
            if linhas[0] in ['ldc_w', 'goto', 'iflt', 'ifeq', 'if_icmpeq', 'invokevirtual']:
               num_all_words.append(j)
               j = j + 1
               if linhas[1] in ident:
                  num_all_words.append(linhas[1])
                  j = j + 2
               else:
                  num_all_words.append(j)
                  j = j + 2
            else:
               num_all_words.append(j)
               j = j + 1
               if linhas[1] in ident:
                  num_all_words.append(linhas[1])
                  j = j + 1
               else:
                  num_all_words.append(j)
                  j = j + 1
      if len(linhas) == 3: 
         if linhas[0] == 'iinc':
            num_all_words.append(j)
            j = j + 1
            num_all_words.append(j)
            j = j + 1
            num_all_words.append(j)
            j = j + 1
         else:          
            ident[linhas[0]] = j
            num_all_words.append(0)
            if linhas[1] in ['ldc_w', 'goto', 'iflt', 'ifeq', 'if_icmpeq', 'invokevirtual']:
               num_all_words.append(j)
               j = j + 1
               if linhas[2] in ident:
                  num_all_words.append(linhas[2])
                  j = j + 2
               else:
                  num_all_words.append(j)
                  j = j + 2
            else:
               num_all_words.append(j)
               j = j + 1
               if linhas[2] in ident:
                  num_all_words.append(linhas[2])
                  j = j + 1
               else:
                  num_all_words.append(j)
                  j = j + 1
      if len(linhas) == 4:
         ident[linhas[0]] = j
         num_all_words.append(0)   
         num_all_words.append(j)
         j = j + 1
         num_all_words.append(j)
         j = j + 1
         num_all_words.append(j)
         j = j + 1
         


def words(arquivo): #Função que produz uma lista com todas as strings(identificadores de linha, comando, operandos e variáveis) do arquivo de entrada, diferente de arquivo que tem como cada elemento as linhas do programa
   for linhas in arquivo:
   	for palavras in linhas:
         all_words.append(palavras)

def cont_vars(all_words): #Coloca no dicionário variaveis as variaveis com o valor sendo elas mesmas {'x': 'x', 'y': 'y'}, em outro dicionario coloca a variável, porém com o valor da ordem que ela aparece {'x': 0, 'y': 1}, isso nos ajuda na hora de traduzir
   i = 0
   for palavras in all_words:
   	if palavras not in comandos and palavras not in ident:
         if not palavras.isnumeric() and palavras not in variaveis:
         	variaveis[palavras] = palavras
         	varia_ind[palavras] = i 
         	i = i + 1

def corrige(all_words, num_all_words): #Função que coloca a string que identifica a variáveil na posição(byte) que ela aparece na lista/vetor num_all_words, lista/vetor nos ajuda na hora de "traduzir"
   i = 0;
   for palavras in all_words:
   	if palavras in variaveis:
         num_all_words[i] = variaveis[palavras]
   	i = i + 1

def codificacao(all_words, num_all_words): #Faz a codificação dos bytes 
   for i in range(len(num_all_words)):
      if num_all_words[i] in variaveis:
            codigo.append(varia_ind[num_all_words[i]])
      elif num_all_words[i] in ident:
         aux1 = ident[num_all_words[i]] - num_all_words[i - 1]
         if aux1 < 0:
            aux1 = (65536 + aux1)
            aux2 = aux1//256
            aux1 = aux1 - 256*aux2 
            codigo.append(aux2)
            codigo.append(aux1)
         else:
            if aux1 < 256:
               codigo.append(0)
               codigo.append(aux1)
            else:
               aux2 = aux1//256
               aux1 = aux1 - 256*aux2 
               codigo.append(aux2)
               codigo.append(aux1)
      else:
         if num_all_words[i] > 0: 
            if all_words[i] in comandos:
               codigo.append(comandos[all_words[i]])
            else:
               aux = int(all_words[i])
               codigo.append(int(all_words[i]))

def saida(codigo): #Produz e coloca os bytes codificados no arquivo
   Q = []
   inic(registadores)
   tam = (len(codigo) + 20) 

   Q.append(tam%256)
   tam = tam//256
   Q.append(tam%256)
   tam = tam//256
   Q.append(tam%256)
   tam = tam//256
   Q.append(tam%256)

   codigo = Q + inicial + codigo
   final_bytes = bytes(codigo)
   filename = sys.argv[1].split('.')[0] + '.exe'
   with open(filename, 'wb') as binary_output:
      bytes_written = binary_output.write(final_bytes)
      print("Arquivo produzido")
            

def main():
   with open(sys.argv[1], 'r') as arquivo:
      arquivo = [linhas.split() for linhas in arquivo]

   if verificador(arquivo):
      identificadores(arquivo)
      contador(arquivo)
      words(arquivo)
      cont_vars(all_words)
      corrige(all_words, num_all_words)
      codificacao(all_words, num_all_words)
      saida(codigo)
   else:
      print("False \n")

main()