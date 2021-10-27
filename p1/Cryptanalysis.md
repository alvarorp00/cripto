# Modification of the Affine Cipher

 - Cryptography Basis | EPS - UAM

##  1. <a name='Motivation'></a>Motivation

The modification of the affine cipher consists in using a polialphabetic key instad of it's monoalphabetic key. 

{TODO: EXPLAIN HOW THIS WORKS AND DEMONSTRATE IT WORKS, PURPOSE}

##  2. <a name='CryptanalysisoftheModifiedAffineCipher'></a>Cryptanalysis of the Modified Affine Cipher

This cryptogram can be cipher text attacked, such as the original affine cipher but considering at least more variables in here.

The initial conditions and restrictions are the same applied for the classic affine cipher, but with a tricky new thing: **gcd(a, m) = 1** must be fullfilled for every part of the vector given; this means that if _a_ has a length of 8, then: **gcd(a[i], m)** for every *i in [0,7]*.

Remember how this cryptogram does the cipher and decipher process:

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

Once having seen how it works, we can continue with the cryptanalysis. A few explanation can be found below.

In vigenere attack we guide from frequency in texts.

f_0 / n' , ... , f_25 / n'
f_ki / n' , ... , f_25+ki / n'

Notice: n' => characters encrypted by each parameter of the key. In other words,
the length of the substring constructed (this substring is builded with all the characters that are thought to have been encrypted by the same key value, K_i).

But as we're not in a shift-based cipher but in a affine modified one,
we have:

f0 / n' , ... , f25 / n'

| | | | | | | | | | | | | | | | |
v v v v v v v v v v v v v v v v v

(x + k_i) % 26 => (x*a + b) % 26

Notice that f_ki in a vigenere attack references the shift value that is going to be applied to the i_th element of the alphabet.

So, starting in 0 we would have:

f_b / n' , ... , f_25*a+b / n'

-> f_b at first as 0*a+b it's equivalent to just shifting b positions...

-> At this moment we must remember that the ideal probability distribution
p0 , ... , p25 subscripts above formula is evaluated modulo 26.

So p_i means distribution probability in frequence table given in
alphabet (check config/dictionary.json file or visit this [link](https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html)).
  
  So instead of:
  
  M_g <= Sum(0..25) := (p_i * f_(i + g)) / (n')
  
  We'll be replacing:
  
f_(i + g) => f_(a*i + b)

And the pair (a,b) that gets closer
to de IC ideal value (0.065 used in ic)
will be the one selected for that row of
elements

Once done, we'll check if (a,b) is possible in Z_26 by
calculating it's gcd(a, mz) and if it's not possible we'll skip
those values and continue with next one closer to 0.065.

Another option would've been trying all (a,b) combinations
directly but won't use statistic information
as the other method.

For guessing (a, b), we'll use concepts of the
cryptanalysis of the normal affine cipher:
  
-> match main occurrence in cipher text
   with main occurrence in language
-> match second occurrence in cipher text
   with second main occurrence in language
-> if not, match third ocurrence in cipher text
   with second main ocurrencence in language
-> and so on...

So, both combined, we'll try to search an (a,b) pair
(both a and b in [0, 25]) and then compute it's M_g
value. For all (a, b) available (this means that 
gcd (a, mz) == 1), we'll get only the one closer
to ideal IC value, which is 0.065 as we've stated
above.

For IC test, we can avoid searching for specific n-grams and let the algorithm search most suitable itself. This can be easily codified but with two options considered:
  1. Find the first one that produces critical values above a threshold related to the ideal value for, in this case, 0.065.
  2. Compute all values which text can be dividied and find the closest one to the ideal value.

In general, we could also do a two-round check with two algorithms, which is what we're doing in vigenere attack: combine kasiski and IC methods but with the criteria set in the first option: find the first key values that kasiski and IC can accept.

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
    size_t keylength; // guessed key length
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
   1   │ CHREEVOAHMAERATBIAXXWTNXBEEOPHBSBQMQEQERBW
   2   │ RVXUOAKXAOSXXWEAHBWGJMMQMNKGRFVGXWTRZXWIAK
   3   │ LXFPSKAUTEMNDCMGTSXMXBTUIADNGMGPSRELXNJELX
   4   │ VRVPRTULHDNQWTWDTYGBPHXTFALJHASVBFXNGLLCHR
   5   │ ZBWELEKMSJIKNBHWRJGNMGJSGLXFEYPHAGNRBIEQJT
   6   │ AMRVLCRREMNDGLXRRIMGNSNRWCHRQHAEYEVTAQEBBI
   7   │ PEEWEVKAKOEWADREMXMTBHHCHRTKDNVRZCHRCLQOHP
   8   │ WQAIIWXNRMGWOIIFKEE

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
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
  }

  ic = &(freq->IC);
  ic->ok = false;

  // now we have in k_divisors a set of all elements that divide len
  // although we should've performed previous computations in a more
  // efficient way, we're not taking care of that now

  // We're performing now following operation:
  //   y = y1y2...yn
  //   | | | | | | |
  //   | | | | | | |
  //   v v v v v v v
  //   y1 = y1 ym+1 y2m+1...
  //   y1 = y2 ym+2 y2m+2...
  //   y1 = y3 ym+3 y2m+3...
  //   ... ... ... ... ...
  //   y1 = ym y2m y3m...
  //   | | | | | | | |
  //   v v v v v v v v

  freq->chrs = (struct Param*)calloc(alphabet_getCurrentSize(freq->alphabet), sizeof(struct Param));

  if (!freq->chrs)
  {
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s\n", strerror(errno));
    goto _end_IC;
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
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto _end_IC;
    }

    IC = 0.0;
    for (j = 0; j < m; j++)
    {
      _M = (size_t)ceil((float)(freq->textlen) / (float)(m)); 
      substr[j] = (char*)calloc(_M + 1, sizeof(char)); // +1 for trailing '\0'
      if (!substr[j])
      {
        #line __LINE__ __FILE__
        snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
        goto _end_IC;
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
    #line __LINE__ __FILE__
    snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
    goto _end_IC;
  }

  for (j = 0; j < ic->keylength; j++)
  {
    ic->strs[j] = (char*)calloc(ic->_M + 1, sizeof(char)); // +1 for trailing '\0'
    if (!ic->strs[j])
    {
      #line __LINE__ __FILE__
      snprintf(errbuff, ERRBUFF_LEN, "%s", strerror(errno));
      goto _end_IC;
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

Primero que todo, vemos que además de la estructura, recibe un flag `use_kasiski_candidates`. Este flag nos permitirá diferenciar entre comprobar todos los valores posibles entre los que podemos partir la cadena de entrada o solamente los que el test de kasiski arroja como altamente probables para ser longitud de clave (es decir, todos los valores entre los que podremos dividir la distancia que el test de kasiski ha encontrado). Para cada uno de estos valores (sean las distintas longitudes de la cadena total de entrada, que no tienen que coincidir en longitud ya que el _padding_ no es necesario, pues puede haber un trozo de una cadena que no haya llegado a cifrarse con la última parte de la clave; sean las disntitas longitudes calculadas en el método anterior) se procede de la siguiente forma:
  1. Iniciamos poniendo el valor del IC resultante a 0. Lo mismo con la longitud de la cadena que se cree haber hayado.
  2. Escogemos el número de subcadenas en las que vamos a partir el texto (según lo que se acaba de contar). Este punto es algo sutil, pues realmente estamos haciendo lo siguiente:
     - Si el valor que estamos comprobando para la longitud de la cadena es **klength**, calculamos **_M** de la siguiente forma: **_M = strlen(texstring) / klength**. De esta forma, en **_M** tendremos la cantidad de caracteres que cada parte de la clave habrá cifrado como mucho.
  3. Reservamos el número de subcadenas correspondiente al valor de la longitud de la clave, y por cada una de estas reservamos el número de caracteres que cada una habría cifrado, pero utilizando la función **ceil** puesto que si la división no es entera, una cadena podría quedar más corta. Así ajustamos el valor, por lo que podría darse el caso de que la subcadena correspondiente a la(s) última(s) parte de la clave estuviera(n) rellena(s) de 0's, aunque como veremos, no es ningún problema.
  4. Por cada subcadena correspondiente a los caracteres cifrados por la i-ésima posición de la clave, copiamos los caracteres que esta habría cifrado. Para ello, por cada iteración **j** copiamos el **j-ésimo** caracter desplazado un número **i** de veces (para que se corresponda con el trozo de la clave que lo ha cifrado).
  5. Una vez tenemos en la subcadena los caracteres que la posición i-ésima de la cadena hubiera cifrado, calculamos las frecuencias de los caracteres del alfabeto en esa subcadena (llamada a la función `_computeFrequency`), y después calculamos el índice de coincidencia parcial para esa cadena (llamada a `_computePartialIC`).
  6. Este proceso se repite para todas las posiciones de la posible clave. Se suman cada uno de los índices de coincidencia parciales y finalmente se divide entre el número de subcadenas en que lo hemos dividido (es decir, entre la longitud de la clave). Esta media será el índice de coincidencia total para esa longitud de cadena de cifrado.
  7. Aquí comparamos este nuevo valor con el que tenemos guardado en la estructura (que será el valor final). En caso de estar más cerca del índice de coincidencia ideal (0.065), actualizaremos en la estructura (que será donde recuperaremos los valores de respuesta) los valores para la longitud de clave, índice de coincidencia encontrado y número (máximo) de caracteres que cada posición de la clave habría cifrado.
  8. Finalmente, puesto que vamos liberando la memoria asociada a las cadenas calculadas, para el valor resultante de longitud de clave generamos las subcadenas que cada parte de la clave habría cifrado, y las guardamos en el campo `freq->IC.strs`, de donde podremos recuperarlas después. Si la longitud de la clave es **keylength**, entonces tendremos que realizar un análisis de frecuencias este número de veces (equivalente a atacar un número **keylength** de textos cifrados por un cifrador por desplazamiento _shift cipher_).

### Criptoanálisis de Vigenère - Rotura mediante Kasiski e IC

// TODO