sizeLimit=12
echo "Size : $sizeLimit"
a=$(diff validation.txt print.txt | wc -l)

echo "Number of lines different: $a"
echo "Validation limit: $sizeLimit"
if [ "$a" -gt "$sizeLimit" ]; then
    return 1
else
    return 0
fi
