sizeLimit=12
echo "Size : $sizeLimit"
diff validation.txt print.txt
diff validation.txt print.txt | wc -l
a=$(diff validation.txt print.txt | wc -l)

echo "Number of lines different: $a"
echo "Validation limit: $sizeLimit"
if [ "$a" -gt "$sizeLimit" ]; then
    echo "Returning 1"
    return 1
else
    echo "Returning 0"
    return 0
fi
