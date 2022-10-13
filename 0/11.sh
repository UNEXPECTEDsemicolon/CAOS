IFS="${IFS}:"
MAN_DIRS=""
for MAN_DIR in `manpath`
do
    if cd "${MAN_DIR}/man3" 2> /dev/null; then
        MAN_DIRS="${MAN_DIRS} ${MAN_DIR}/man3" 
    fi
done
for INPUT in `cat`
do
    for DIR in 
done

# read in