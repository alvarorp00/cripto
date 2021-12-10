#!/bin/bash -x
echo "[INFO] Usage: $0 <algorithm> <key> [<iv>]"

openssl enc -base64 -e $1 -in plaintexts/lorem_ipsum.txt -out ciphertexts/lorem_ipsum.txt -K $2 -iv $3;
openssl enc -base64 -d $1 -in ciphertexts/lorem_ipsum.txt  -out decryptedtexts/lorem_ipsum.txt -K $2 -iv $3;
diff -U 0 plaintexts/lorem_ipsum.txt decryptedtexts/lorem_ipsum.txt | wc -c;
diff <(xxd plaintexts/lorem_ipsum.txt) <(xxd ciphertexts/lorem_ipsum.txt) | wc -c;

exit 0;
