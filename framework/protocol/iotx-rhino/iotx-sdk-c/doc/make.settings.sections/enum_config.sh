#! /bin/bash

M=$(ls section.*.*|cut -d. -f2|sort -nr|head -1)
iter=1
while (( iter <= M )); do
    PARA[${iter}]="$(ls -1 section.${iter}.*|xargs)"
#   echo "PARA[${iter}] = ${PARA[${iter}]}"
    iter=$(( iter + 1 ))
done

X=1
Y=1
Z=1
while (( X <= $(echo "${PARA[1]}"|awk '{ print NF }') )); do
    while (( Y <= $(echo "${PARA[2]}"|awk '{ print NF }') )); do
        while (( Z <= $(echo "${PARA[3]}"|awk '{ print NF }') )); do
            echo "$X:$Y:$Z"
#           cat << EOB > make.settings.sample.$X.$Y
            cat << EOB > make.settings.sample.$X.$Y.$Z
EOB
            PART_A=$(echo "${PARA[1]}"|awk "{ print \$${X} }")
            PART_B=$(echo "${PARA[2]}"|awk "{ print \$${Y} }")
            PART_C=$(echo "${PARA[3]}"|awk "{ print \$${Z} }")
            CMD="cat ${PART_A} ${PART_B} ${PART_C} >> make.settings.sample.$X.$Y.$Z"
#           CMD="cat ${PART_A} ${PART_B} >> make.settings.sample.$X.$Y"
            echo "CMD: ${CMD}"
            eval $CMD
            Z=$(( Z + 1 ))
        done
        Y=$(( Y + 1 ))
        Z=1
    done    
    X=$(( X + 1 ))
    Y=1
done
