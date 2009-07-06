#!/bin/bash
SERVERFILE="../ketama.servers"

echo -en "Checking if hashing works as expected..."

OURHASH=`LD_LIBRARY_PATH=. ./ketama_test ${SERVERFILE} | md5sum | cut -b1-32`

if [ "$OURHASH" != "5672b131391f5aa2b280936aec1eea74" ]
then
	echo "FAIL"
	exit 1
else
	echo "OK"
fi

echo -ne "Testing distribution..."

# stitched together from some oneliners i had in my backbuffer.. :\
PC_DIFF=`
(
awk '$1&&$1!~/^#/{tots[$1]+=$2;tot+=$2}END{for(s in tots){printf("EXPECTED\t%f\t%s\n",tots[s]/tot,s);}}' ../ketama.servers ${SERVERFILE} | sort -k 2
./ketama_test ${SERVERFILE} | awk '$3{tots[$3]++;tot++}END{for(s in tots){printf("OBSERVED\t%f\t%s\n",tots[s]/tot,s);}}' | sort -k 2
) | awk ' 

{ if($1=="OBSERVED") ob[$3]=$2;
  if($1=="EXPECTED") ex[$3]=$2;
}END{
  for(k in ob) { 
    diff=ex[k]-ob[k]; if(diff<0) diff*=-1; 
    totdiff+=diff;
    #printf("%s\t%f\n",k,diff); 
  }
  #printf("Total diff: %f\n", totdiff);
  printf("%d",100*totdiff);
}'
`
# anything >= 10% off the theoretical distribution is a fail.
if [ ${PC_DIFF} -lt 10 ]
then
	echo "OK (${PC_DIFF}% err)"
else
	echo "FAIL (${PC_DIFF}% err)"
	exit 2
fi

