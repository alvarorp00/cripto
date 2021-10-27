# Modificación del cifrado afín

Se procede a detallar la modificación del cifrador afín propuesto así como su criptoanálisis.

## Motivación

La modificación del cifrado afín propuesto consiste en convertir este criptograma en uno polialfabético, que acepte un vector de claves (vector para el parámetro **-a** y otro vector para el parámetro **-b**, ambos de igual longitud). La norma que tiene que seguir es que cada una de las partes de la cadena de entrada correspondiente al vector **a** debe cumplir la propiedad **gcd(m, a[i]) = 1**, siendo **m** el tamaño del alfabeto sobre el que estamos trabajando.

El código sobre su funcionamiento lo encontraremos en el [apartado siguiente](#criptoanálisis-del-afín-modificado), donde se detalla su criptoanálisis.

Veamos algunos ejemplos de su ejecución y los compararemos con el afín clásico:

```
❯ ./affine -m 26 -a 3 -b 21
 ! @@@ AFFINE CIPHER @@@ ! 
-> Enter message (press CTRL + D in new line to finish): 
HOLA MUNDO, ESTO ES UN TEXTO DE PRUEBA
QLCVFDIELHXALHXDIAHMALEHOUDHYV

❯ ./affine_mod -m 26 -a DFFH -b ABCD
 ! @@@ AFFINE CIPHER MOD @@@ ! 
-> Enter message (press CTRL + D in new line to finish): 
HOLA MUNDO, ESTO ES UN TEXTO DE PRUEBA
VTFDKXPYQVOGQVONNSWIFTRFTIYFDB
```

La diferencia está en que el valor de la letra en el alfabeto (A:0; ...; Z:25) se corresponderá con el valor numérico de cada parte del vector de la clave.

Para cifrar puede especificarse la opción `-C` o no ponerse nada. Sin embargo, para descifrar una cadena, necesita especificarse la opción `-D`.

Desciframos lo anterior:

```
❯ ./affine_mod -m 26 -a DFFH -b ABCD -D
 ! @@@ AFFINE CIPHER MOD @@@ ! 
-> Enter message (press CTRL + D in new line to finish): 
VTFDKXPYQVOGQVONNSWIFTRFTIYFDB
HOLAMUNDOESTOESUNTEXTODEPRUEBA
```

Los valores del vector **-a** anteriores se corresponden numéricamente con [3, 5, 5, 7] que efectivamente definen una función inyectiva en $Z_{26}$. Si lo intentamos con, por ejemplo, la cadena "bBCC" que se corresponde con [1, 1, 2, 2] sale lo siguiente:

```
❯ ./affine_mod -m 26 -a BBCC -b EFGH -C
 ! @@@ AFFINE CIPHER MOD @@@ ! 
Failure EUCLIDES for gcd(2, 26)=2
>> Cipher didn't worked as expected: gcd(2, 26) = 2 != 1
```

Por lo que sigue teniendo que cumplirse la propiedad fundamental del cifrador afín sobre el alfabeto en el que se trabaja.

## Criptoanálisis del Afín Modificado

Este criptograma es vulnerable a ataques de texto cifrado conocido, igual que el cifrado afín original; requiere considerar algunas variables más. Lo veremos a continuación.

Las condiciones iniciales y las restricciones son las mismas que aplican para el cifrado afín clásico, pero con el truco de que **gcd(a, m) = 1** debe cumplirse para cada parte del vector: $gcd(a[i], m) = 1, \forall{i} \in [0, 25]$, de forma que describe una función inyectiva sobre $Z_{26}$.

Recordemos como este criptograma cifra y descifra:
```c

/**
  * Using:
  * xz := numeric code of plaintext at i-th position
  * yz := numeric code of ciphertext at i-th position
  * cx := auxiliar variable
  * az := a's key vector
  * az[i] := b's key vector at i-th position
  * bz := b's key vector
  * bz[i] := b's key vector at i-th position
  * mz := alphabet size (26 for english)
  * plaintext := char array with plaintext
  * ciphertext := char array with ciphertext
  * i := index
  * KLENGTH := length of the key
  */

void affine_mod_cipher()
{
  for ( i=0; i<strlen(plaintext); i++ )
  {
    xz = alphabet_num_code( plaintext[i] );
    cx = ( az[( i % KLENGTH )] * xz ) + bz[( i % KLENGTH )];
    ciphertext[i] = cx % mz;
  }
}

void affine_mod_decipher()
{
  for ( i=0; i<strlen(ciphertext); i++ )
  {
    yz = alphabet_num_code( ciphertext[i] );
    cx = multiplicative_inverse( az[( i % KLENGTH )], mz ); // perform multiplicative inverse for az[i] in mz (as gcd() == 1)
    plaintext[i] = ( cx * ( yz - bz[( i % KLENGTH )] ) ) % mz;
  }
}
```

Una vez hemos visto cómo funciona, podemos proceder a la parte del criptoanálisis. Considerar las siguientes aclaraciones.

Para hallar el tamaño de la clave, nos bastaría con hacer exactamente lo mismo que en el cifrado de vigenère. Es decir, un ataque por kasiski combinado con el índice de coincidencia (ver [IC + Kasiski](#índice-de-coincidencia)). Una vez tengamos el tamaño de la clave, procederemos como sigue.

En el cifrado de vigenère, guiamos el ataque según la frecuencia y apariciones de elementos repetidos en el texto:

  - Sin desplazamiento:  $f_0 / n' , ... , f_25 / n'$
  - Con desplazamiento:  $f_ki / n' , ... , f_25+ki / n'$

Nótese que $n'$ hace referencia al número de caracteres cifrados por cada parámetro de la clave. En otras palabras, es la longitud de la subcadena construida para cada valor del vector de la clave (se construye con los caracteres que creen haberse cifrado con el parámetro i-ésimo de la clave, $K_i$).

Pero como no estamos en un cifrador basado en desplazamiento natural, sino en uno basado en el afín clásico:


Tenemos que: $f0 / n' , ... , f25 / n' -> (x + k_i) mod 26 [Vigenère] => (x*a + b) mod 26 [Affine Mod]$

Hay que fijarse en que $f_{k_i}$ en un ataque a vigenère hace referencia al valor de desplazamiento que se le va a aplicar al i-ésimo elemento del alfabeto. Si empezamos a contar en 0, es decir: $[A: 0, ..., Z:25]$, tendremos que: $f_b / n' , ... , f_{25*a+b} / n'$, estando $f_b$ en la primera posición, como si fuera $f_{0*a+b}$, pues es equivalente a desplazarse $b$ posiciones. Fijémonos también en que estamos pasando de tener un número de desplazamientos $g$ (ver [criptoanálisis del cifrado de vigenère](#criptoanálisis-de-vigenère---rotura-mediante-kasiski-e-ic)) a tener un número de valores posibles que va de la siguiente forma: $(a, b)$ $\in$ $Z_{\phi (m)} \times Z_{m}$. Y esto por cada parte del vector de la clave.

En este momento, conviene recordar que el valor ideal de la distribución de probabilidades de los caracteres en el inglés $p_0, ..., p_25$ suscribe la fórmula de arriba sobre módulo 26.

Es decir, $p_i$ hace referencia a la distribución de frecuencias en la tabla dada en el alfabeto (mirar el directorio _config/dictionary.json_, donde se observa la siguiente configuración por cada letra: $[<caracter>, <representacion numerica>, <probabilidad_castellano>, <probabilidad_inglés>]$). Nótese que las probabilidades en el fichero de configuración están en porcentaje, así que para hacer los cálculos han de dividirse entre 100 al cargar el archivo. Para recordar esos valores, puede visitarse el siguiente [enlace](https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html).

De esta forma, en vez de hacer $M_g = \sum_{i=0}^{25}\frac{p_i \times f_{i+g}}{n'}$, calcularíamos $M_g = \sum_{i=0}^{25}\frac{p_i \times f_{a*i + b}}{n'}$; reemplazando $f_{i+g}$ por $f_{a*i + b}$.

Así, la pareja $(a,b)$ que más cerca se quedase del valor $IC$ ideal (0.065) sería la que seleccionásemos para descifrar esa fila (subcadena) de elementos. Una vez hecho esto, tendremos que calcular si la pareja de $(a,b)$ que hemos hallado es posible en $Z_{26}$. Si el cálculo del $\gcd(a,m) \not ={1}$, entonces procederemos con el siguiente que más cerca se quede al valor buscado. Es decir, exactamente lo mismo que en el ataque al cifrado de vigenère pero en vez de buscar sobre el parámetro $g$, el valor con el que se cifró, lo haríamos sobre la tupla $(a,b)$.

Otra forma de hacer el ataque a este cifrado sería hacerlo igual que en el cifrado afín clásico, pero por cada subcadena. Es decir, seguimos utilizando estadística, pues el orden de iteración de caracteres sería de la siguiente forma:

  1. Emparejamos el caracter que más se repite en el texto cifrado con el que más se repite en el lenguaje.
  2. Emparejamos el segundo caracter que más se repite en el texto cifrado con el que más se repite en el lenguaje.
  3. Hallamos el valor de $(a,b)$ por medio de la resolución del sistema de ecuaciones.
  4. Calculamos $\gcd(a,m)$. Si es distinto a $1$ entonces:
     1. Emparejamos el segundo que más se repite con el tercer caracter más repetido en el texto cifrado.
     2. Hallamos el valor de $(a,b)$ resolviendo el sistema
     3. Iterativamente...
  5. Si es válido, desciframos toda la subcadena con los valores de $(a,b)$ calculados.

Es decir, exactamente igual que el cifrado afín. Vamos emparejando hasta que encontremos un valor de $(a,b)$ que nos sirva, guiándonos por prioridad de frecuencias de aparición. Esto es posible ya que el afin propuesto es como hacer un afín distinto por cada subcadena asociada a una parte de la clave.

### Consideraciones respecto al afín original: debilidades y fortalezas.

Encuentra muchas similitudes respecto al cifrado de vigenère, solo que al tener dos parámetros de clave y no únicamente uno es algo más costoso computar los valores de la posible clave. Respecto al cifrado afín original, un texto cifrado con `affine(a, b, m, plaintext)` y con $len(plaintext) = l$, si ciframos otro texto `plaintext'` con `affine_mod(a[], b[], m, plaintext')`, serán igualmente débiles cuando $len(plaintext') = l' \equiv l \times size(\vec{a}) = l \times size(\vec{b})$. Es decir, cuando cada parte de la clave - cada parte del vector $(\vec {a}, \vec {b})$ - cifre un número de caracteres igual al que está cifrando $(a, b)$ en la versión original, pues será el punto donde los ataques basados en frecuencias en el texto se encuentren, generalmente, en condiciones similares.

Funcionará mejor para cadenas no muy largas, puesto que cuando el texto comience a ser suficientemente largo, su seguridad respecto al cifrado afín original no se verá incrementada (al igual que no lo hace tampoco vigenère respecto del cifrado por desplazamiento para textos suficientemente largos, que según vimos en algunas pruebas, a partir de los $\approx$ 20 - 30 caracteres hemos podido romper). Lamentablemente, no hemos podido codificar el criptoanálisis de esta modificación del cifrado afín, por lo que no hemos podido demostrarlo experimentalmente.

## Criptoanálisis del Cifrado de Vigenère

Para este análisis, contaremos con la siguiente estructura:

```c
struct Frequency{
  alphabet_t *alphabet; // must be provided
  size_t a_sz; // alphabet_size, must be provided
  char *textstring; // input text, needs to be set up first
  ssize_t textlen; // length of input text
  struct Param{
    char chr; // chr to search
    double prob; // probability of ocurrence in given text
    uint_fast64_t ocurrences; // number of times chr appears in given text
  } *chrs;
  struct IC{
    char **strs; // store final substrings
    size_t _M; // substrings length == len(Y_i) 
    size_t keylength; // length of the key
    float IC; // index of coincidence value
    bool ok; // return status for IC calc
  } IC;
  struct Kasiski{
    size_t distance; // distance between repeated sequences
    char *str; // string match
    size_t *keycandidates; // keylength candidates
    size_t ncandidates; // number of keylength candidates
    bool ok; // status of kasiski performance
  } Kasiski;
  uint_fast32_t ngram; // ngram used for kasiski and IC test
};
```

Vemos claramente algunos elementos diferenciados. Hay dos mínimos, que son el alfabeto (_alphabet_) y la cadena de texto (_textstring_). Sin estos dos elementos las funciones que la utilizarán no podrán hacer ningún cálculo. La parte de **chrs** es un array que guardará datos asociados a cada uno de los caracteres que portará el alfabeto (en esta práctica, todos aquellos entre el A[65] y Z[90]). La subestructura **IC** guardará información asociada al cálculo del índice de coincidencia, y lo mismo con la subestructura **Kasiski**. En el campo **ngram** se guardará el tamaño del _n-grama_ a utilizar por estos dos métodos de cálculo. Si el proporcionado es _0_ o _NULO_, no se utilizará.

Para poder ejecutar el criptoanálisis del cifrado de vigenere, será suficiente con una cadena de texto cifrado. Es decir, realizaremos un ataque de texto cifrado. Para ello, nos basamos en un análisis de frecuencias de caracteres. Nos apoyamos fundamentalmente en dos métodos:

### Kasiski:

 - Buscamos repeticiones de **n** caracteres (_ngrama_). Se apoya en la idea de que en un cifrador polialfabético, la probabilidad de repetición de secuencias de caracteres de manera aleatoria y no coincidente es fundamentalmente baja - es decir, la probabilidad de que una cadena del texto cifrado se repita y su valor original previo a cifrarse sea diferente, es muy baja -.
 - Para ejecutar el programa y hacer análisis de distancias (sin más información que esta), bastará con ejecutar: **./kasiski -m "longitud alfabeto" -l "longitud ngrama" -i "fichero entrada" -o "fichero salida"
 - Igual que en las otras versiones proporcionadas, en caso de no ofrecer parámetro de entrada y salida para los ficheros, se usarán los valores estándar (**stdin** y **stdout**).

Ahora bien, lo único que haremos con el anterior comando (./kasiski...) será llamar a otro método, el que vemos justo aquí debajo. Cargará el parámetro correspondiente a la estructura que se ha presentado anteriormente con el alfabeto a utilizar y el texto completo leído (todo lo que se ha leído desde el fichero de entrada).

```c
static void _kasiski(struct Frequency *freq)
{
  size_t i, j, k; // indexes

  char buffer[KB1 + 1];
  
  if (!freq)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "No frequency struct given");
    goto _end_kasiski;
  }

  freq->Kasiski.ok = false;

  if (freq->ngram == 0)
    goto _end_kasiski;

  /**
  * Does not check if keylength matched
  * divides whole text length as
  * in vigenere is possible to not fit
  * text length with key
  * 
  */

  // search for repeated sequences...
  for ( i=0; i<freq->textlen; i++ )
  {
    for ( j=i+1; j<freq->textlen; j++ )
    {
      for ( k=0; freq->textstring[i+k] == freq->textstring[j+k]; k++ )
        buffer[k] = freq->textstring[i+k]; // copy value
      buffer[k] = '\0'; // trailing 0!
      if ( k>=freq->ngram && (( j - i ) % k == 0) ) // greater or equal than value searched && distance divisible by ngram length
      {
        freq->Kasiski.ok = true;
        freq->Kasiski.distance = ( j - i ); // difference between two compared indexes
        freq->Kasiski.keycandidates = _get_divisors(freq->Kasiski.distance, &(freq->Kasiski.ncandidates));
        freq->Kasiski.str = (char*)calloc(k, sizeof(char));
        if (!freq->Kasiski.str)
          goto _end_kasiski;
        strncpy(freq->Kasiski.str, buffer, k);
        goto _end_kasiski;
      }
    }
  }

  _end_kasiski:
    return;
}
```

Como podemos observar, lo que este código está haciendo es comprobar primero que la longitud del **n-grama** es superior a 0. Después de esto, comienza iterando sobre un índice (**i**) primero y otro (**j**) segundo. El segundo comienza inmediatamente en la posición consecutiva a la del primero y comprueba si hay un índice coincidente. En caso de no haberlo, sigue desplazándose hasta que encuentra un caracter coincidente con el del primer índice. En este momento, va a desplazarse **k** veces, que será el número de caracteres que coinciden entre las dos cadenas en distancia (**i** - **j**). Si esta distancia **k** es superior al valor del **n-grama** que se está buscando, entonces aceptaremos la distancia hallada como válida. No se sigue buscando, ya que se considera que la probabilidad de que estas dos cadenas vengan de valores de texto claro diferentes es extremadamente baja. También comprueba que la distancia hallada (**i** - **j**) es múltiplo de la longitud del **n-grama**, ya que esta distancia sí o sí tiene que ser múltiplo de la clave (lo que conlleva que la probabilidad de que se haga este _match_ por error sea aún más baja) para que esta propiedad se cumpla.

Ahora, se guarda el valor de la variable **ok** como positivo (pues ha encontrado el **n-grama**), guarda la distancia entre los dos índices, guarda la cadena del **n-grama** hallado (caracteres) y finalmente calcula los candidatos de la clave. Esto último será usado por el Índice de Coincidencia. Este array va a guardar todos aquellos valores que dividen a la distancia hallada y que son candidatos a ser la longitud de la clave.

Como veremos ahora, el Índice de Coincidencia puede no usar esto, pero la probabilidad de que acierte se reduce considerablemente.

Un ejemplo de la ejecución de este programa es el siguiente (nótese que se corresponde con el ejemplo del libro `Cryptography: Theory and Practice - Stinson & Paterson`):

```
❯ bat ciphertexts/2cipher.txt
───────┬───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
       │ File: ciphertexts/2cipher.txt
───────┼───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1   │ CHREEVOAHM...
   ────┴───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

❯ make kasiski
Compiling all...
Executable generated successfully!

❯ ./kasiski -m 26 -l 3 -i ciphertexts/2cipher.txt
 ! @@@ KASISKI TEST @@@ ! 
Kasiski Results [for n-gram of length 3]: 
         Distance: 165
         String matched: CHR
         Candidates [total: 7]:
                 -> 3
                 -> 5
                 -> 11
                 -> 15
                 -> 33
                 -> 55
                 -> 165

❯ ./kasiski -m 26 -l 8 -i ciphertexts/2cipher.txt
 ! @@@ KASISKI TEST @@@ ! 
>> Cipher didn't worked as expected: Kasiski couldn't find substr matching required n-gram
```

Para cifrar puede especificarse la opción `-C` o no ponerse nada. Sin embargo, para descifrar una cadena, necesita especificarse la opción `-D`.


### Índice de Coincidencia

El **índice de coincidencia** de una cadena **x** se denota de la forma $I_{c}(x)$(**x**), y define la probabilidad de que dos elementos escogidos de forma aleatoria en esta cadena sean idénticos.

Para aplicar el índice de coincidencia vamos a utilizar la misma estructura de datos que se ha planteado hace un momento. Como vemos, tiene su propio apartado **IC** donde trabajará con los datos relacionados con el índice de coincidencia.

Nos basamos fundamentalmente en lo siguiente:

  - Conocemos las frecuencias de los caracteres tanto en el inglés como en el castellano. En este caso se utilizarán únicamente las del inglés. Considerar las frecuencias de A, B, ..., Z como f1, f2, ..., f25. De encontrarse estas en una cadena de caracteres **x**, la posibilidad de sacar dos elementos idénticos se expresa mediante un coeficiente binomial: $I_{c}(x)$ = $\frac{ \sum_{i=0}^{25}(f_i)(f_i -1) }{n(n-1)}$.

  - Si consideramos la cadena **x** como parte del idioma escogido (inglés), vamos a ver que, en general, $I_{c}(x)$ $\approx$	$\sum_{i=0}^{25} p_i^2 = 0.065$ puesto que la probabilidad de que dos elementos aleatorios sean, por ejemplo, _A_, es $p_0^2$, y lo demás con el resto.
  - De esta forma, considerando las subcadenas $Y_i$, 1$\leq$i$\leq m$ tendremos colocados cada elemento cifrado por la parte $i$ de la clave en su subcadena correspondiente (habiendo partido el texto en cadenas de tamaño $n'$, siendo $n'=m/n$, con **m** como longitud del texto cifrado y **n** como el número de subcadenas en que hemos dividido). Así, en cada subcadena $Y_i$ se encontrarán todos los caracteres cifrados con la $i$-ésima posición de la clave utilizada.
  - Si las subcadenas se construyen de esta forma y $m$ se aproxima a la longitud de la clave entonces tendremos una situación muy parecida a la del caso general, siendo así muy cercana a 0.065. Si tuviesemos una cadena completamente aleatoria, entonces la probabilidad de aparición de un caracter estaría repartida con equiprobabilidad: $I_{c}$ $\approx$ 26 $(\frac{1}{26})^2$ = $\frac{1}{26}$ = 0.038.
  
A partir de esto, nos centraremos en buscar longitudes de cadenas que nos acerquen al valor ideal y nos alejen al mismo tiempo del valor equiprobable (el cuál no es real en un lenguaje).

```c
static void _IC(struct Frequency *freq, bool use_kasiski_candidates)
{

  size_t m, i, j, k, c;
  size_t _M;

  char **substr;
  float IC;

  struct IC *ic;

  if (!freq)
  {
    ERROR...
  }

  ic = &(freq->IC);
  ic->ok = false;

  freq->chrs = (struct Param*)calloc(alphabet_getCurrentSize(freq->alphabet), sizeof(struct Param));

  if (!freq->chrs)
  {
    ERROR...
  }

  freq->a_sz = alphabet_getCurrentSize(freq->alphabet);
  ic->IC = 0;
  ic->_M = 0;
  ic->keylength = 0;

  // m stands for key length...

  for (i = 0; i < freq->textlen; i++)
  { 
    if (use_kasiski_candidates && i >= freq->Kasiski.ncandidates)
      break;
    m = (use_kasiski_candidates) ? freq->Kasiski.keycandidates[i] : ( i + 2 );
    substr = (char**)calloc(m, sizeof(char*));
    if (!substr)
    {
      ERROR...
    }

    IC = 0.0;
    for (j = 0; j < m; j++)
    {
      _M = (size_t)ceil((float)(freq->textlen) / (float)(m)); 
      substr[j] = (char*)calloc(_M + 1, sizeof(char)); // +1 for trailing '\0'
      if (!substr[j])
      {
        ERROR...
      }

      for (c = 0, k = j; k < freq->textlen; k += m, c++)
        substr[j][c] = freq->textstring[k];
      substr[j][c] = '\0';
      _computeFrequency(freq, substr[j], c);
      IC += _computePartialIC(freq, c);

      if (substr[j])
        free(substr[j]);
    }
    IC /= m;

    if (fabs(IC - ENG_IC) < fabs(ic->IC - ENG_IC))
    {
      ic->IC = IC;
      ic->_M = _M;
      ic->keylength = m;
      if (fabs(ic->IC - ENG_IC) <= IC_THRESHOLD)
        break;
    }

    if (substr)
      free(substr);
  }

  ic->strs = (char**)calloc(ic->keylength, sizeof(char*));
  if (!ic->strs)
  {
    ERROR...
  }

  for (j = 0; j < ic->keylength; j++)
  {
    ic->strs[j] = (char*)calloc(ic->_M + 1, sizeof(char)); // +1 for trailing '\0'
    if (!ic->strs[j])
    {
      ERROR...
    }
    for (c = 0, k = j; k < freq->textlen; k += ic->keylength, c++)
      ic->strs[j][c] = freq->textstring[k];
    ic->strs[j][c] = '\0';
  }
  ic->ok = true;

  _end_IC:
    return;
}
```

Primero que todo, vemos que además de la estructura, recibe un flag `use_kasiski_candidates`. Este flag nos permitirá diferenciar entre comprobar todos los valores posibles entre los que podemos partir la cadena de entrada (longitudes de clave desde 1 hasta la misma longitud del texto) o solamente los que el test de kasiski arroja como altamente probables para ser longitud de clave (es decir, todos los valores entre los que podremos dividir la distancia que el test de kasiski ha encontrado). Para cada uno de estos valores (sean las distintas longitudes de la cadena total de entrada, que no tienen que coincidir en longitud ya que el _padding_ no es necesario, pues puede haber un trozo de una cadena que no haya llegado a cifrarse con la última parte de la clave; sean las disntitas longitudes calculadas en el método anterior) se procede de la siguiente forma:
  1. Iniciamos poniendo el valor del IC resultante a 0. Lo mismo con la longitud de la cadena que se cree haber hayado.
  2. Escogemos el número de subcadenas en las que vamos a partir el texto (según lo que se acaba de contar). Este punto es algo sutil, pues realmente estamos haciendo lo siguiente:
     - Si el valor que estamos comprobando para la longitud de la cadena es **klength**, calculamos **_M** de la siguiente forma: **_M = strlen(texstring) / klength**. De esta forma, en **_M** tendremos la cantidad de caracteres que cada parte de la clave habrá cifrado como mucho.
  3. Reservamos el número de subcadenas correspondiente al valor de la longitud de la clave, y por cada una de estas reservamos el número de caracteres que cada una habría cifrado, pero utilizando la función **ceil** puesto que si la división no es entera, una cadena podría quedar más corta. Así ajustamos el valor, por lo que podría darse el caso de que la subcadena correspondiente a la(s) última(s) parte(s) de la clave estuviera(n) rellena(s) de 0's, aunque como veremos, no es ningún problema.
  4. Por cada subcadena correspondiente a los caracteres cifrados por la i-ésima posición de la clave, copiamos los caracteres que esta habría cifrado. Para ello, por cada iteración **j** copiamos el **j-ésimo** caracter desplazado un número **i** de veces (para que se corresponda con el trozo de la clave que lo ha cifrado).
  5. Una vez tenemos en la subcadena los caracteres que la posición i-ésima de la cadena hubiera cifrado, calculamos las frecuencias de los caracteres del alfabeto en esa subcadena (llamada a la función `_computeFrequency`), y después calculamos el índice de coincidencia parcial para esa cadena (llamada a `_computePartialIC`).
  6. Este proceso se repite para todas las posiciones de la posible clave. Se suman cada uno de los índices de coincidencia parciales y finalmente se divide entre el número de subcadenas en que lo hemos dividido (es decir, entre la longitud de la clave). Esta media será el índice de coincidencia total para esa longitud de cadena de cifrado.
  7. Aquí comparamos este nuevo valor con el que tenemos guardado en la estructura (que será el valor final). En caso de estar más cerca del índice de coincidencia ideal (0.065), actualizaremos en la estructura (que será donde recuperaremos los valores de respuesta) los valores para la longitud de clave, índice de coincidencia encontrado y número (máximo) de caracteres que cada posición de la clave habría cifrado.
  8. Finalmente, puesto que vamos liberando la memoria asociada a las cadenas calculadas, para el valor resultante de longitud de clave generamos las subcadenas que cada parte de la clave habría cifrado, y las guardamos en el campo `freq->IC.strs`, de donde podremos recuperarlas después. Si la longitud de la clave es **keylength**, entonces tendremos que realizar un análisis de frecuencias este número de veces (equivalente a atacar un número **keylength** de textos cifrados por un cifrador por desplazamiento _shift cipher_).

El código del programa que utilizará esta función `_IC` es el siguiente (vemos que también se apoya en el test de Kasiski si así se especifica):

```c
void IC(const char *m, const char *ngram, FILE *i_file, FILE *o_file)
{
  struct Frequency freq = {0};
  
  char *input = NULL;
  alphabet_t *alphabet = NULL;

  size_t i;
  
  if (!m || !i_file || !o_file)
  {
    ERROR...
  }

  alphabet = alphabet_init(atoi(m));
  if (!alphabet)
  {
    ERROR...
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    ERROR...
  }

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    ERROR...
  }

  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);
  freq.ngram = (ngram != NULL) ? atol(ngram) : 0;

  _kasiski(&(freq));

  if (!freq.Kasiski.ok)
    _IC(&(freq), false);
  else
    _IC(&(freq), true);

  fprintf(o_file, "IC Results [for n-gram of length %ld]: \n", freq.ngram);
  fprintf(o_file, "\t IC found: %f\n", freq.IC.IC );
  fprintf(o_file, "\t Keylength guessed: %ld\n", freq.IC.keylength );
  fprintf(o_file, "\t Cipher Strings (Y_i):\n");
  for ( i=0; i<freq.IC.keylength && i < 10; i++ ) // so we do not print excesive lines...
    fprintf(o_file, "\t\t Y_%ld -> %.30s%s\n", i+1, freq.IC.strs[i], (strlen(freq.IC.strs[i]) > 30) ? "..." : "");

  cipher_status = true;

  end_IC:
    _freq_free(&(freq));
    if (alphabet)
      alphabet_clean(alphabet);
    return; 
}
```

Utilizaremos como texto cifrado el resultado de la siguiente ejecución:

```
❯ make vigenere
Compiling all...
Executable generated successfully!

❯ ./vigenere -m 26 -k CIFRADO -C -i plaintexts/el_quijote.txt -o ciphertexts/el_quijote_cifrado.txt
 ! @@@ VIGENERE @@@ ! 

───────┬────────────────────────────────────────────────────────────────────────
       │ File: ciphertexts/el_quijote_cifrado.txt
───────┼────────────────────────────────────────────────────────────────────────
   1   │ FWSHULXQBJUEOOOIS...
...

[Texto cifrado según la llamada a vigenere anterior]
```

Veamos un ejemplo de la ejecución del programa (considerar que el test de índice de coincidencia permite usar tamaño de n-grama, pero si no se lo especificamos será equivalente a pasarle **-l 0** y no lo tendrá en cuenta, por lo que no usará información proveniente de kasiski para sus cálculos):

```
❯ make IC
Compiling all...
Executable generated successfully!

❯ ./IC -m 26 -i ciphertexts/el_quijote_cifrado.txt -o salida.out
[TARDA DEMASIADO - VER EXPLICACIÓN ABAJO]

❯ ./IC -m 26 -l 2 -i ciphertexts/el_quijote_cifrado.txt
 ! @@@ IC TEST @@@ ! 
IC Results [for n-gram of length 2]: 
         IC found: 0.041928
         Keylength guessed: 2
         Cipher Strings (Y_i):
                 Y_1 -> FSUXBUOOSHAOVGEWAHARYFFRAZGUGF...
                 Y_2 -> WHLQJEOITDKZLSMMQGXASZGLGFAHKG...

❯ ./IC -m 26 -l 8 -i ciphertexts/el_quijote_cifrado.txt
 ! @@@ IC TEST @@@ ! 
IC Results [for n-gram of length 8]: 
         IC found: 0.075209
         Keylength guessed: 6006
         Cipher Strings (Y_i):
                 Y_1 -> FCCEPFPJEUGJQWWNEFOUPUKGDWRLVQ...
                 Y_2 -> WJTIMWI...
```

Nota: la ejecución para este texto sin la opción de utilizar n-gramas se ha llevado a cabo en un servidor aparte, puesto que es tremendamente lenta al estar comprobando tantos valores posibles de longitudes de texto, y la longitud de este es realmente considerable:

Para cifrar puede especificarse la opción `-C` o no ponerse nada. Sin embargo, para descifrar una cadena, necesita especificarse la opción `-D`.

```
❯ ls -latgG --block-size=KB ciphertexts/el_quijote_cifrado.txt
-rw-rw-r-- 1 803kB Oct 27 14:44 ciphertexts/el_quijote_cifrado.txt
```

Podemos observar que incluso después de algo más de dos horas seguía ejecutándose:

```
> ps -ef | grep "IC"
homeserv+ 1807484 1807177 99 12:54 pts/1    02:45:08 ./IC -m 26 -i ciphertexts/el_quijote_cifrado.txt -o salida.out
```

Esto se aplicará más adelante en el criptoanálisis total, por lo que veremos que se ha definido un umbral de n-grama en caso de que no se especifique ninguno, porque una ejecución de este estilo tarda horas y para nada termina resultando más eficiente o acertada.

### Criptoanálisis de Vigenère - Rotura mediante Kasiski e IC

Teniendo en cuenta los resultados que arroja el índice de coincidencia utilizando los resultados del test de kasiski, vemos que estamos cerca de poder encontrar la clave original. Si a las cadenas que nos arroja el IC les aplicamos un análisis de frecuencias, entonces podremos romper el cifrado de Vigenère partiendo de un texto cifrado conocido (puesto que simplemente tendremos que romper, de manera independiente, varios cifrados por desplazamiento). El código que utiliza el criptoanálisis que vamos a aplicar es el siguiente:

```c
void cryptanalyze_vigenere(const char *m, const char *ngram, FILE *i_file, FILE *o_file)
{
  #define __KSK_AUTO "auto" // string to match if auto mode has been selected
  #define __KSK_AUTO_THRS 8 // threshold of auto search mode. Means kasiski will never be executed for ngrams bigger than 8 in auto mode
  
  mpz_t mz, *key; // mz -> alphabet size; *key => key vector
  mpf_t fx, gx, fig, pi, m_g, _m_g;
  
  char *input = NULL,
       *output = NULL;

  ssize_t max_ngram;
  
  alphabet_t *alphabet;
  struct Frequency freq = {0};
  struct Frequency substrfreq = {0};

  size_t i, j, g, idx;
  char *keystring = NULL;
  bool key_found;
  
  if (!m || !i_file || !o_file)
  {
    ERROR...
  }

  mpz_init(mz);
  mpz_set_str(mz, m, 10L);

  alphabet = alphabet_init(mpz_get_ui(mz));
  if (!alphabet)
  {
    ERROR...
  }

  if (alphabet_loadFromFile(alphabet, _DICT_FNAME) == false)
  {
    ERROR...
  }

  input = _load_from_file(i_file, alphabet);
  if (!input)
  {
    ERROR...
  }

  freq.alphabet = alphabet;
  freq.textstring = input;
  freq.textlen = strlen(input);

  if (!ngram || strcmp(ngram, __KSK_AUTO) == 0) // test maximum value for kasiski ngrams
  {
    max_ngram = 0;
    for ( i=2; i<freq.textlen; i++ )
    {
      freq.ngram = i;
      _kasiski(&(freq));
      if (freq.Kasiski.ok == false)
        break;
      max_ngram = i;
      if (max_ngram >= __KSK_AUTO_THRS)
        break;
    }
    freq.ngram = max_ngram; // we don't need to run kasiski again...
    freq.Kasiski.ok = true;
  }
  else
  {
    freq.ngram = (ngram != NULL) ? atol(ngram) : 0;
    _kasiski(&(freq));
  }

  if (freq.Kasiski.ok)
    _IC(&(freq), true);
  else
    _IC(&(freq), false);

  if (!freq.IC.ok)
  {
    ERROR...
  }

  // key length guessed at this point

  _computeFrequency(&(freq), freq.textstring, freq.textlen);

  // lets find closest G in M_g, 0 <= G <= 25 (in english) that
  // gives us a M_g closest to 0.065

  key = (mpz_t*)calloc(freq.IC.keylength, sizeof(mpz_t));
  if (!key)
  {
    ERROR...
  }

  mpf_inits(m_g, _m_g, NULL);
  mpf_inits(fx, gx, pi, fig, NULL);
  
  substrfreq.alphabet = alphabet;

  for ( i=0; i<freq.IC.keylength; i++ ) // for each part of the key...
  {
    
    substrfreq.textstring = freq.IC.strs[i];
    substrfreq.textlen = strlen(substrfreq.textstring);
    substrfreq.chrs = (struct Param*)calloc(alphabet_getCurrentSize(alphabet), sizeof(struct Param));

    if (!substrfreq.chrs)
    {
      ERROR...
    }

    _computeFrequency(&(substrfreq), substrfreq.textstring, substrfreq.textlen);

    /**
     * Frequencies analysis
     * 
     * 0 <= i < keylength
     * 
     * f_i is each node in text frequency iterator
     * n', which is string length, is strlen ( freq.IC.strs[i] ) 
     *     better than freq.IC._M as it can be unpadded and not
     *     be fit exactly in ( total_length / keylength )
     * strs[i] is each substring cipher by each part of the key,
     *         so strs[0] will be all characters supposedly encrypted
     *         by the first part of the key!
     */

    mpz_init(key[i]);
    key_found = false;

    mpf_set_ui(m_g, 0L);
    
    for ( g=0; g<alphabet_getCurrentSize(alphabet); g++ )
    {
      // Lets see which value of g produces the closest
      // value of M_g to 0.065 defined as MG_IDEAL_VALUE
      mpf_set_ui(_m_g, 0L);
      for ( j=0; j<alphabet_getCurrentSize(alphabet); j++ )
      {
        // for each element of the alphabet...
        // -> Sum(0..25) <= (p_i * f_i+g) / (n') <- i = [0, 25]
        mpf_set_d(pi, alphabet_getNumProb(alphabet, j, ENGLISH)); // pi := p_i
        idx = ( (j + g) % alphabet_getCurrentSize(alphabet) ); // idx := (i + g) % 26 (for english alphabets)
        mpf_set_d(fig, substrfreq.chrs[ idx ].prob); // fig := ( f_i+g / n' ); as prob is not ocurrence but probability!
        mpf_mul(fx, pi, fig); // fx := p_i * f_i+g
        mpf_add(_m_g, _m_g, fx);
      }

      mpf_set_d(gx, MG_IDEAL_VALUE);

      mpf_sub(fx, m_g, gx);
      mpf_sub(gx, _m_g, gx);

      mpf_abs(fx, fx);
      mpf_abs(gx, gx);   

      if ( mpf_cmp(gx, fx) < 0 ) // ¿ |gx| < |fx|? If yes, new M_g calculated is better and so G is our value
      {
        mpf_set(m_g, _m_g); // save new M_g calculated as it's better than previous one
        mpz_set_ui(key[i], g); // we've found part of the key!
        key_found = true;
      }
    }
    if (substrfreq.chrs)
    {
      free(substrfreq.chrs);
      substrfreq.chrs = NULL;
    }

    if (!key_found)
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "Part [%ld/%ld] of key couldn't be found!", i+1, freq.IC.keylength);
      break;
    }
  }

  mpf_clears(m_g, _m_g, fx, gx, NULL);
  mpf_clears(pi, fig, NULL);

  if (!key_found)
    ERROR...
  
  keystring = (char*)calloc(freq.IC.keylength + 1, sizeof(char)); // +1 for trailing '\0'
  if (!keystring)
  {
    ERROR...
  }

  for ( i=0; i<freq.IC.keylength; i++ )
  {
    keystring[i] = alphabet_get_fromNum(alphabet, mpz_get_si(key[i]) );
  }
  keystring[i] = '\0';

  vigenere(DECIPHER, m, keystring, i_file, o_file);

  end_cryptanalyze_vigenere:
    _freq_free(&freq);
    if (alphabet)
      alphabet_clean(alphabet);
    if (keystring)
      free(keystring);
    mpz_clear(mz);
    if (key)
    {
      for ( i=0; i<freq.IC.keylength; i++ )
        mpz_clear(key[i]);
      free(key);
    }
    return;
}
```

Vamos a ver qué está haciendo el código de arriba:
  1. Recibidos el tamaño del alfabeto, un fichero de entrada y uno de salida, inicializa las variables correspondientes y carga el alfabeto que se va a utilizar. Opcionalmente se le puede especificar el tamaño del n-grama que usará. Si este valor no se especifica, hará sondeos hasta un máximo de longitud 8 (que se puede ver en los `#define` del principio).
  2. Habiendo cargado en la estructura el alfabeto y la cadena cifrada de entrada (`input`), procede a dilucidar qué hacer con el tamaño del n-grama. Tanto si se ha especificado la opción `-l auto` como si no se ha especificado, hará llamadas a la función `_kasiski` que vimos más arriba, hasta que o bien falle (y se quedará con el tamaño anterior a haber fallado, pues ese n-grama sí se encontró) o bien el n-grama llegue a ser de longitud 8, en cuyo caso saldrá y aceptará los candidatos a ser la longitud de clave que este test habrá arrojado.
  3. Si el test de Kasiski para alguno de estos valores no ha fallado (es decir, la longitud del n-grama buscado es igual o superior a 1 y hay una serie de longitudes candidatas a ser el tamaño de clave) el índice de coincidencia (la función `_IC` que se vio antes) los utilizará (flag de `use_kasiski_candidates` a *true*), **ahorrándose** hacer comprobaciones de los índices de coincidencia **para cada uno de los valores posibles entre 1 y la longitud del texto**.
  4. El `IC` nos dirá cuál es la longitud de clave que más posibilidades tiene de serlo, y nos habrá calculado las cadenas con los caracteres que hubieron sido cifrados por cada uno de los valores de la clave.
  5. Con una llamada a `_computeFrequency` para el texto completo de entrada. Así sacaremos la probabilidad de aparición de un caracter en todo el texto de entrada.
  6. En este punto solo queda hacer un análisis de frecuencias (como si fuese un cifrado por desplazamiento) para cada una de estas subcadenas:
     1. Generaremos una estructura de frecuencias para cada subcadena. Se utilizará para comprobar la frecuencia y probabilidad de aparición de un caracter del alfabeto en la cadena de entrada asociada (llamada a la función `_computeFrequency`).
     2. Con esta información, nos basaremos en que el sumatorio del cuadrado de la probabilidad de cada elemento en el idioma inglés arroja un valor de 0.065, por lo que comprobaremos todos los posibles valores con los que se pueden desplazar los caracteres (en $Z_{26}$ serán los valores $[0, 25]$). Para cada uno de los valores posibles, nos quedaremos con el que más cerca se quede del valor ideal $M_{g} = 0.065$.

        $\sum_{i=0}^{25} \frac{p_{i}*f_{i+g}}{n'}$, donde n' es el tamaño de la longitud de la subcadena que contiene los caracteres que se han cifrado por la parte de la clave que estamos tratando de romper.

     Así, para cada $g \in [0,25]$, el valor que produzca un valor de $M_g$ más cercano al ideal
        $\sum_{i=0}^{25} p_{i}^2 = 0.065$ será el valor con el que se creerá que se cifró esa cadena. 

  7. Para cada una de las cadenas hacemos este cálculo (_líneas 681 - 757_ de la función `cryptanalyze_vigenere`), consiguiendo recuperar de esta forma la clave original. Luego, llamamos a la función `vigenere` con la opción `DECIPHER` y con la cadena conteniendo la clave que hemos hallado.

Ejemplos de ejecución (volveremos a usar el mismo texto del quijote cifrado con vigenere y la clave que se usó en la demostración anterior):

```
❯ make cryptanalyze_vigenere
Compiling all...
Executable generated successfully!

❯ ./cryptanalyze_vigenere -m 26 -i ciphertexts/el_quijote_cifrado.txt -o quixote_decipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ !

❯ bat ciphertexts/el_quijote_cifrado.txt
───────┬────────────────────────────────────────────────────────────────────────
       │ File: quixote_decipher.txt
───────┼────────────────────────────────────────────────────────────────────────
   1   │ DONQUIJOTEDELAMANCHAMIGU...
...

Sería equivalente a realizar lo siguiente:

❯ ./cryptanalyze_vigenere -m 26 -l auto -i ciphertexts/el_quijote_cifrado.txt -o quixote_decipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ !

[Vemos que lo ha descifrado satisfactoriamente]
```

Y con un ejemplo más corto:

```
❯ bat ciphertexts/2cipher.txt
───────┬────────────────────────────────────────────────────────────────────────
       │ File: ciphertexts/2cipher.txt
───────┼────────────────────────────────────────────────────────────────────────
   1   │ CHREEVOAHMAERATBIAXXWTNXBEEOPHBSBQMQEQERBW
   2   │ RVXUOAKXAOSXXWEAHBWGJMMQMNKGRFVGXWTRZXWIAK
   3   │ LXFPSKAUTEMNDCMGTSXMXBTUIADNGMGPSRELXNJELX
   4   │ VRVPRTULHDNQWTWDTYGBPHXTFALJHASVBFXNGLLCHR
   5   │ ZBWELEKMSJIKNBHWRJGNMGJSGLXFEYPHAGNRBIEQJT
   6   │ AMRVLCRREMNDGLXRRIMGNSNRWCHRQHAEYEVTAQEBBI
   7   │ PEEWEVKAKOEWADREMXMTBHHCHRTKDNVRZCHRCLQOHP
   8   │ WQAIIWXNRMGWOIIFKEE
───────┴────────────────────────────────────────────────────────────────────────

❯ ./cryptanalyze_vigenere -m 26 -l auto -i ciphertexts/2cipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
THEALMONDTREEWASINTENTATIVEBLOSSOMTHEDAYSWERELONGEROFTENENDINGWITHMAGNIFICENTEVENINGSOFCORRUGATEDPINKSKIESTHEHUNTINGSEASONWASOVERWITHHOUNDSANDGUNSPUTAWAYFORSIXMONTHSTHEVINEYARDSWEREBUSYAGAINASTHEWELLORGANIZEDFARMERSTREATEDTHEIRVINESANDTHEMORELACKADAISICALNEIGHBORSHURRIEDTODOTHEPRUNINGTHEYSHOULDHAVEDONEINNOVEMBER
```

Probaremos ahora a cifrar el siguiente texto:

```
❯ cat plaintexts/entrada.txt
LOREM IPSUM DOLOR SIT AMET,...
```

El cuál cifraremos con la siguiente palabra:

```
LOREMIPSUMDOLO...
```

Es decir, lo cifraremos con sí mismo pero eliminando los caracteres que no reconoce.

```
❯ ./vigenere -m 26 -k LOREMIPSUMDOL...-i plaintexts/entrada.txt -o cipher.txt
 ! @@@ VIGENERE @@@ !
```

El resultado de hacer su criptoanálisis es el siguiente:

```
❯ ./cryptanalyze_vigenere -m 26 -l auto -i cipher.txt -o decipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
❯ cat decipher.txt
OEECFQK...
❯ ./cryptanalyze_vigenere -m 26 -l 1 -i cipher.txt -o decipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
❯ cat decipher.txt
OYEEEI...
❯ ./cryptanalyze_vigenere -m 26 -l 0 -i cipher.txt -o decipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
❯ cat decipher.txt
JHNE...
```

Como vemos, el ruido que genera es suficiente ya que estamos generando 1 desplazamiento por clave, por lo que es practicamente imposible poder hacer un analisis sobre repeticiones y estadistico que permita resolver la clave. El problema que esto presenta es que es tremendamente costoso manejar tamaños de clave que se asemejen al texto (imaginemos por un momento en el caso del quijote una clave del propio tamaño del texto cifrado: la clave tendria una longitud de 800k caracteres). En general, cuando cada una de las partes de la clave cifra un numero no muy grande de caracteres, es complicado darle la vuelta.

Veamos un ejemplo:

```
❯ cat entrada.txt
HOLA MUNDO, QUIERES UN CAFE. HACE UN DIA ESTUPENDO PARA HACER UN EXAMEN

❯ ./vigenere -m 26 -k HOLAMUNDO -C -i entrada.txt -o cipher.txt
 ! @@@ VIGENERE @@@ ! 

❯ cat cipher.txt
OCWAYOAGCXITEDYFXBJOQETUPHIURTAQMGXDLBOOBUEDVHQPRGHRAOTSY

[Sin Kasiski]
❯ ./cryptanalyze_vigenere -m 26 -l 0 -i cipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
AEDCSPNTOMENTOHRZILIRRGGEDCJCCMSTIREYOADXOTOETSWTAIENAIOS

[Con n-grama de longitud 2]
❯ ./cryptanalyze_vigenere -m 26 -l 2 -i cipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
AEDCSPNTOMENTOHRZILIRRGGEDCJCCMSTIREYOADXOTOETSWTAIENAIOS

[Con n-grama de longitud 4]
❯ ./cryptanalyze_vigenere -m 26 -l 4 -i cipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
AEDCSPNTOMENTOHRZILIRRGGEDCJCCMSTIREYOADXOTOETSWTAIENAIOS

[Con n-grama de hasta longitud 8]
❯ ./cryptanalyze_vigenere -m 26 -l auto -i cipher.txt
 ! @@@ CRYPTANALYZE VIGENERE @@@ ! 
AEDCSPNTOMENTOHRZILIRRGGEDCJCCMSTIREYOADXOTOETSWTAIENAIOS
```

Esto se debe a que aunque kasiski encuentre una distancia, el índice de coincidencia no tiene suficientes valores.