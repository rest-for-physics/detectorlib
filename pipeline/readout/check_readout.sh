sizeLimit=12
echo $sizeLimit
a=$(diff validation.txt print.txt | wc -l)

echo "Number of lines different: $a"
echo "Validation limit: $sizeLimit"
if [ "$a" -gt "$sizeLimit" ]; then
    exit 1
else
    exit 0
fi
