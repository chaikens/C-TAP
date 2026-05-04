#!/usr/bin/bash -v

YES_OR_NO="yes"

if [ $YES_OR_NO = "yes" ]
then
    echo YES_OR_NO is yes
fi

YES_OR_NO="no"

if [ $YES_OR_NO = "yes" ]
then
    echo YES_OR_NO is yes
else
    echo YES_OR_NO is not yes
    echo YES_OR_NO is $YES_OR_NO
fi

YES_OR_NO=

#DONT DO THIS!!!
if [ $YES_OR_NO = "yes" ]
then
    echo YES_OR_NO is yes
else
    echo YES_OR_NO is not yes
    echo YES_OR_NO is $YES_OR_NO
fi


#DONT DO THIS!!!
if [ $YES_OR_UNDEFINED = "yes" ]
then
    echo YES_OR_UNDEFINED is yes
else
    echo YES_OR_UNDEFINED is not yes.
    echo YES_OR_UNDEFINED is $YES_OR_UNDEFINED
fi

YES_OR_NO="yes"

if [ ${YES_OR_NO}"" = "yes" ]
then
    echo YES_OR_NO is yes
else
    echo YES_OR_NO is not yes
    echo YES_OR_NO is ${YES_OR_NO}
fi

YES_OR_NO="no"

if [ ${YES_OR_NO}"" = "yes" ]
then
    echo YES_OR_NO is yes
else
    echo YES_OR_NO is not yes
    echo YES_OR_NO is ${YES_OR_NO}
fi

YES_OR_NO=

if [ ${YES_OR_NO}"" = "yes" ]
then
    echo YES_OR_NO is yes
else
    echo YES_OR_NO is not yes
    echo YES_OR_NO is ${YES_OR_NO}
fi

if [ ${UNDEFINED}"" = "yes" ]
then
    echo UNDEFINED is yes
else
    echo UNDEFINED is not yes
    echo UNDEFINED is $UNDEFINED
fi

if true
then
    echo return code is $?
    echo true is true
fi

if false
then
    echo return code is $?
    echo false is true
else
    echo return code is $?
    echo false is false
fi

type true

which true

type false

which false

which false

type test

which test

if true
then
    echo true is true
fi

if [ true ]
then
    echo true is true
fi

if test true
then
    echo true is true
fi

if [ $UNDEFINED ]
then
    echo UNDEFINED is true
else
    echo UNDEFINED is false
fi

EMPTYVAL=

if [ $EMPTYVAL ]
then
    echo EMPTYVAL is true
else
    echo EMPTYVAL is false
fi

if [ $EMPTYVAL = ""]
then
    echo EMPTYVAL is null string
else
    echo EMPTYVAL is false
fi

NULLSTRING=""

if [ $NULLSTRING ]
then
    echo NULLSTRING is true
else
    echo NULLSTRING is false
fi

if [ $NULLSTRING = ""]
then
    echo NULLSTRING is null string
fi


if ls bashControlTutorial.bash 
then
    echo ls this script is true
else
    echo ls this script is false
fi

if ls NOTbashControlTutorial.bash 
then
    echo ls non-file is true
else
    echo ls non-file is false
fi

if ! ls NOTbashControlTutorial.bash
then
    echo We correctly detected a command failure
fi


#DONT DO THIS!!!
if [ ls bashControlTutorial.bash ]
then
    echo ls this script is true
else
    echo ls this script is false
fi

#DONT DO THIS!!!
if [ ! ls bashControlTutorial.bash ]
then
    echo ls this script is false
else
    echo ls this script is true
fi

#DONT DO THIS!!!
if [ ls NOTbashControlTutorial.bash ]
then
    echo ls non-file is true
else
    echo ls non-file is false
fi

#DONT DO THIS!!!
if [ ! ls NOTbashControlTutorial.bash ]
then
    echo ls non-file is false
else
    echo ls non-file script is true
fi

#USE THIS PATTERN TO RUN A COMMAND AND
#ACT IF IT FAILED

true
ret=$?
if [ $ret != 0 ]
then
    echo shouldnt happen
else
    echo ret non-zero is false, good ret
fi

true
if [ $? ]
then
    echo good
fi

false
if [ ! $? ]
then
    echo good
fi



false
ret=$?
if [ $ret != 0 ]
then
    echo non-zero, an error, is reported
else
    echo shouldnt happen
fi

REUSE_BMPS=no

if [ ${REUSE_BMPS}"" != "yes" ]
then
echo we will not reuse bitmaps
fi

REUSE_BMPS=yes
if [ ${REUSE_BMPS}"" != "yes" ]
then
    echo wrong
else
    echo we will reuse bitmaps
fi

