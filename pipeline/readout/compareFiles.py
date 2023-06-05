import sys

with open('print.txt') as file_1:
    file_1_text = file_1.readlines()

with open('validation.txt') as file_2:
    file_2_text = file_2.readlines()

stop_removing = 0
while( not stop_removing ):
    if( len(file_1_text) == 0 ):
        break
    if( file_1_text[0].rstrip().find("Number of readout planes" ) >= 0):
        stop_removing = 1
    file_1_text.pop(0)

stop_removing = 0
while( not stop_removing ):
    if( len(file_2_text) == 0 ):
        break
    if( file_2_text[0].rstrip().find("Number of readout planes" ) >= 0):
        stop_removing = 1
    file_2_text.pop(0)

result = 0
n = 0
for line in file_1_text:
    if( line.rstrip() != file_2_text[n].rstrip() ):
        print("XX:" + line.rstrip() )
        print("YY:" + file_2_text[n].rstrip() )
        result = 1
    n = n + 1

print("Readout validation result " + str(result) )
sys.exit(result)
