#!/bin/bash

# Working example:
#./affine_mod -m 26 -a 1 -5 1 -b 2 45 -4 -i entrada.txt -o cipher.txt
#./cryptanalyze_aff_mod -m 26 -i cipher.txt -o decipher.txt
#diff -u -I '[A,Z]' entrada.txt decipher.txt

maxKLen=3 # Test for keys of length 3
msize=26

low=-20
hight=50

ci_file="entrada.txt"
co_file="cipher.txt"

di_file="cipher.txt"
do_file="decipher.txt"

successes=0
failures=0

echo "@@@ -- -- -- VALID PAIRS -- -- -- @@" > validpairs.txt # Initialize file

# Generate (a,b) pairs

function check { # check ${input_file} ${output_file} ${a} ${b}
  # successes=$((successes+1))
  DIFF=$(diff -u -I '[A,Z]' $1 $2)
  if [ "$DIFF" != "" ]; then
    failures=$((failures+1))
  else
    successes=$((successes+1))
    echo "--> a $3 b $4" >> validpairs.txt
  fi
}

for (( i=(($low)); i<=(($hight)); i++ ))
do
  a="$i"
  _a="$a"
  ./affine_mod -K 1 -m $msize -a "$a" -b 1 -i $ci_file -o $co_file # Just checking key...
  retVal=$?
  [ $retVal -ne 0 ] && continue
    for (( j=(($low)); j<=(($hight)); j++ ))
    do
      a="$_a" # for next iterations not giving back trash values
      a="$a $j"
      __a="$a"
      ./affine_mod -K 2 -m $msize -a "$a" -b 1 1 -i $ci_file -o $co_file # Just checking key...
      retVal=$?
      [ $retVal -ne 0 ] && continue
        for (( k=(($low)); k<=(($hight)); k++ ))
        do
          a="$__a"
          a="$a $k"
          ./affine_mod -K 3 -m $msize -a "$a" -b 1 1 1 -i $ci_file -o $co_file # Just checking key...
          retVal=$?
          [ $retVal -ne 0 ] && continue
          printf "\n\t a: $a || b: $b\n"
            for (( _i=(($low)); _i<=(($hight)); _i++ ))
            do
              b="$_i"
              _b="$b"
              for (( _j=(($low)); _j<=(($hight)); _j++ ))
              do
                b="$_b" # for next iterations not giving back trash values
                b="$b $_j"
                __b="$b"
                for (( _k=(($low)); _k<=(($hight)); _k++ ))
                do
                  b="$__b"
                  b="$b $_k"
                  # printf "\n\t a: $a || b: $b\n"
                  ./affine_mod -K $maxKLen -m $msize -a $a -b $b -i $ci_file -o $co_file
                  ./cryptanalyze_aff_mod -m $msize -i $di_file -o $do_file
                  check $co_file $do_file "$a" "$b"
                done
              done
            done
        done
    done
done

printf "Successes: $successes | Failures: $failures\n" > stdin
exit 0