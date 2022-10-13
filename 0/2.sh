#!/usr/bin/env bash

declare -A matrix

swap () {
    for j in {0..$N}
    do
        temp=${matrix[$1,$j]}
        matrix[$1,$j]=${matrix[$2,$j]}
        matrix[$2,$j]=$temp
    done
}

EPS="0.0000000000000001" 
equal () {
    [ $(echo "define abs (x) { if (x>=0) { return x; } else { return -x; } } abs($1 - $2) < $EPS" | bc -l) -eq 1 ]
}

i=0
for str in $(cat $1 | sed -r s/[[:space:]]+//g)
do
    j=0
    for elem in $(echo "$str" | sed -r "s/,/\ /g")
    do
        matrix[$i,$j]="$elem"
        j=$(($j+1))
    done
    i=$(($i+1))
done

N=$(($i))
for ((k=0; k<$N; k++))
do
    i=$k;
    while equal ${matrix[$i,$k]} 0
    do
        i=$(($i+1))
    done
    if [ $i -ne $k ]; then
        swap $k $i
        i=$k
    fi
    for ((j=$N; j>=k; j--))
    do
        matrix[$i,$j]=$(echo "${matrix[$k,$j]} / ${matrix[$k,$k]}" | bc -l)
    done
    for ((i=`echo $(($k+1))`; i<$N; i++))
    do
        for ((j=$N; j>=$k; j--))
        do
            matrix[$i,$j]=$(echo "${matrix[$i,$j]} - ${matrix[$k,$j]} * ${matrix[$i,$k]}" | bc -l)
        done
    done
done

declare -A ans
for ((i=`echo $(($N-1))`; i>=0; i--))
do
    ans[$i]=${matrix[$i,$N]};
    for ((j=`echo $(($i+1))`; j<$N; j++))
    do
        ans[$i]=$(echo "${ans[$i]} - ${matrix[$i,$j]} * ${ans[$j]}" | bc -l)
    done
done

for ((i=0; i<$N; i++))
do
    echo "${ans[$i]}" | sed -r 's/^(-?)\./\10./'
done

unset ans
unset matrix