# set -x
#
# TODO: check if LLC size is correct
# LLC (combined) size = 2^27 bytes = 134217728 bytes = 33554432 ints
# Sweeping across size to obtain LLC curve => 67108864 ints

# increasing array size by 5% each time (minimum AppSize is 32)
SIZE=32
PREV_SIZE=0
DIV_AMOUNT=20
MAX_SIZE=67108864
RUN_COUNT=10

# sweep exponentially across array sizes
while [ $SIZE -le $MAX_SIZE ]
do
    # RUN STRIDED HERE
    for (( i=0; i<$RUN_COUNT; ++i ))
    do
        ./strided 1 $SIZE 1 $SIZE
        ./strided 1 $SIZE 4 $SIZE
        ./strided 1 $SIZE 16 $SIZE
        ./strided 1 $SIZE 64 $SIZE
        ./strided 1 $SIZE 256 $SIZE
    done

    NEW_SIZE=$((SIZE+(SIZE/DIV_AMOUNT)))
    SIZE_DIFF=$((NEW_SIZE-SIZE))
    PREV_SIZE=$SIZE

    if [ $SIZE_DIFF -lt 32 ]
    then
        SIZE=$((PREV_SIZE+32))
    else
        SIZE=$((SIZE+(SIZE/DIV_AMOUNT)))
    fi
done
