#!bin/bash

# O argumento de linha $1 precisa ter formato MMSS
# (por exemplo 130 para 1 minuto e 30 segundos)

if ! [ -d screenshots ]
then
	mkdir screenshots
fi

filename="screenshots/`date "+%Y%m%d%H%M%S"`"
id=`xwininfo -display :0`
id=${id/#*Window id: /}
id=${id/% */}

end=$((`date "+%Y%m%d%H%M%S"`+$1))
while [[ `date "+%Y%m%d%H%M%S"` < $end ]]
do
	xwd -id $id -silent -out $filename-$i.xwd
	sleep .01
	i=$((i+1))
done

mogrify -format png screenshots/*.xwd
rm -f screenshots/*.xwd

