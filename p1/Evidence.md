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