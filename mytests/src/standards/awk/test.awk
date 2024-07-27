#!/usr/bin/awk -f

BEGIN {
    while (getline < "test.awk.input" > 0) {
        printf("[%s] | [%s]\n", $1, $2);
    }
    close("test.awk.input")
    exit
}
END {
    printf("Argentina Vamos!\n")
}
