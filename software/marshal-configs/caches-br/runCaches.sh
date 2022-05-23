# set -x
#
# TODO: check if LLC size is correct
# LLC (combined) size = 2^27 bytes = 134217728 bytes = 33554432 ints
# Sweeping across size to obtain LLC curve => 67108864 ints

# increasing array size by 10% each time (minimum AppSize is 32)
SIZE=32
PREV_RUN_SIZE=0
DIV_AMOUNT=10
MAX_SIZE=67108864
RUN_COUNT=10

cd root/caches

# delete results file if it already exists
if [ -f "../run_result.csv" ]
then
    rm ../run_result.csv
    touch ../run_result.csv
fi

# sweep exponentially across array sizes
while [ $SIZE -le $MAX_SIZE ]
do
    for (( i=0; i<$RUN_COUNT; ++i ))
    do
        ./caches $SIZE $SIZE 0 >> ../run_result.csv
    done

    NEW_SIZE=$((SIZE+(SIZE/DIV_AMOUNT)))
    SIZE_DIFF=$((NEW_SIZE-SIZE))
    PREV_SIZE=$SIZE

    if [ $SIZE_DIFF -lt 32 ]
    then
        SIZE=$((PREV_SIZE+32))
    else
        SIZE=$NEW_SIZE
    fi
done

#while [ $SIZE -le $MAX_SIZE ]
#do
#    if [ $SIZE -eq 32]
#    then
#        ./caches $SIZE $SIZE 0 > ../run_result.csv
#        for (( i=1; i<($RUN_COUNT-1); ++i ))
#        do
#            ./caches $SIZE $SIZE 0 >> ../run_result.csv
#        done
#        PREV_RUN_SIZE=$SIZE
#        SIZE=$((SIZE+32))
#    else
#        # if DIFF < 32, just increment AppSize by 32
#        DIFF=$((SIZE-PREV_RUN_SIZE))
#        if [ $DIFF -lt 32 ]
#        then
#            SIZE=$((PREV_RUN_SIZE+32))
#        else
#            for (( i=0; i<($RUN_COUNT-1); ++i ))
#            do
#                ./caches $SIZE $SIZE 0 >> ../run_result.csv
#            done
#            ./caches $SIZE $SIZE 0 >> ../run_result.csv
#            PREV_RUN_SIZE=$SIZE
#            SIZE=$((SIZE+(SIZE/DIV_AMOUNT)))
#        fi
#    fi
#done

poweroff

