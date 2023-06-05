sizeLimit=2
a=$(diff validation.txt print.txt | wc -c)

if [ "$a" -gt "$sizeLimit" ]; then
    exit 1
else
    exit 0
