#!/usr/bin/bash

destd=/media/ctap/4A21-0000/BENCAM-PRODS

for f in RESULTS-BEN*
do
    echo $f
    ./C-TAP-PRODUCTS.sh $f $destd
done
