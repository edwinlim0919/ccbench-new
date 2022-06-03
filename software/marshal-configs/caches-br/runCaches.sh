# TODO: check if LLC size is correct
# LLC (combined) size = 2^27 bytes = 134217728 bytes = 33554432 ints
# Sweeping across size to obtain LLC curve => 67108864 ints

# increasing array size by 10% each time (minimum AppSize is 32)
SIZE=32
PREV_SIZE=0
DIV_AMOUNT=10
MAX_SIZE=67108864
NUM_ITERATIONS=100
RUN_COUNT=10

cd /root/caches

# clear results file if it already exists
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
        ./caches $SIZE $NUM_ITERATIONS 0 >> ../run_result.csv
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

poweroff
