# LLC (combined) size = 2^27 bytes = 134217728 bytes = 33554432 ints
# Sweeping across size to obtain LLC curve => 67108864 ints

# increasing array size by 10% each time (minimum AppSize is 32)
SIZE=32
PREV_SIZE=0
DIV_AMOUNT=5
#MAX_SIZE=67108864
MAX_SIZE=1024
NUM_ITERATIONS=1000
RUN_COUNT=5

cd /root/strided

# make sure results file exists
if [ -f "../stride-1.csv" ]
then
    rm ../stride-1.csv
fi
touch ../stride-1.csv

# sweep exponentially across array sizes
while [ $SIZE -le $MAX_SIZE ]
do
    for (( i=0; i<$RUN_COUNT; ++i ))
    do
        ./strided 1 $SIZE 1 $NUM_ITERATIONS >> ../stride-1.csv
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
