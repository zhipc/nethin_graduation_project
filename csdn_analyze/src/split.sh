#!/bin/bash

RETVAL=1

WORKDIR=$(dirname ${0})

while test 0 -ne ${RETVAL}
do
    ${WORKDIR}/split.py
    RETVAL=${?}
done > ${WORKDIR}/split.error 2>&1