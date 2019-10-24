#!/bin/bash



IN_PATH=~/machine_learning/github/Sound-Datasets/ESC-50
OUT_PATH=~/machine_learning/workspace/Working_on_datasets/ESC-50-splitted-no-silence-half-second


echo Input path: $IN_PATH
echo Output path: $OUT_PATH

size=40000

ONE=-A-
TWO=-B-
THREE=-C-
FOUR=-D-
FIVE=-E-
SIX=-F-
SEVEN=-G-
EIGTH=-H-
NINE=-I-
TEN=-J-


cd $IN_PATH



OIFS=$IFS; IFS=$'\n'; array=($(ls )); IFS=$OIFS; echo "${array[0]}"

echo size of : ${#array[@]}


for revolution in "${array[@]}"; do
    
	echo $revolution
	mkdir -p ${OUT_PATH}/$revolution
	cd ${revolution}
        inside_array=($(ls ));
	for inside_revolution in "${inside_array[@]}"; do
		echo "Listing: ${inside_revolution}"
        #if [ ${revolution} = 'glass_breaking' ];then
         name=${revolution}
		
        
        sox "${IN_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${inside_revolution}" silence 1 0.1 1% -1 0.1 1%
       	 
            
        sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$ONE${inside_revolution}" trim 0 0.5
            file1=${OUT_PATH}/${revolution}/${name}$ONE${inside_revolution} 
            filesize1=$(wc -c $file1 | awk '{print $1}')
            
            if [ $filesize1 -lt $size ];then
               rm $file1
            fi

        sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$TWO${inside_revolution}" trim 0.5 0.5
            file2=${OUT_PATH}/${revolution}/${name}$TWO${inside_revolution} 
            filesize2=$(wc -c $file2 | awk '{print $1}')
            
            if [ $filesize2 -lt $size ];then
               rm $file2
            fi


         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$THREE${inside_revolution}" trim 1 0.5  
            file3=${OUT_PATH}/${revolution}/${name}$THREE${inside_revolution}
            filesize3=$(wc -c $file3 | awk '{print $1}')
            if [ $filesize3 -lt $size ];then
               rm $file3
            fi

         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$FOUR${inside_revolution}" trim 1.5 0.5
            file4=${OUT_PATH}/${revolution}/${name}$FOUR${inside_revolution} 
            filesize4=$(wc -c $file4 | awk '{print $1}')
            if [ $filesize4 -lt $size ];then
               rm $file4
            fi

         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$FIVE${inside_revolution}" trim 2 0.5  
            file5=${OUT_PATH}/${revolution}/${name}$FIVE${inside_revolution} 
            filesize5=$(wc -c $file5 | awk '{print $1}')
            if [ $filesize5 -lt $size ];then
               rm $file5
            fi

         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$SIX${inside_revolution}" trim 2.5 0.5
            file6=${OUT_PATH}/${revolution}/${name}$SIX${inside_revolution}
            filesize6=$(wc -c $file6 | awk '{print $1}')
            if [ $filesize6 -lt $size ];then
               rm $file6
            fi
        
         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$SEVEN${inside_revolution}" trim 3 0.5
            file7=${OUT_PATH}/${revolution}/${name}$SEVEN${inside_revolution}
            filesize7=$(wc -c $file7 | awk '{print $1}')
            if [ $filesize7 -lt $size ];then
               rm $file7
            fi

         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$EIGHT${inside_revolution}" trim 3.5 0.5
            file8=${OUT_PATH}/${revolution}/${name}$EIGHT${inside_revolution}
            filesize8=$(wc -c $file8 | awk '{print $1}')
            if [ $filesize8 -lt $size ];then
               rm $file8
            fi

         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$NINE${inside_revolution}" trim 4 0.5
            file9=${OUT_PATH}/${revolution}/${name}$NINE${inside_revolution}
            filesize9=$(wc -c $file9 | awk '{print $1}')
            if [ $filesize9 -lt $size ];then
               rm $file9
            fi
        
         sox "${OUT_PATH}/${revolution}/${inside_revolution}" "${OUT_PATH}/${revolution}/${name}$TEN${inside_revolution}" trim 4.5 0.5
            file10=${OUT_PATH}/${revolution}/${name}$TEN${inside_revolution}
            filesize10=$(wc -c $file10 | awk '{print $1}')
            if [ $filesize10 -lt $size ];then
               rm $file10
            fi
         
         rm ${OUT_PATH}/${revolution}/${inside_revolution}
        #fi
    done

    
    cd ..
done



