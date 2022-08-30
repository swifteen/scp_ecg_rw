#!/bin/bash
#==============================================
# 作者: zq1219
# 说明: iMAC工程代码格式化脚本
#==============================================
array=`git diff-index --cached  --name-only HEAD`

for name in ${array}
do
    extension=${name##*.}
    if [[ ${extension} == "h" || ${extension} == "cpp" || ${extension} == "c" ]]
    then
#        echo "###astyle###"$name
        if [ -f ${name} ]; then
        #去掉window下的^M
        #        fromdos ${name}
        #去掉文件头中的BOM标记
                sed -i '1s/^\xEF\xBB\xBF//' ${name}
        #格式化代码
        #       astyle --style=otbs -s4 -S -N -L -m0 -M40 -f -U -k1 -W1  -j -xL -n ${name}
                astyle --style=bsd -s4 -S -L -m0 -M40 -f -U -k1 -W1 -j -xL -n -c -p -H -xb -y -xe -Y ${name}
        git add ${name}
        fi
    fi
done
