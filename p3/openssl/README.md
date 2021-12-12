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

### Cifrados Asimétricos

Vemos qué tipos de cifrados asimétricos (de clave pública) tenemos disponibles:

```text
$ openssl list -public-key-algorithms
Name: OpenSSL RSA method
	Type: Builtin Algorithm
	OID: rsaEncryption
	PEM string: RSA
Name: rsa
	Alias for: rsaEncryption
Name: OpenSSL PKCS#3 DH method
	Type: Builtin Algorithm
	OID: dhKeyAgreement
	PEM string: DH
Name: dsaWithSHA
	Alias for: dsaEncryption
Name: dsaEncryption-old
	Alias for: dsaEncryption
Name: dsaWithSHA1-old
	Alias for: dsaEncryption
Name: dsaWithSHA1
	Alias for: dsaEncryption
Name: OpenSSL DSA method
	Type: Builtin Algorithm
	OID: dsaEncryption
	PEM string: DSA
Name: OpenSSL EC algorithm
	Type: Builtin Algorithm
	OID: id-ecPublicKey
	PEM string: EC
Name: OpenSSL HMAC method
	Type: Builtin Algorithm
	OID: hmac
	PEM string: HMAC
Name: OpenSSL CMAC method
	Type: Builtin Algorithm
	OID: cmac
	PEM string: CMAC
Name: OpenSSL RSA-PSS method
	Type: Builtin Algorithm
	OID: rsassaPss
	PEM string: RSA-PSS
Name: OpenSSL X9.42 DH method
	Type: Builtin Algorithm
	OID: X9.42 DH
	PEM string: X9.42 DH
Name: OpenSSL X25519 algorithm
	Type: Builtin Algorithm
	OID: X25519
	PEM string: X25519
Name: OpenSSL X448 algorithm
	Type: Builtin Algorithm
	OID: X448
	PEM string: X448
Name: OpenSSL POLY1305 method
	Type: Builtin Algorithm
	OID: poly1305
	PEM string: POLY1305
Name: OpenSSL SIPHASH method
	Type: Builtin Algorithm
	OID: siphash
	PEM string: SIPHASH
Name: OpenSSL ED25519 algorithm
	Type: Builtin Algorithm
	OID: ED25519
	PEM string: ED25519
Name: OpenSSL ED448 algorithm
	Type: Builtin Algorithm
	OID: ED448
	PEM string: ED448
Name: sm2
	Alias for: id-ecPublicKey
```

De los anteriores, nos centramos en los principales, que son los siguientes:

 - [RSA][19]$^{19}$:
   - Sistema criptográfico de clave pública desarrollado en 1979, basado en factorización de números enteros. Es el más utilizado de este tipo y sirve para cifrar y firmar electrónicamente.
 - [PKCS#3 DH][20]$^{20}$:
   - Es parte del estándar desarrollado por RSA Laboratories. En concreto esta versión es la del RSA de Diffie - Hellman.
 - [DSA][21]$^{21}$:
   - Estándar de cifrado de clave pública del Gobierno Federal de los Estados Unidos de América. También basado en aritmética modular como RSA, aunque el anterior se fundamentaba en factorización de números primos mientras que DSA utiliza el problema del logaritmo discreto. Este es algo más eficiente firmando, pero más lento verificando.
 - [EC][22]$^{22}$:
   - Se centra en parejas de claves públicas y privadas, frecuentemente discutido en el contexto del algoritmo RSA. Este tipo de criptografía es una alternativa a RSA, pues utiliza el concepto matemático de curvas elípticas para conseguir resultados similares con órdenes de hasta 10 veces menos de longitud de clave.

Podemos hacer la prueba con alguno de ellos, centrándonos en [**RSA**][15]$^{14, 15}$.

```text
$ openssl genrsa -out private_key.pem 3072
Generating RSA private key, 3072 bit long modulus (2 primes)
...........................++++
..........................................................................................................++++
e is 65537 (0x010001)
$ openssl rsa -in private_key.pem -text
RSA Private-Key: (3072 bit, 2 primes)
modulus:
    00:bc:a4:09:f9:4f:16:9e:5f:91:a8:50:70:da:e9:
    [...]
    f6:cb:52:95:0d:22:ac:ed:9a:1b
publicExponent: 65537 (0x10001)
privateExponent:
    7c:0f:b2:13:89:65:09:c1:36:8d:28:62:20:46:79:
    [...]
    14:9d:2c:49:3b:a3:09:70:09
prime1:
    00:f8:bb:3c:77:87:59:c2:f6:28:d0:e9:bc:16:2d:
    [...]
    8f:b0:9a:fd:a3:32:1e:a3:ab:e7:28:46:b7
prime2:
    00:c2:27:43:8b:79:43:46:09:14:ad:1f:f2:33:74:
    [...]
    91:de:75:2e:e9:ed:2c:dc:3a:68:4c:c3:bd
exponent1:
    79:55:cd:3f:79:d5:7d:d7:db:3d:5e:68:14:61:6c:
    [...]
    59:11:a2:9d:53:bb:c2:97:ff:21:94:ad
exponent2:
    27:10:43:54:d2:a7:28:76:37:44:c6:02:b3:3b:b0:
    [...]
    33:88:1c:52:50:b8:0f:ef:68:80:d9:79
coefficient:
    00:9c:69:8d:9f:c0:48:0f:2a:59:9c:49:5e:0f:15:
    [...]
    ec:95:20:5a:aa:fa:fd:1c:f5:a2:d7:6a:b1
writing RSA key
-----BEGIN RSA PRIVATE KEY-----
MIIG4wIBAAKCAYEAvKQJ+U8Wnl+RqFBw2ulBDi7FIApSgd0w780oVj3hDdIcyVdM
[...]
0DSSDDhg9ElNMgbrRibi1iRZgGcQqZj4giXslSBaqvr9HPWi12qx
-----END RSA PRIVATE KEY-----
```

Y para visualizar su clave pública:

```text
$ openssl rsa -in private_key.pem -pubout -out public_key.pem
writing RSA key
$ openssl rsa -in public_key.pem -pubin -text
RSA Public-Key: (3072 bit)
Modulus:
    00:bc:a4:09:f9:4f:16:9e:5f:91:a8:50:70:da:e9:
    [...]
    f6:cb:52:95:0d:22:ac:ed:9a:1b
Exponent: 65537 (0x10001)
writing RSA key
-----BEGIN PUBLIC KEY-----
MIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAvKQJ+U8Wnl+RqFBw2ulB
[...]
rIo4frlHi43+rbRN+EeA4/R5YqrfpJT2y1KVDSKs7ZobAgMBAAE=
-----END PUBLIC KEY-----
```

Probamos ahora a cifrar:

```text
$ openssl rsautl -encrypt -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -pubin -inkey public_key.pem
$ openssl rsautl -decrypt -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -inkey private_key.pem
$ diff <(xxd plaintexts/lorem_ipsum.txt) <(xxd ciphertexts/lorem_ipsum.txt) | wc -c
2308
$ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c
0
```

Nota: Hemos acortado el tamaño del fichero para que ocupe $1024 bits$, puesto que rsa requiere que el fichero sea más pequeño que el tamaño de la clave.

### Generación de claves privadas y públicas

Primero vamos con **RSA**:

Generamos una pareja de clave privada - clave pública:

```text
$ openssl genrsa -out private_key.pem 3072
Generating RSA private key, 3072 bit long modulus (2 primes)
................................++++
...............................................................++++
e is 65537 (0x010001)
$ openssl rsa -in private_key.pem -pubout -out public_key.pem
writing RSA key
```

Ahora, calculando el hash con **SHA512**, cifraremos el resultado del hash y comprobaremos que el hash del original y el descifrado del resultado del hash cifrado (la firma) tienen el mismo valor:

```text
openssl genrsa -out private_key.pem 3072
Generating RSA private key, 3072 bit long modulus (2 primes)
................................++++
...............................................................++++
e is 65537 (0x010001)
$ openssl rsa -in private_key.pem -pubout -out public_key.pem
writing RSA key
$ openssl dgst -sha512 -sign private_key.pem -out sha512_signature.sign plaintexts/lorem_ipsum.txt
$ xxd sha512_signature.sign
00000000: 3449 5acd fedb 31fb d2bd 4f02 9a97 c57b  4IZ...1...O....{
00000010: e26b bf52 4967 24d5 c569 9fa7 b4d2 674a  .k.RIg$..i....gJ
00000020: 553e 6e7b f7c0 ebd9 0a94 4771 3aa8 2b5c  U>n{......Gq:.+\
00000030: cbc9 b25e 99c9 0f51 9997 7dd7 abb2 0ed2  ...^...Q..}.....
00000040: 2729 7a14 a84c 75b2 f2a8 42da 24ee 0474  ')z..Lu...B.$..t
00000050: 4b36 b54d 4f98 4a61 c83c 9fe0 bef1 bf4c  K6.MO.Ja.<.....L
00000060: aa63 a61d 49ba a079 3cd6 129d 9662 e87c  .c..I..y<....b.|
00000070: be44 5728 c8fc 7f3e 5d90 04bf 0ee3 5217  .DW(...>].....R.
00000080: bfde 5418 def0 3623 41b4 992e c319 78c6  ..T...6#A.....x.
00000090: 0aec 9136 e577 ae8b 9484 d2ad a01a d851  ...6.w.........Q
000000a0: 3ff0 6200 96ce 4f0c caa6 62cc 3eb6 6dd8  ?.b...O...b.>.m.
000000b0: b365 69bd cfe6 99da 8357 4c9e 288c 7c14  .ei......WL.(.|.
000000c0: 048d 06a1 20da ce03 7d56 1203 a27a 5435  .... ...}V...zT5
000000d0: 7867 a5b1 384c 5399 aac3 0274 3dbd a54b  xg..8LS....t=..K
000000e0: 886e 067d 0191 a7ce c088 ceaa 42bd 2338  .n.}........B.#8
000000f0: efb1 0622 c6ec 8f73 57f0 2d15 c905 4c99  ..."...sW.-...L.
00000100: 046e 9526 5d4b 193f d5a3 0096 59e9 a506  .n.&]K.?....Y...
00000110: 5c47 e33e 16b2 56af fc3f ded2 c9cf fdab  \G.>..V..?......
00000120: 13b3 a786 bbdc 4f6f 6593 7457 90a4 ea2c  ......Ooe.tW...,
00000130: d478 34c3 dbe2 804e 524d 0b6c 5104 269e  .x4....NRM.lQ.&.
00000140: 699d 2b13 b934 02f3 8829 69d2 d624 4ae9  i.+..4...)i..$J.
00000150: 322c fb9c 0e03 72f2 cc2d c49a 3639 8fd9  2,....r..-..69..
00000160: f8ad b96c 09cf 4da7 899e 71a5 795c 417f  ...l..M...q.y\A.
00000170: 1483 4f6d c52c df66 914f e70b 011d ae59  ..Om.,.f.O.....Y
$ openssl dgst -sha512 -verify public_key.pem -signature sha512_signature.sign plaintexts/lorem_ipsum.txt
Verified OK
```

Vamos ahora con el algoritmo [**ECC**][16]$^{16}$. Primero, vemos la lista de posibles curvas:

Conviene echar un vistazo a qué curvas son adecuadas y cuáles no (consultar [bibliografía][17]$^{17}$). La lista anterior no indica si las curvas son seguras o no, sino realmente cuáles tienen un grado de seguridad más avanzado. En nuestro caso, procederemos con la curva P-256 (brainpoolP256r1), no necesitamos ningún requerimiento para el ejemplo, aunque dependiendo del uso que le demos al cifrado de clave pública (si es por ejemplo para un servidor ssl) habría que tener en cuenta [más factores][18]$^{18}$.

```text
$ openssl ecparam -list_curves
  secp112r1 : SECG/WTLS curve over a 112 bit prime field
  secp112r2 : SECG curve over a 112 bit prime field
  [...]
  secp192k1 : SECG curve over a 192 bit prime field
  secp224k1 : SECG curve over a 224 bit prime field
  [...]
  wap-wsg-idm-ecid-wtls5: X9.62 curve over a 163 bit binary field
  [...]
  wap-wsg-idm-ecid-wtls12: WTLS curve over a 224 bit prime field
  Oakley-EC2N-3: 
	IPSec/IKE/Oakley curve #3 over a 155 bit binary field.
	Not suitable for ECDSA.
	Questionable extension field!
  Oakley-EC2N-4: 
	IPSec/IKE/Oakley curve #4 over a 185 bit binary field.
	Not suitable for ECDSA.
	Questionable extension field!
  brainpoolP160r1: RFC 5639 curve over a 160 bit prime field
  brainpoolP160t1: RFC 5639 curve over a 160 bit prime field
  [...]
  brainpoolP256r1: RFC 5639 curve over a 256 bit prime field
  brainpoolP256t1: RFC 5639 curve over a 256 bit prime field
  [...]
  brainpoolP512r1: RFC 5639 curve over a 512 bit prime field
  brainpoolP512t1: RFC 5639 curve over a 512 bit prime field
  SM2       : SM2 curve over a 256 bit prime field
```

Vamos ahora a probar con **ECC**:

```text
$ openssl ecparam -name brainpoolP256r1 -genkey -noout -out private_key.pem
$ openssl ec -in private_key.pem -pubout -out public_key.pem
read EC key
writing EC key
$ openssl req -new -x509 -key private_key.pem -out certificate.pem
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [AU]:
State or Province Name (full name) [Some-State]:
Locality Name (eg, city) []:
Organization Name (eg, company) [Internet Widgits Pty Ltd]:
Organizational Unit Name (eg, section) []:
Common Name (e.g. server FQDN or YOUR name) []:
Email Address []:
$ openssl cms -encrypt -binary -aes-256-cbc -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt certificate.pem
$ openssl cms -encrypt -decrypt -in ciphertexts/lorem_ipsum.txt -out decryptedtexts/lorem_ipsum.txt -inkey private_key.pem
$ diff <(xxd plaintexts/lorem_ipsum.txt) <(xxd ciphertexts/lorem_ipsum.txt) | wc -c
4330
$ diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c
0
$ openssl cms -sign -binary -in plaintexts/lorem_ipsum.txt -out signed_lorem_ipsum.txt -inkey private_key.pem -signer certificate.pem -nodetach
$ openssl cms -verify -in signed_lorem_ipsum.txt -out verified_lorem_ipsum.txt -CAfile certificate.pem
Verification successful
```

### Diferencias entre velocidades de cifrados simétricos y asimétricos

 <!-- TODO -->

 Podemos ejecutar el comando `openssl speed` directamente, y esperar a que termine. No suele ser la mejor opción, ya que nos va a comparar todo. $^{23}$

 Así, haremos la comparación entre el algoritmo **AES** y **ECC** (en este último, para la versión **ECDH** basada en intercambio de claves Diffie-Hellman $^{24}$), así como entre **DES**, **RSA** y **DSA**; aunque realmente será como estar comparando los 5, salvo que por no saturar demasiado la ejecución, lo separaremos en 3 distintas:

 ```text
$ openssl speed -evp aes-256-cbc ecdh
Doing aes-256-cbc for 3s on 16 size blocks: 90881814 aes-256-cbc's in 2.99s
Doing aes-256-cbc for 3s on 64 size blocks: 36997062 aes-256-cbc's in 3.00s
Doing aes-256-cbc for 3s on 256 size blocks: 9887755 aes-256-cbc's in 3.00s
Doing aes-256-cbc for 3s on 1024 size blocks: 2513202 aes-256-cbc's in 3.00s
Doing aes-256-cbc for 3s on 8192 size blocks: 314011 aes-256-cbc's in 2.99s
Doing aes-256-cbc for 3s on 16384 size blocks: 156798 aes-256-cbc's in 3.00s
Doing 160 bits  ecdh's for 10s: 38704 160-bits ECDH ops in 9.99s
Doing 192 bits  ecdh's for 10s: 29730 192-bits ECDH ops in 9.98s
Doing 224 bits  ecdh's for 10s: 92497 224-bits ECDH ops in 9.99s
Doing 256 bits  ecdh's for 10s: 141508 256-bits ECDH ops in 10.00s
Doing 384 bits  ecdh's for 10s: 7853 384-bits ECDH ops in 9.98s
Doing 521 bits  ecdh's for 10s: 20668 521-bits ECDH ops in 10.00s
Doing 163 bits  ecdh's for 10s: 37540 163-bits ECDH ops in 9.98s
Doing 233 bits  ecdh's for 10s: 27588 233-bits ECDH ops in 9.99s
Doing 283 bits  ecdh's for 10s: 17364 283-bits ECDH ops in 10.00s
Doing 409 bits  ecdh's for 10s: 10683 409-bits ECDH ops in 9.99s
Doing 571 bits  ecdh's for 10s: 5298 571-bits ECDH ops in 10.00s
Doing 163 bits  ecdh's for 10s: 38316 163-bits ECDH ops in 9.97s
Doing 233 bits  ecdh's for 10s: 28163 233-bits ECDH ops in 9.98s
Doing 283 bits  ecdh's for 10s: 15595 283-bits ECDH ops in 9.96s
Doing 409 bits  ecdh's for 10s: 8778 409-bits ECDH ops in 9.98s
Doing 571 bits  ecdh's for 10s: 4247 571-bits ECDH ops in 9.98s
Doing 256 bits  ecdh's for 10s: 21524 256-bits ECDH ops in 9.99s
Doing 256 bits  ecdh's for 10s: 22247 256-bits ECDH ops in 9.98s
Doing 384 bits  ecdh's for 10s: 9369 384-bits ECDH ops in 9.98s
Doing 384 bits  ecdh's for 10s: 9152 384-bits ECDH ops in 9.99s
Doing 512 bits  ecdh's for 10s: 5463 512-bits ECDH ops in 9.99s
Doing 512 bits  ecdh's for 10s: 5885 512-bits ECDH ops in 9.99s
Doing 253 bits  ecdh's for 10s: 240340 253-bits ECDH ops in 10.00s
Doing 448 bits  ecdh's for 10s: 18933 448-bits ECDH ops in 9.99s
OpenSSL 1.1.1f  31 Mar 2020
built on: Wed Aug 25 01:13:44 2021 UTC
options:bn(64,64) rc4(8x,int) des(int) aes(partial) blowfish(ptr) 
compiler: gcc -fPIC -pthread -m64 -Wa,--noexecstack -Wall -Wa,--noexecstack -g -O2 -fdebug-prefix-map=/build/openssl-xajkZk/openssl-1.1.1f=. -fstack-protector-strong -Wformat -Werror=format-security -DOPENSSL_TLS_SECURITY_LEVEL=2 -DOPENSSL_USE_NODELETE -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAESNI_ASM -DVPAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPOLY1305_ASM -DNDEBUG -Wdate-time -D_FORTIFY_SOURCE=2
The 'numbers' are in 1000s of bytes per second processed.
type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
aes-256-cbc     486324.09k   789270.66k   843755.09k   857839.62k   860327.13k   856326.14k
                              op      op/s
 160 bits ecdh (secp160r1)   0.0003s   3874.3
 192 bits ecdh (nistp192)   0.0003s    2979.0
 224 bits ecdh (nistp224)   0.0001s    9259.0
 256 bits ecdh (nistp256)   0.0001s    14150.8
 384 bits ecdh (nistp384)   0.0013s    786.9
 521 bits ecdh (nistp521)   0.0005s    2066.8
 163 bits ecdh (nistk163)   0.0003s    3761.5
 233 bits ecdh (nistk233)   0.0004s    2761.6
 283 bits ecdh (nistk283)   0.0006s    1736.4
 409 bits ecdh (nistk409)   0.0009s    1069.4
 571 bits ecdh (nistk571)   0.0019s    529.8
 163 bits ecdh (nistb163)   0.0003s    3843.1
 233 bits ecdh (nistb233)   0.0004s    2821.9
 283 bits ecdh (nistb283)   0.0006s    1565.8
 409 bits ecdh (nistb409)   0.0011s    879.6
 571 bits ecdh (nistb571)   0.0023s    425.6
 256 bits ecdh (brainpoolP256r1)   0.0005s    2154.6
 256 bits ecdh (brainpoolP256t1)   0.0004s    2229.2
 384 bits ecdh (brainpoolP384r1)   0.0011s    938.8
 384 bits ecdh (brainpoolP384t1)   0.0011s    916.1
 512 bits ecdh (brainpoolP512r1)   0.0018s    546.8
 512 bits ecdh (brainpoolP512t1)   0.0017s    589.1
 253 bits ecdh (X25519)   0.0000s  24034.0
 448 bits ecdh (X448)   0.0005s    1895.2

$ openssl speed -evp des-cbc rsa
Doing des-cbc for 3s on 16 size blocks:    14011283 des-cbc's in 3.00s
Doing des-cbc for 3s on 64 size blocks:    3720650 des-cbc's in 3.00s
Doing des-cbc for 3s on 256 size blocks:   941334 des-cbc's in 3.01s
Doing des-cbc for 3s on 1024 size blocks:  236877 des-cbc's in 3.00s
Doing des-cbc for 3s on 8192 size blocks:  29093 des-cbc's in 3.00s
Doing des-cbc for 3s on 16384 size blocks: 14783 des-cbc's in 3.00s
Doing 512 bits private rsa's for 10s: 242748 512 bits private RSA's in 10.01s
Doing 512 bits public rsa's for 10s: 3545602 512 bits public RSA's in 10.00s
Doing 1024 bits private rsa's for 10s: 86546 1024 bits private RSA's in 10.01s
Doing 1024 bits public rsa's for 10s: 1400973 1024 bits public RSA's in 10.00s
Doing 2048 bits private rsa's for 10s: 12848 2048 bits private RSA's in 10.01s
Doing 2048 bits public rsa's for 10s: 429031 2048 bits public RSA's in 10.00s
Doing 3072 bits private rsa's for 10s: 4104 3072 bits private RSA's in 10.01s
Doing 3072 bits public rsa's for 10s: 201770 3072 bits public RSA's in 10.00s
Doing 4096 bits private rsa's for 10s: 1798 4096 bits private RSA's in 10.00s
Doing 4096 bits public rsa's for 10s: 115148 4096 bits public RSA's in 10.00s
Doing 7680 bits private rsa's for 10s: 200 7680 bits private RSA's in 10.02s
Doing 7680 bits public rsa's for 10s: 33439 7680 bits public RSA's in 10.00s
Doing 15360 bits private rsa's for 10s: 38 15360 bits private RSA's in 10.24s
Doing 15360 bits public rsa's for 10s: 8721 15360 bits public RSA's in 10.01s
OpenSSL 1.1.1f  31 Mar 2020
built on: Wed Aug 25 01:13:44 2021 UTC
options:bn(64,64) rc4(8x,int) des(int) aes(partial) blowfish(ptr) 
compiler: gcc -fPIC -pthread -m64 -Wa,--noexecstack -Wall -Wa,--noexecstack -g -O2 -fdebug-prefix-map=/build/openssl-xajkZk/openssl-1.1.1f=. -fstack-protector-strong -Wformat -Werror=format-security -DOPENSSL_TLS_SECURITY_LEVEL=2 -DOPENSSL_USE_NODELETE -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAESNI_ASM -DVPAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPOLY1305_ASM -DNDEBUG -Wdate-time -D_FORTIFY_SOURCE=2
The 'numbers' are in 1000s of bytes per second processed.
type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes
des-cbc          74726.84k    79373.87k    80060.30k    80854.02k    79443.29k    80734.89k
                  sign    verify    sign/s verify/s
rsa  512  bits 0.000041s 0.000003s  24250.5 354560.2
rsa 1024  bits 0.000116s 0.000007s   8646.0 140097.3
rsa 2048  bits 0.000779s 0.000023s   1283.5  42903.1
rsa 3072  bits 0.002439s 0.000050s    410.0  20177.0
rsa 4096  bits 0.005562s 0.000087s    179.8  11514.8
rsa 7680  bits 0.050100s 0.000299s     20.0   3343.9
rsa 15360 bits 0.269474s 0.001148s      3.7    871.2

$ openssl speed dsa
Doing 512 bits sign dsa's for 10s: 157269 512 bits DSA signs in 9.97s
Doing 512 bits verify dsa's for 10s: 224381 512 bits DSA verify in 10.00s
Doing 1024 bits sign dsa's for 10s: 81123 1024 bits DSA signs in 9.97s
Doing 1024 bits verify dsa's for 10s: 98208 1024 bits DSA verify in 9.95s
Doing 2048 bits sign dsa's for 10s: 29644 2048 bits DSA signs in 9.97s
Doing 2048 bits verify dsa's for 10s: 31818 2048 bits DSA verify in 9.97s
OpenSSL 1.1.1f  31 Mar 2020
built on: Wed Aug 25 01:13:44 2021 UTC
options:bn(64,64) rc4(8x,int) des(int) aes(partial) blowfish(ptr) 
compiler: gcc -fPIC -pthread -m64 -Wa,--noexecstack -Wall -Wa,--noexecstack -g -O2 -fdebug-prefix-map=/build/openssl-xajkZk/openssl-1.1.1f=. -fstack-protector-strong -Wformat -Werror=format-security -DOPENSSL_TLS_SECURITY_LEVEL=2 -DOPENSSL_USE_NODELETE -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAESNI_ASM -DVPAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPOLY1305_ASM -DNDEBUG -Wdate-time -D_FORTIFY_SOURCE=2
                  sign    verify    sign/s verify/s
dsa  512 bits 0.000063s 0.000045s  15774.2  22438.1
dsa 1024 bits 0.000123s 0.000101s   8136.7   9870.2
dsa 2048 bits 0.000336s 0.000313s   2973.3   3191.4
 ```

Con estos datos, podemos observar en una gráfica una tabla comparando el número de veces que pueden ejecutarse sobre lo que se considera una ejecución (procesamiento de un _bytearray_ de $1000 bytes$):

![aes_performance](img/aes_performance_speed.png)

![ecdh_performance](img/ecdh_performance_speed.png)

![des_performance](img/des_performance_speed.png)

![rsa_performance](img/rsa_performance_speed.png)

![dsa_performance](img/dsa_performance_speed.png)

Respecto a los simétricos, vemos que su velocidad es prácticamente la misma, apenas hay diferencias notables (aunque uno sea más seguro que el otro, lo que haría a **AES** algo más eficiente en términos de seguridad).

Por otro lado, las curvas elípticas para el intercambio basado en esquema Diffie-Hellman, vemos que hay una en concreto que destaca sobre las demás, la curva [*Curve25519*][25]$^{25}$, que con una clave de $256 bits$ ofrece una seguridad de $128$, pero que resulta ser extremadamente rápida. Realmente la curva del esquema Diffie-Hellman es la *X25519*, pero la curva subyacente es la citada, puesto que esta última queda definida como una función *Diffie-Hellman*. Es la que *openssh* utiliza por defecto en el modo **ECDH**. De hecho, vemos la diferencia respecto a los esquemas **RSA** y **DSA** en las gráficas; por lo que considerando que una clave pública de un esquema **RSA** en comparación a uno basado en **ECC** es 6 veces superior, y hasta 12 veces en el caso de la clave privada, las diferencias son notables.

Finalmente, observar las diferencias entre **RSA** y **DSA**. **DSA** es más estable que **RSA** en las dos partes del proceso (firma y verificación), aunque bastante más lento que **RSA** en la parte de verificación (que, sin embargo, cae exponencialmente conforme incrementamos el tamaño de clave).

### Certificados X.509

Los problemas de la capa de transporte (nivel 4 según el estándar OSI) residen principalmente en la vulnerabilidad a ataques _man in the middle_, _spoofing_ o _replay attack_. De esta forma, puesto que encontramos entre los usos más comunes de los certificados X.509 algunos de los siguientes:

  - Cifrado web con SSL (Secure Socket Layer), nivel 3.5 entre capa de aplicación y transporte.
  - Cifrado y certificación de correos electrónicos mediante el protocolo S/MIME.
  - Firma digital
  - Autenticación y no repudio

otorgan una confianza entre las partes implicadas en la comunicación.

Debido a su estructura, pueden contener información relevante como nombre del sujeto que se está certificando, país, región, provincia, estado, localidad, organización...

De hecho, utilizamos uno de estos en el [apartado de cifrados asimétricos](#cifrados-asimétricos), donde hicimos primero el ejemplo con RSA y luego con curvas elípticas, siendo en este último necesario hacerlo con certificados para la fase de firma y verificación.

Ahora generaremos un certificado auto-firmado (_self-signed certificate_), que aunque en la red sean inseguros, para el ejemplo que nos concierne es suficiente. $^{26}$

```text
$ openssl req -x509 -newkey rsa:3072 -keyout private_key.pem -out certificate.pem -sha512 -nodes -subj '/CN=localhost'
Generating a RSA private key
......................++++
............................................................................................................................++++
writing new private key to 'private_key.pem'
-----
```

Ponemos el _flag_ `-nodes` para que no se nos pida una _passphrase_ (contraseña), y el _flag_ `-subj` para indicarle quién será el sujeto del certificado, y así evitar que se nos solicite introducir más información. Si hubiésemos querido ponerle expiración concreta, podríamos haber añadido el flag `-days`.

Vemos los valores del certificado:

```text
$ openssl x509 -text -noout -in certificate.pem
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number:
            6a:9d:f0:79:aa:2d:6c:e8:06:fd:18:49:ce:6b:42:ce:ee:fd:e7:92
        Signature Algorithm: sha512WithRSAEncryption
        Issuer: CN = localhost
        Validity
            Not Before: Dec 12 11:08:19 2021 GMT
            Not After : Jan 11 11:08:19 2022 GMT
        Subject: CN = localhost
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                RSA Public-Key: (3072 bit)
                Modulus:
                    00:d7:b3:ea:88:dd:14:4d:29:98:e7:f0:2e:f3:2b:
                    [...]
                    4f:fc:95:f0:c4:de:69:51:aa:29
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Subject Key Identifier: 
                91:B5:53:54:5A:F9:FE:FE:10:54:0D:DA:8E:E9:86:8E:0F:03:94:B7
            X509v3 Authority Key Identifier: 
                keyid:91:B5:53:54:5A:F9:FE:FE:10:54:0D:DA:8E:E9:86:8E:0F:03:94:B7

            X509v3 Basic Constraints: critical
                CA:TRUE
    Signature Algorithm: sha512WithRSAEncryption
         86:90:96:f1:77:0c:6a:a2:b4:03:d4:10:b9:39:a7:c7:8a:98:
         [...]
         24:20:3d:42:d4:f9
```

Podemos ver la información que contiene, como el algoritmo _hash_ que se ha utilizado para firmarlo; período de validez; algoritmo asimétrico utilizado (**RSA**), así como la cantidad de $bits$ generados; etc.

Así, el certificado quedó firmado y podemos verificarlo de la siguiente forma:

```text
$ openssl x509 -noout -modulus -in certificate.pem | openssl sha512
(stdin)= 33f2dbaf2bfc3ea6f54169bebe3c9ab9efbb07afb91cf8c8737750423a7f544d9adc9a12dd884c24aa0ce286f88460bef12db4d9e733199db4f4f73ef56e1fb5
$ openssl rsa -noout -modulus -in private_key.pem | openssl sha512
(stdin)= 33f2dbaf2bfc3ea6f54169bebe3c9ab9efbb07afb91cf8c8737750423a7f544d9adc9a12dd884c24aa0ce286f88460bef12db4d9e733199db4f4f73ef56e1fb5
$ diff <(openssl x509 -noout -modulus -in certificate.pem | openssl sha512) <(openssl rsa -noout -modulus -in private_key.pem | openssl sha512) | wc -c
0
```

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

\[14]: https://www.openssl.org/docs/manmaster/man1/rsautl.html

\[15]: https://linux.die.net/man/1/rsautl

\[16]: https://linux.die.net/man/1/ec

\[17]: http://safecurves.cr.yp.to/

\[18]: https://security.stackexchange.com/questions/78621/which-elliptic-curve-should-i-use

\[19]: https://www.geeksforgeeks.org/rsa-algorithm-cryptography/

\[20]: https://www.teletrust.de/fileadmin/files/oid/oid_pkcs-3v1-4.pdf

\[21]: https://www.simplilearn.com/tutorials/cryptography-tutorial/digital-signature-algorithm

\[22]: https://avinetworks.com/glossary/elliptic-curve-cryptography

\[23]: https://www.openssl.org/docs/man1.1.1/man1/openssl-speed.html

\[24]: https://es.wikipedia.org/wiki/Elliptic-curve_Diffie-Hellman

\[25]: https://es.wikipedia.org/wiki/Curve25519

\[26]: https://www.openssl.org/docs/manmaster/man1/req.html

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
[14]: https://www.openssl.org/docs/manmaster/man1/rsautl.html
[15]: https://linux.die.net/man/1/rsautl
[16]: https://linux.die.net/man/1/ec
[17]: http://safecurves.cr.yp.to/
[18]: https://security.stackexchange.com/questions/78621/which-elliptic-curve-should-i-use
[19]: https://www.geeksforgeeks.org/rsa-algorithm-cryptography/
[20]: https://www.teletrust.de/fileadmin/files/oid/oid_pkcs-3v1-4.pdf
[21]: https://www.simplilearn.com/tutorials/cryptography-tutorial/digital-signature-algorithm
[22]: https://avinetworks.com/glossary/elliptic-curve-cryptography
[23]: https://www.openssl.org/docs/man1.1.1/man1/openssl-speed.html
[24]: https://es.wikipedia.org/wiki/Elliptic-curve_Diffie-Hellman
[25]: https://es.wikipedia.org/wiki/Curve25519
[26]: https://www.openssl.org/docs/manmaster/man1/req.html