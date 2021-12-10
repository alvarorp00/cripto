## OpenSSL

**OpenSSL** es una robusta librería criptográfica de propósito general con orientación comercial pensada para una comunicación segura. La licencia que utiliza es _Apache license_, lo que permite utilizarla para propósitos tanto comerciales como no comerciales libremente.

En esta parte de la práctica, vamos a ver diferentes aspectos y herramientas que nos permite utilizar.

### Cifrados Simétricos

```text
$ openssl help
Standard commands
asn1parse         ca                ciphers           cms               
crl               crl2pkcs7         dgst              dhparam           
dsa               dsaparam          ec                ecparam           
enc               engine            errstr            gendsa            
genpkey           genrsa            help              list              
nseq              ocsp              passwd            pkcs12            
pkcs7             pkcs8             pkey              pkeyparam         
pkeyutl           prime             rand              rehash            
req               rsa               rsautl            s_client          
s_server          s_time            sess_id           smime             
speed             spkac             srp               storeutl          
ts                verify            version           x509              

Message Digest commands (see the `dgst' command for more details)
blake2b512        blake2s256        gost              md4               
md5               rmd160            sha1              sha224            
sha256            sha3-224          sha3-256          sha3-384          
sha3-512          sha384            sha512            sha512-224        
sha512-256        shake128          shake256          sm3               

Cipher commands (see the `enc' command for more details)
aes-128-cbc       aes-128-ecb       aes-192-cbc       aes-192-ecb       
aes-256-cbc       aes-256-ecb       aria-128-cbc      aria-128-cfb      
aria-128-cfb1     aria-128-cfb8     aria-128-ctr      aria-128-ecb      
aria-128-ofb      aria-192-cbc      aria-192-cfb      aria-192-cfb1     
aria-192-cfb8     aria-192-ctr      aria-192-ecb      aria-192-ofb      
aria-256-cbc      aria-256-cfb      aria-256-cfb1     aria-256-cfb8     
aria-256-ctr      aria-256-ecb      aria-256-ofb      base64            
bf                bf-cbc            bf-cfb            bf-ecb            
bf-ofb            camellia-128-cbc  camellia-128-ecb  camellia-192-cbc  
camellia-192-ecb  camellia-256-cbc  camellia-256-ecb  cast              
cast-cbc          cast5-cbc         cast5-cfb         cast5-ecb         
cast5-ofb         des               des-cbc           des-cfb           
des-ecb           des-ede           des-ede-cbc       des-ede-cfb       
des-ede-ofb       des-ede3          des-ede3-cbc      des-ede3-cfb      
des-ede3-ofb      des-ofb           des3              desx              
rc2               rc2-40-cbc        rc2-64-cbc        rc2-cbc           
rc2-cfb           rc2-ecb           rc2-ofb           rc4               
rc4-40            seed              seed-cbc          seed-cfb          
seed-ecb          seed-ofb          sm4-cbc           sm4-cfb           
sm4-ctr           sm4-ecb           sm4-ofb   
```

Vemos que se nos muestra una ayuda muy general, separándose en varios apartados según sus comandos(_standard_, _digesters_, _ciphers_).

Concretamente, para ver con más detalle los cifrados:

```text
$ openssl ciphers -v
TLS_AES_256_GCM_SHA384  TLSv1.3 Kx=any      Au=any  Enc=AESGCM(256) Mac=AEAD
TLS_CHACHA20_POLY1305_SHA256 TLSv1.3 Kx=any      Au=any  Enc=CHACHA20/POLY1305(256)
[...]
```
Nota: se ha omitido parte de la salida por cuestiones de formato.

Los cifrados soportados son los siguientes:

  - [AES][1]$^1$
    - Algoritmo de cifrado simétrico que se encuentra entre los más utilizados y seguros del mundo. También conocido como **Rijndael**, a diferencia de al que sustituyó - **DES** -, consiste en una red de sustitución - permutación y no en una red de Feistel. Es implementable tanto en hardware como en software, soporta tamaños de bloque fijos de $128 bits$ y tamaños de clave de $128$, $196$ ó $256 bits$; cabe decir que la versión original de **Rijndael** soporta tamaños de clave múltiplos de $32 bits$, con mínimo de $128$ y máximo de $256$.
    - Hace la mayoría de sus cálculos en un campo finito conocido como _Galois Field_, concretamente el campo encerrado en $GF(2^8)$. Opera sobre una matriz de $4Bytes \times 4Bytes$ llamada _state_.
    - Dependiendo de la longitud de clave, por cada bloque hace 10, 12 ó 14 rondas.
  - [ARIA][2]$^2$
    - Cifrado propuesto un grupo de investigadores surcoreanos en 2003. Basado en una inversión - referido normalmente como _involution_, en matemáticas se refiere a la una función $f$ tal que aplicada sobre $g \implies f(g(x)) = x$ - de la [SPN][3] (_Sustitution - Permutation Network_) que fue estudiada por los desarrolladores del AES. Previamente se habían enviado proposiciones de algoritmos de cifrado por bloques que eran inversiones de esta red, pero cuyas capas de difusión que operan con multiplicaciones en campos finitos, pese a ser favorables en contextos de eficiencia, pueden derivar en algunos fallos causados por dichas estructuras de inversión completa.
    - ARIA es un cifrado por bloques basado en una SPN pero sin ser completamente inversivo. Es decir, las capas de sustitución no son completamente inversivas.
    - La interfaz es la misma que en el AES: $128 bits$ de bloque con claves de $128$, $192$ ó $256 bits$, aunque cambia el número de rondas, que pasa a ser 12, 14 ó 16, dependiendo del tamaño de la clave.
    - Utiliza SBoxes de $8bits \times 8bits$ y sus inversas en rondas alternativamente, siendo una de estas la Rijndael S-Box.
  - [BLOWFISH][4]$^{4,5}$
    - Algoritmo de cifrado por bloques, con bloques de tamaño $64 bits$, con un tamaño de clave que varía entre $32$ y $448 bits$. Utiliza 18 subclaves en un P-Array, que consiste en 18 claves de $32 bits$ (P1, P2, ..., P18). También utiliza 4 SBoxes de $32 bits$ con 256 entradas por cada una de ellas (S1,0, ..., S1,255, ..., S4,0, ..., S4,255). Utiliza 16 rondas para cifrar cada bloque.
  - [CAMELLIA][6]$^6$
    - Cifrado por bloques, con bloques de tamaño $128 bits$, con tamaño de clave de $128, 192 y 256 bits$ (misma interfaz que AES).
    - Soporta implementación tanto en software como en hardware, además de probar una gran seguridad contra ataques basados en criptoanálisis lineales.
    - Comparado contra otros finalistas del AES (MARS, RC6, Rijndael, Serpent, Twofish), ofrece al menos una velocidad de cifrado y descifrado comparable a estos.
    - Una versión optimizada en código ensamblador puede cifrar en un procesador _Pentium III_ (800 MHz) más de 276 Mb/s, lo que lo convierte en un cifrado mucho más rápido que una versión optimizada del cifrado DES.
    - Su diseño en hardware (incluyendo tanto cifrado como descifrado) requiere en torno a unas 11.000 puertas lógicas, siendo así el que menos ocupaba en el momento de su publicación entre los cifradores de 128 bits.
    - En el _paper_ de su publicación se asegura que, utilizando técnicas de criptoanálisis basadas en el _estado del arte (state of art)_, no tiene características que puedan sostenerse con una probabilidad superior a $2^{-128}$.
    - Utiliza SBoxes de $8bits \times 8bits$, y realiza operaciones lógicas (muy rápidas en _hardware_), lo que permite una implementación eficiente incluso en arquitecturas con procesadores de $8bits$ como las de las _smart cards_, procesadores de $32 bits$ usados ampliamente en su día y los dominantes actuales, de $64 bits$.
  - [CAST][7]$^{7,8,9}$
    - Es un diseño de un cifrador simétrico que opera sobre bloques de $64 bits$ de texto plano, consistente en 16 rondas y que produce $64 bits$ de texto cifrado, con una longitud de clave variable entre $40$ y $128 bits$ en saltos de $8 bits$.
    - Este cifrador se basa en una red de Feistel (_Feistel Network_) clásica, utilizando la conocida función $F$. 
    - El uso de la función anterior incluye operaciones sobre 4 SBoxes, cada una con un tamaño de $8bits \times 32 bits$ (el mismo tamaño que las utilizadas en el DES).
  - [DES][10]$^{10}$
    - La especificación abarca dos FIPS (_Federal Information Processing Standard_). Consiste en un cifrador simétrico por bloques de $64 bits$ desarrollado por IBM en los años setenta, con una longitud de clave original de $64 bits$, siendo reducida a $56$ manteniendo los otros $8 bits$ extra de paridad, aunque la versión que aquí predomina es que la NSA podía hacer ataques por fuerza bruta sobre claves de $56 bits$ antes que el resto del mundo, y de ahí la motivación en la reducción.
    - El cifrado está basado en operaciones sobre una red de Feistel, utilizando su función $F$. Utiliza 16 subclaves generadas a partir de una clave de $64 bits$ (ignorando $8 bits$ de esos $64$, dejándolos como bits de paridad para control de errores), cada una de $48 bits$, alimentando la función $F$ con la subclave correspondiente a la ronda y el bloque derecho ($32 bits$ más significativos del bloque de $64 bits$ en ronda).
    - Permite implementación en software, hardware, firmware y, según la propia especificación, cualquier otra combinación de estas. Actualmente el DES de una clave (_single DES_ o simplemente _DES_) está en desuso debido a que no es seguro, por lo que ha sido reemplazado tanto por el cifrado AES como por el TDEA (_Triple DES Encryption Algorithm_); siendo este último un cifrado que, con tres claves de $64 bits$ ($56 bits$ efectivos), cifra un bloque con la primera clave, descifra con la segunda y cifra con la tercera; haciendo las operaciones inversas en el descifrado: descifra con la tercera, cifra con la segunda y descifra con la primera. Debido a que es susceptible (puesto que el _single DES_ lo es) a ataques MITM (_Meet In The Middle_), la longitud efectiva de clave pasa a ser de $112 bits$ en vez de $168 bits$.
  - [RC2][11]$^{11}$
    - Desarrollado por Ron Rivest en 1987 (_Ron Cipher 2_), es un algoritmo de cifrado simétrico con bloques de texto de $64 bits$ y un tamaño de clave variable entre $8$ y $1024 bits$. Puede considerarse como una proposición para el sustituto de DES (aunque finalmente en el _AES Competition Process_ se presentó una versión más moderna, el _RC6_).
    - Es un algoritmo diseñado para implementarse con facilidad en microprocesadores de $16bits$. En un IBM AT (_IBM Personal Computer (1984)_), el cifrado corría a una velocidad al doble que DES (asumiendo que la fase de expansión de clave se haya hecho previamente).
    - La expansión de clave consiste en pasar de un _input_ de $64 bits$ a $64$ subclaves de $16 bits$ cada una.
    - La fase de cifrado coge un _input_ de $64 bits$ y hace un _in place_, es decir, recibe $4$ bloques de $16 bits$ y cifra cada uno de estos dejándolos en su sitio (manteniendo el orden de los bloques de $16bits$).
  - [SEED][12]$^{12}$
    - Cifrado simétrico de bloques de $128 bits$ desarrollado por KISA (_Korean Information Security Agency_) desde 1998. Es un estándar nacional en la República de Korea, y está diseñado para utilizar SBoxes y permutaciones que estén balanceadas con la capacidad computacional de la tecnología actual.
    - Utiliza una red de Feistel con $16$ rondas, y es fuerte contra criptoanálisis lineales y diferenciales; así como ataques de clave relacionados, balanceados con una compensación entre seguridad y eficiencia.
    - Utiliza claves de $128 bits$ y SBoxes de $8 bits \times 8 bits$.
  - [SM4][13]$^{13}$
    - Cifrado simétrico de bloques, con una longitud de bloque de 128 bits, con claves de igual tamaño.Opera sobre una estructura de Feistel no balanceada, e itera sus funciones de ronda 32 veces (32 rondas) tanto en el cifrado como en el descifrado y en las fases de expansión de clave (32 subclaves generadas). El funcionamiento del descifrado es el clásico para los basados en estructuras de Feistel: el mismo procedimiento que para el descifrado pero con las claves en orden inverso.


Vamos a probar a cifrar con alguno de ellos, cifrando el texto que se encuentra en _plaintexts/lorem_ipsum.txt_. 

Creamos un _script_ para hacer las diferentes llamadas correspondientes, añadiendo en la primera línea `#!/bin/bash -x` para que nos muestre la salida se los comandos que se están ejecutando.

```bash
#!/bin/bash -x
echo "[INFO] Usage: $0 <algorithm> <key> [<iv>]"

openssl enc -base64 -e $1 -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K $2 -iv $3;
openssl enc -base64 -d $1 -in ciphertexts/lorem_ipsum.txt  -out decryptedtexts/lorem_ipsum.txt -K $2 -iv $3;
diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c;
diff <(xxd plaintexts/lorem_ipsum.txt) <(xxd ciphertexts/lorem_ipsum.txt) | wc -c;

exit 0;
```

AES

```text
$ ./test.sh -aes-256-cbc 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -aes-256-cbc -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ openssl enc -base64 -d -aes-256-cbc -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42413
+ exit 0
```

ARIA

```text
$ ./test.sh -aria-256-cfb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -aria-256-cfb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ openssl enc -base64 -d -aria-256-cfb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

BLOWFISH

```text
$ ./test.sh -des-cfb 01234567 76543210 -x
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -des-cfb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ openssl enc -base64 -d -des-cfb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

CAMELLIA

```text
$ ./test.sh -camellia-192-cbc 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -camellia-192-cbc -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ openssl enc -base64 -d -camellia-192-cbc -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210

+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42413
+ exit 0
```

CAST

```text
./test.sh -cast5-cfb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -cast5-cfb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ openssl enc -base64 -d -cast5-cfb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

DES

```text
./test.sh -des-ofb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -des-ofb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ openssl enc -base64 -d -des-ofb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ wc -c
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
0
+ wc -c
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
++ xxd plaintexts/lorem_ipsum.txt
42347
+ exit 0
```

TDEA

```text
$ ./test.sh -des-ede3-ofb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -des-ede3-ofb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ openssl enc -base64 -d -des-ede3-ofb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

RC2

```text
./test.sh -rc2-ecb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -rc2-ecb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
warning: iv not used by this cipher
+ openssl enc -base64 -d -rc2-ecb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
warning: iv not used by this cipher
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
++ xxd ciphertexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
42347
+ exit 0
```

SEED

```text
$ ./test.sh -seed-ofb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -seed-ofb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ openssl enc -base64 -d -seed-ofb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

RM4

```text
./test.sh -sm4-cfb 01234567 76543210
+ echo '[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]'
[INFO] Usage: ./test.sh <algorithm> <key> [<iv>]
+ openssl enc -base64 -e -sm4-cfb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ openssl enc -base64 -d -sm4-cfb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -K 01234567 -iv 76543210
+ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt
+ wc -c
0
+ wc -c
++ xxd plaintexts/lorem_ipsum.txt
+ diff /dev/fd/63 /dev/fd/62
++ xxd ciphertexts/lorem_ipsum.txt
42347
+ exit 0
```

Como podemos ver, ciframos y desciframos y siempre recuperamos el texto original, aunque dependiendo del cifrado, del modo utilizado y del tamaño del bloque del cifrador se producirá mayor o menor diferencia.

Vamos a probar con algunas claves débiles de DES (por lo que necesitaremos utilizar el encadenamiento ecb para observarlo, aunque lo comprobaremos experimentalmente).

```text
openssl enc -e -des-ecb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 0101010101010101
$ openssl enc -e -des-ecb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt  -K 0101010101010101
$ diff <(xxd plaintexts/lorem_ipsum.txt) <(xxd ciphertexts/lorem_ipsum.txt) | wc -c
35979
$ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c
191
```

De hecho:

```text
$ cat decryptedtexts/lorem_ipsum.txt
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras vel aliquam orci, nec hendrerit leo. Suspendisse tortor ipsum, volutpat vel justo id, mollis mattis sem.
[...]
```

Hemos visto que hay una clave débil. Sin embargo, si cambiamos el modo de operación:

```text
$ openssl enc -e -des-cfb -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K 0101010101010101 -iv 0
hex string is too short, padding with zero bytes to length
$ openssl enc -e -des-cfb -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt  -K 0101010101010101 -iv 0
hex string is too short, padding with zero bytes to length
$ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c
82
```

Y comprobamos:

```text
$ cat decryptedtexts/lorem_ipsum.txt
Lorem ip�C
          �E��
```

Vemos que el primer bloque tiene el mismo problema, pero a partir del segundo, debido al encadenamiento ya no. Además, esto es independiente del vector de inicialización, pues en la primera ronda se aplicará el mismo, siendo así el primer bloque descubierto siempre que la clave sea débil.

## Referencias

\[1]: https://nvlpubs.nist.gov/nistpubs/fips/nist.fips.197.pdf

\[2]: http://www.math.snu.ac.kr/~jinhong/04Aria.pdf

\[3]: https://en.wikipedia.org/wiki/Substitution%E2%80%93permutation_network

\[4]: https://www.geeksforgeeks.org/blowfish-algorithm-with-examples/

\[5]: https://www.schneier.com/academic/archives/1995/09/the_blowfish_encrypt.html

\[6]: https://info.isl.ntt.co.jp/crypt/eng/camellia/dl/reference/sac_camellia.pdf

\[7]: https://es.wikipedia.org/wiki/CAST-128

\[8]: https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.182.6820&rep=rep1&type=pdf

\[9]: https://www.ime.usp.br/~rt/cast256/CAST-256.pdf

\[10]: https://csrc.nist.gov/csrc/media/publications/fips/46/3/archive/1999-10-25/documents/fips46-3.pdf

\[11]: https://www.ipa.go.jp/security/rfc/RFC2268EN.html

\[12]: https://datatracker.ietf.org/doc/html/rfc4269

\[13]: http://www.gmbz.org.cn/upload/2018-04-04/1522788048733065051.pdf

<!-- Links -->

[1]: https://nvlpubs.nist.gov/nistpubs/fips/nist.fips.197.pdf
[2]: http://www.math.snu.ac.kr/~jinhong/04Aria.pdf
[3]: https://en.wikipedia.org/wiki/Substitution%E2%80%93permutation_network
[4]: https://www.geeksforgeeks.org/blowfish-algorithm-with-examples/
[5]: https://www.schneier.com/academic/archives/1995/09/the_blowfish_encrypt.html
[6]: https://info.isl.ntt.co.jp/crypt/eng/camellia/dl/reference/sac_camellia.pdf
[7]: https://es.wikipedia.org/wiki/CAST-128
[8]: https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.182.6820&rep=rep1&type=pdf
[9]: https://www.ime.usp.br/~rt/cast256/CAST-256.pdf
[10]: https://csrc.nist.gov/csrc/media/publications/fips/46/3/archive/1999-10-25/documents/fips46-3.pdf
[11]: https://www.ipa.go.jp/security/rfc/RFC2268EN.html
[12]: https://datatracker.ietf.org/doc/html/rfc4269
[13]: http://www.gmbz.org.cn/upload/2018-04-04/1522788048733065051.pdf